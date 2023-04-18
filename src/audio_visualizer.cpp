#include "include/audio_visualizer.hpp"

#include <QPainter>
#include <QWidget>
#include <qmath.h>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QVector2D>
#include <QCoreApplication>
#include <QSysInfo>
#include <QtGlobal>

#include <algorithm>
#include <limits>
#include <complex>
#include <fftw3.h>


#define DFT_SIZE 2048
#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 16
#define SAMPLE_TYPE qint16
#define MAX_MAGNITUDE 10'000
#define BUCKET_SIZE 8


namespace audio_app {


const double FPS = 50;
const double MSPF = 1000 / FPS;
const double FREQ_SCALE = 20;


audio_visualizer::audio_visualizer(QWidget *parent) 
    : QWidget(parent)
    , m_probe(this)
    , m_dft_input(fftw_alloc_real(DFT_SIZE * 2))
    , m_dft_output(fftw_alloc_complex(DFT_SIZE))
    , m_magnitudes(DFT_SIZE / 2)
    , m_old_spectrum(DFT_SIZE / (2 * BUCKET_SIZE))
    , m_spectrum(DFT_SIZE / (2 * BUCKET_SIZE))
    , m_new_samples_arrived(false)
    , m_frames_stacked(1)
    , m_max_frames_stacked(1)
    , m_update_timer(this)
{
    setWindowTitle("Visualizer");

    m_dft_plan = fftw_plan_dft_r2c_1d(DFT_SIZE, m_dft_input, m_dft_output, FFTW_MEASURE);

    connect(&m_probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(process_buffer(QAudioBuffer)));

    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_update_timer.start(MSPF);
}

void audio_visualizer::paintEvent(QPaintEvent *) {
    if (m_new_samples_arrived) {
        m_new_samples_arrived = false;
        m_max_frames_stacked = m_frames_stacked;
        m_frames_stacked = 1;

        process_dft();
        compute_magnitudes();
        backup_spectrum();
        compute_spectrum();
    }

    draw_spectrum_rects();
    // draw_spectrum_peaks();

    if (m_internal_player->state() == QMediaPlayer::PlayingState) {
        m_frames_stacked++;
    }
}

double audio_visualizer::linear_interpolation(double x1, double x2, double step, double max) {
    return x1 + qPow(step / max, 1.0) * (x2 - x1);
}

void audio_visualizer::draw_spectrum_peaks() {
    // QPainter painter(this);
    // painter.setPen(QPen(Qt::black, 3));

    // int w = width() * 4 / 5;
    // int h = height() * 4 / 5;
    // int dx = (width() - w) / 2;
    // int dy = (height() - h) / 2;

    // // Calculate the number of frequency values and the spacing between them
    // qint64 num_freq = DFT_SIZE / 2;
    // double freq_spacing = static_cast<double>(w) / num_freq;

    // std::complex<double> *dft_data1 = reinterpret_cast<std::complex<double> *>(m_old_dft_output);
    // std::complex<double> *dft_data2 = reinterpret_cast<std::complex<double> *>(m_dft_output);

    // for (int i = 0; i < num_freq - BUCKET_SIZE + 1; i += BUCKET_SIZE) {
    //     // Calculate the x and y positions of the current and next frequency values
    //     double x1 = dx + i * freq_spacing;
    //     double x2 = dx + (i + BUCKET_SIZE) * freq_spacing;

    //     // Calculate magnitudes for 2 adjacent frequencies

    //     int current_magnitude1 = 
    //         FREQ_SCALE * m_internal_player->volume() / 10
    //         * qLn(std::abs(dft_data1[i]) / MAX_MAGNITUDE + 1);

    //     int next_magnitude1 = 
    //         FREQ_SCALE * m_internal_player->volume() / 10 
    //         * qLn(std::abs(dft_data1[i + BUCKET_SIZE]) / MAX_MAGNITUDE + 1);


    //     // For new data
    //     int current_magnitude2 =
    //         FREQ_SCALE * m_internal_player->volume() / 10 
    //         * qLn(std::abs(dft_data2[i]) / MAX_MAGNITUDE + 1);

    //     int next_magnitude2 = 
    //         FREQ_SCALE * m_internal_player->volume() / 10 
    //         * qLn(std::abs(dft_data2[i + BUCKET_SIZE]) / MAX_MAGNITUDE + 1);


    //     // Calculate the interpolated frequencies
    //     double current_magnitude = linear_interpolation(
    //             current_magnitude1, current_magnitude2,
    //             m_frames_stacked, m_max_frames_stacked
    //     );

    //     double next_magnitude = linear_interpolation(
    //             next_magnitude1, next_magnitude2,
    //             m_frames_stacked, m_max_frames_stacked
    //     );

    //     double y1 = dy + h - current_magnitude;
    //     double y2 = dy + h - next_magnitude;

    //     // Draw a line between the two frequency values
    //     painter.drawLine(x1, y1, x2, y2);
    // }

}

void audio_visualizer::draw_spectrum_rects() {
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 3));

    int w = width() * 4 / 5;
    int h = height() * 4 / 5;
    int dx = (width() - w) / 2;
    int dy = (height() - h) / 2;

    // Calculate the number of frequency values and the spacing between them
    int spectrum_count = m_spectrum.size();
    double spacing = 1. * w / spectrum_count;

    for (int i = 0; i < m_spectrum.size(); ++i) {
        // Calculate the interpolated spectrum
        double spectrum_value = linear_interpolation(
                m_old_spectrum[i], m_spectrum[i],
                m_frames_stacked, m_max_frames_stacked
        );

        // Calculate the x and y positions of the current frequency
        double x = dx + i * spacing;
        double y = dy + h - spectrum_value;

        // Draw the rect
        painter.drawRect(
            qRound(x), qRound(y),
            qRound(spacing), qRound(spectrum_value)
        );
    }

}

void audio_visualizer::process_dft() {
    auto p = m_collected_samples.constBegin();
    for (int i = 0; i < DFT_SIZE; ++i) {
        Q_ASSERT(p != m_collected_samples.constEnd());
        double windowing = 0.54 * - 0.46 * qCos(2.0 * M_PI * i / (DFT_SIZE - 1));
        m_dft_input[2 * i] = *p * windowing;
        m_dft_input[2 * i + 1] = 0;
        p++;
    }
    Q_ASSERT(p == m_collected_samples.constEnd());
    fftw_execute(m_dft_plan);
}

void audio_visualizer::compute_magnitudes() {
    for (int i = 0; i < DFT_SIZE / 2; ++i) {
        m_magnitudes[i] = std::abs(reinterpret_cast<std::complex<double> *>(m_dft_output)[i]);
    }
}

void audio_visualizer::backup_spectrum() {
    Q_ASSERT(m_spectrum.size() == m_old_spectrum.size());
    for (int i = 0; i < m_spectrum.size(); ++i) {
        m_old_spectrum[i] = m_spectrum[i];
    }
}

void audio_visualizer::compute_spectrum() {
    Q_ASSERT(m_spectrum.size() == DFT_SIZE / (2 * BUCKET_SIZE));
    compute_magnitudes();
    for (int i = 0; i < DFT_SIZE / 2 - BUCKET_SIZE + 1; i += BUCKET_SIZE) {
        double magnitude = *std::max_element(
            m_magnitudes.begin() + i, m_magnitudes.begin() + i + BUCKET_SIZE
        );
        
        m_spectrum[i / BUCKET_SIZE] = qMin(magnitude / 1000, static_cast<double>(height() / 2));
    }
}

void audio_visualizer::process_buffer(const QAudioBuffer &buffer) {
    const SAMPLE_TYPE *raw_data = reinterpret_cast<const SAMPLE_TYPE *>(buffer.constData());

    Q_ASSERT(SAMPLE_RATE == buffer.format().sampleRate());
    Q_ASSERT(SAMPLE_SIZE == buffer.format().sampleSize());

    for (int i = 0; i < buffer.sampleCount(); i += buffer.format().channelCount()) {
        Q_ASSERT(m_collected_samples.size() <= DFT_SIZE);

        if (m_collected_samples.size() == DFT_SIZE) {
            m_collected_samples.dequeue();
            m_new_samples_arrived = true;
        }

        m_collected_samples.enqueue(raw_data[i]);
    }
}

}  // namespace audio_app