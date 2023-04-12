#include "include/AudioVisualizer.hpp"

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

AudioVisualizer::AudioVisualizer(QWidget *parent) 
    : QWidget(parent)
    , m_probe(this)
    , m_chunkDurationMs(MSPF)
    , m_chunkPosition(0)
    , m_chunkProcessedPosition(0)
    , m_updateTimer(this)
{
    setWindowTitle("Visualizer");

    connect(&m_probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateAudioPosition()));

    m_updateTimer.start(MSPF);
}

void AudioVisualizer::setAudioFormat(QAudioFormat format) {
    Q_ASSERT(format.isValid());

    m_audioFormat = format;
    int samplesCount = getSamplesPerChunk();
    
    freeFFT();
    initFFT(samplesCount);
}

/*--PROTECTED--*/

void AudioVisualizer::paintEvent(QPaintEvent *) {
    while (m_chunkPosition + m_chunkDurationMs < m_audioPosition) {
        if (!m_chunkQueue.empty()) {
            m_magnitudes = m_chunkQueue.dequeue();
            m_chunkPosition += m_chunkDurationMs;
        } else {
            m_skipChunks = 1 + (m_audioPosition - m_chunkPosition) / m_chunkDurationMs;
            m_chunkPosition += m_skipChunks * m_chunkDurationMs;
        }
    }

    if (m_magnitudes.empty()) {
        if (m_chunkQueue.empty() || m_chunkPosition >= m_audioPosition) {
            return;
        }
        m_magnitudes = m_chunkQueue.dequeue();
        m_chunkPosition += m_chunkDurationMs;
    }

    // qDebug() << m_chunkCurrentPosition << m_audioPosition;

    QPainter painter(this);
    painter.setPen(Qt::black);

    int w = width() * 4 / 5;
    int h = height() * 4 / 5;
    int dx = (width() - w) / 2;
    int dy = (height() - h) / 2;

    // Calculate the number of frequency values and the spacing between them
    int numFreq = m_magnitudes.size();
    double freqSpacing = static_cast<double>(w) / numFreq;
    // double maxValue = *std::max_element(m_magnitudes.constBegin(), m_magnitudes.constEnd());

    // Iterate through each frequency value and draw a line between adjacent points
    for (int i = 0; i < numFreq - 1; i++)
    {
        // Calculate the x and y positions of the current and next frequency values
        int x1 = dx + i * freqSpacing;
        int x2 = dx + (i + 1) * freqSpacing;
        // int y1 = h + dy - (int)(qPow(m_magnitudes[i] / maxValue, 0.5) * h);
        // int y2 = h + dy - (int)(qPow(m_magnitudes[i + 1] / maxValue, 0.5) * h);
        int y1 = h + dy - (int)(FREQ_SCALE * qLn(m_magnitudes[i] + 1));
        int y2 = h + dy - (int)(FREQ_SCALE * qLn(m_magnitudes[i+1] + 1));

        // Draw a line between the two frequency values
        painter.drawLine(x1, y1, x2, y2);
    }
}

/*--PRIVATE--*/

QVector<double> AudioVisualizer::computeMagnitudes(const QVector<double> &data) {
    // Data contains samples
    int samplesCount = data.size();

    Q_ASSERT(samplesCount == getSamplesPerChunk());

    // Copy and prepare data
    for (int i = 0; i < samplesCount; ++i) {
        // Apply Hann window function to avoid spectrum leakage
        double windowing = 0.5 - 0.5 * cos(2 * M_PI * i / (samplesCount - 1));
        m_fftInput[2 * i] = data[i] * windowing;
        m_fftInput[2 * i + 1] = 0;  // We have real values, so im. part equals zero
    }

    // Perform FFT
    fftw_execute(m_fftPlan);

    // One half of the output is empty, another half is symmetric
    QVector<double> magnitudes(samplesCount / 2);
    for (int i = 0; i < samplesCount / 2; ++i) {
        double re = m_fftOutput[i][0];
        double im = m_fftOutput[i][1];
        magnitudes[i] = qSqrt(re * re + im * im);
    }

    return magnitudes;
}

QVector<double> AudioVisualizer::normalizeData(const void *rawData, int start, int end) {
    int samplesCount = end - start;

    Q_ASSERT(samplesCount == getSamplesPerChunk());

    int sampleSize = m_audioFormat.sampleSize();
    double maxValue = (1 << (sampleSize - 1)) - 1;
    QVector<double> audioData(samplesCount);

    int channelCount = m_audioFormat.channelCount();

    for (int i = start; i < end; ++i) {
        if (sampleSize == 8) {
            audioData[i - start] = ((qint8 *)rawData)[i * channelCount] / maxValue;
        } else if (sampleSize == 16) {
            audioData[i - start] = ((qint16 *)rawData)[i * channelCount] / maxValue;
        } else {
            qDebug() << "Audio format is not supported: sample size is" << sampleSize << "which is too high";
            return {};
        }
    }

    return audioData;
}

// Returns samples count per chunk IN ONE CHANNEL.
int AudioVisualizer::getSamplesPerChunk() {
    Q_ASSERT(m_audioFormat.isValid());

    return m_audioFormat.bytesForDuration(m_chunkDurationMs * 1'000) * 8
            / m_audioFormat.sampleSize()
            / m_audioFormat.channelCount();
}

void AudioVisualizer::processBuffer(const QAudioBuffer &buffer) {
    if (!m_audioFormat.isValid()) {
        setAudioFormat(buffer.format());
    }

    Q_ASSERT(m_audioFormat.isValid());

    const void *rawAudioData = buffer.constData();
    int samplesPerChunk = getSamplesPerChunk();
    int totalChunks = buffer.sampleCount()
                    / m_audioFormat.channelCount()
                    / samplesPerChunk;

    for (int i = 0; i < totalChunks; ++i) {
        if (m_skipChunks > 0) {
            m_skipChunks--;
            continue;
        }
        QVector<double> data = normalizeData(
            rawAudioData,
            i * samplesPerChunk
            , (i + 1) * samplesPerChunk
        );
        m_chunkQueue.enqueue(computeMagnitudes(data));
    }
}

}  // namespace auvi