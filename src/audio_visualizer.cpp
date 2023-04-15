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


#define DFT_SIZE 1024
#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 16
#define SAMPLE_TYPE qint16
#define MAX_MAGNITUDE 10'000

namespace audio_app {

const double FPS = 50.;
const double MSPF = 1000. / FPS;
const double FREQ_SCALE = 10.0;

/*--PUBLIC--*/

audio_visualizer::audio_visualizer(QWidget *parent) 
    : QWidget(parent)
    , m_probe(this)
    , m_dft_input(fftw_alloc_real(DFT_SIZE * 2))
    , m_dft_output(fftw_alloc_complex(DFT_SIZE))
    , m_dft_input_index(0)
{
    setWindowTitle("Visualizer");

    m_dft_plan = fftw_plan_dft_r2c_1d(DFT_SIZE, m_dft_input, m_dft_output, FFTW_MEASURE);

    connect(&m_probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(process_buffer(QAudioBuffer)));

}

/*--PROTECTED--*/

void audio_visualizer::paintEvent(QPaintEvent *) {
    if (m_dft_input_index >= DFT_SIZE) {
        fftw_execute(m_dft_plan);
    }

    std::complex<double> *dft_data = reinterpret_cast<std::complex<double> *>(m_dft_output);

    QPainter painter(this);
    painter.setPen(Qt::black);

    int w = width() * 4 / 5;
    int h = height() * 4 / 5;
    int dx = (width() - w) / 2;
    int dy = (height() - h) / 2;

    // Calculate the number of frequency values and the spacing between them
    qint64 num_freq = DFT_SIZE / 2;
    double freq_spacing = static_cast<double>(w) / num_freq;
    // double max_value = *std::max_element(m_magnitudes.constBegin(), m_magnitudes.constEnd());

    double current_magnitude;
    double next_magnitude = std::abs(dft_data[1]);
    
    // Iterate through each frequency value and draw a line between adjacent points
    for (qint64 i = 1; i < num_freq - 1; i++)
    {
        // Calculate magnitudes for 2 adjacent frequencies
        current_magnitude = next_magnitude;
        next_magnitude = std::abs(dft_data[i + 1]);

        // Calculate the x and y positions of the current and next frequency values
        int x1 = dx + i * freq_spacing;
        int x2 = dx + (i + 1) * freq_spacing;

        // int y1 = dy + h - (int)(FREQ_SCALE * qLn(current_magnitude + 1));
        // int y2 = dy + h - (int)(FREQ_SCALE * qLn(next_magnitude + 1));
        int y1 = dy + h - (int)(FREQ_SCALE * (1. * m_internal_player->volume() / 10) * qLn(current_magnitude / MAX_MAGNITUDE + 1));
        int y2 = dy + h - (int)(FREQ_SCALE * (1. * m_internal_player->volume() / 10) * qLn(next_magnitude / MAX_MAGNITUDE + 1));

        // Draw a line between the two frequency values
        painter.drawLine(x1, y1, x2, y2);
    }

    emit update();
}

// void initializeGL() {

// }

// void resizeGL(int, int) {

// }

// void paintGL() {
    
// }

/*--PRIVATE--*/

void audio_visualizer::process_buffer(const QAudioBuffer &buffer) {
    const SAMPLE_TYPE *raw_data = reinterpret_cast<const SAMPLE_TYPE *>(buffer.constData());

    Q_ASSERT(SAMPLE_RATE == buffer.format().sampleRate());
    Q_ASSERT(SAMPLE_SIZE == buffer.format().sampleSize());

    for (int i = 0; i < buffer.sampleCount(); i += buffer.format().channelCount()) {
        int input_index = (m_dft_input_index) % DFT_SIZE;
        double windowing = 0.5 * (1.0 - qCos(2.0 * M_PI * input_index / (DFT_SIZE - 1)));
        m_dft_input[2 * input_index] = raw_data[i] * windowing;
        m_dft_input[2 * input_index + 1] = 0;
        ++m_dft_input_index;
    }
}

}  // namespace audio_app