#include "include/audio_visualizer.hpp"

#include <QPainter>
#include <QWidget>
#include <qmath.h>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QVector2D>
#include <QCoreApplication>

#include <algorithm>
#include <fftw3.h>

namespace audio_app {

const qint32 FPS = 50;
const qint32 MSPF = 1000 / FPS;
const double FREQ_SCALE = 100.0;

/*--PUBLIC--*/

audio_visualizer::audio_visualizer(QWidget *parent) 
    : QWidget(parent)
    , m_probe(this)
    , m_chunk_duration_ms(MSPF)
    , m_chunk_pos(0)
    , m_chunk_processed_pos(0)
    , m_update_timer(this)
{
    setWindowTitle("Visualizer");

    connect(&m_probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(process_buffer(QAudioBuffer)));

    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(update_audio_pos()));

    m_update_timer.start(MSPF);
}

void audio_visualizer::set_audio_format(QAudioFormat format) {
    Q_ASSERT(format.isValid());

    m_audio_format = format;
    int samples_count = get_samples_per_chunk();
    
    freeFFT();
    initFFT(samples_count);
}

/*--PROTECTED--*/

void audio_visualizer::paintEvent(QPaintEvent *) {
    while (m_chunk_pos + m_chunk_duration_ms < m_audio_pos) {
        if (!m_chunk_queue.empty()) {
            m_magnitudes = m_chunk_queue.dequeue();
            m_chunk_pos += m_chunk_duration_ms;
        } else {
            m_skip_chunks = 1 + (m_audio_pos - m_chunk_pos) / m_chunk_duration_ms;
            m_chunk_pos += m_skip_chunks * m_chunk_duration_ms;
        }
    }

    if (m_magnitudes.empty()) {
        if (m_chunk_queue.empty() || m_chunk_pos >= m_audio_pos) {
            return;
        }
        m_magnitudes = m_chunk_queue.dequeue();
        m_chunk_pos += m_chunk_duration_ms;
    }

    // qDebug() << m_chunkCurrentPosition << m_audio_pos;

    QPainter painter(this);
    painter.setPen(Qt::black);

    int w = width() * 4 / 5;
    int h = height() * 4 / 5;
    int dx = (width() - w) / 2;
    int dy = (height() - h) / 2;

    // Calculate the number of frequency values and the spacing between them
    int num_freq = m_magnitudes.size();
    double freq_spacing = static_cast<double>(w) / num_freq;
    // double max_value = *std::max_element(m_magnitudes.constBegin(), m_magnitudes.constEnd());

    // Iterate through each frequency value and draw a line between adjacent points
    for (int i = 0; i < num_freq - 1; i++)
    {
        // Calculate the x and y positions of the current and next frequency values
        int x1 = dx + i * freq_spacing;
        int x2 = dx + (i + 1) * freq_spacing;
        // int y1 = h + dy - (int)(qPow(m_magnitudes[i] / max_value, 0.5) * h);
        // int y2 = h + dy - (int)(qPow(m_magnitudes[i + 1] / max_value, 0.5) * h);
        int y1 = h + dy - (int)(FREQ_SCALE * qLn(m_magnitudes[i] + 1));
        int y2 = h + dy - (int)(FREQ_SCALE * qLn(m_magnitudes[i+1] + 1));

        // Draw a line between the two frequency values
        painter.drawLine(x1, y1, x2, y2);
    }
}

/*--PRIVATE--*/

QVector<double> audio_visualizer::compute_magnitudes(const QVector<double> &data) {
    // Data contains samples
    int samples_count = data.size();

    Q_ASSERT(samples_count == get_samples_per_chunk());

    // Copy and prepare data
    for (int i = 0; i < samples_count; ++i) {
        // Apply Hann window function to avoid spectrum leakage
        double windowing = 0.5 - 0.5 * cos(2 * M_PI * i / (samples_count - 1));
        m_fft_input[2 * i] = data[i] * windowing;
        m_fft_input[2 * i + 1] = 0;  // We have real values, so im. part equals zero
    }

    // Perform FFT
    fftw_execute(m_fft_plan);

    // One half of the output is empty, another half is symmetric
    QVector<double> magnitudes(samples_count / 2);
    for (int i = 0; i < samples_count / 2; ++i) {
        double re = m_fft_output[i][0];
        double im = m_fft_output[i][1];
        magnitudes[i] = qSqrt(re * re + im * im);
    }

    return magnitudes;
}

QVector<double> audio_visualizer::normalize_data(const void *raw_data, int start, int end) {
    int samples_count = end - start;

    Q_ASSERT(samples_count == get_samples_per_chunk());

    int sample_size = m_audio_format.sampleSize();
    double max_value = (1 << (sample_size - 1)) - 1;
    QVector<double> audio_data(samples_count);

    int channel_count = m_audio_format.channelCount();

    for (int i = start; i < end; ++i) {
        if (sample_size == 8) {
            audio_data[i - start] = ((qint8 *)raw_data)[i * channel_count] / max_value;
        } else if (sample_size == 16) {
            audio_data[i - start] = ((qint16 *)raw_data)[i * channel_count] / max_value;
        } else {
            qDebug() << "Audio format is not supported: sample size is" << sample_size << "which is too high";
            return {};
        }
    }

    return audio_data;
}

// Returns samples count per chunk IN ONE CHANNEL.
int audio_visualizer::get_samples_per_chunk() {
    Q_ASSERT(m_audio_format.isValid());

    return m_audio_format.bytesForDuration(m_chunk_duration_ms * 1'000) * 8
            / m_audio_format.sampleSize()
            / m_audio_format.channelCount();
}

void audio_visualizer::process_buffer(const QAudioBuffer &buffer) {
    if (!m_audio_format.isValid()) {
        set_audio_format(buffer.format());
    }

    Q_ASSERT(m_audio_format.isValid());

    const void *raw_audio_data = buffer.constData();
    int samples_per_chunk = get_samples_per_chunk();
    int total_chunks = buffer.sampleCount()
                    / m_audio_format.channelCount()
                    / samples_per_chunk;

    for (int i = 0; i < total_chunks; ++i) {
        if (m_skip_chunks > 0) {
            m_skip_chunks--;
            continue;
        }
        QVector<double> data = normalize_data(
            raw_audio_data,
            i * samples_per_chunk
            , (i + 1) * samples_per_chunk
        );
        m_chunk_queue.enqueue(compute_magnitudes(data));
    }
}

}  // namespace auvi