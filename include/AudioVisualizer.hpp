#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H

#include <QWidget>
#include <QAudioProbe>
#include <QMediaPlayer>
#include <QTimer>
#include <QQueue>
#include <QList>
#include <QVector>

#include <fftw3.h>

namespace audio_app {

class AudioVisualizer : public QWidget {
    Q_OBJECT

public:
    explicit AudioVisualizer(QWidget *parent = nullptr);

    ~AudioVisualizer() {
        freeFFT();
    }

    void setMediaPlayer(QMediaPlayer *player) {
        m_internalPlayer = player;
        m_probe.setSource(m_internalPlayer);
        // connect(m_internalPlayer, SIGNAL(mediaChanged()), this, SLOT(restoreResources()));
    }

    void setAudioFormat(QAudioFormat format);
    
protected:
    void paintEvent(QPaintEvent *) override;

private:
    QVector<double> computeMagnitudes(const QVector<double> &audioData);
    QVector<double> normalizeData(const void *rawData, int start, int end);
    int getSamplesPerChunk();

    // bool shouldSkipChunk() {
    //     return m_audioPosition > m_chunkCurrentPosition + m_chunkDurationMs;
    // }
    
    void freeFFT() {
        fftw_destroy_plan(m_fftPlan);
        fftw_free(m_fftInput);
        fftw_free(m_fftOutput);
    }

    void initFFT(int n) {
        m_fftInput = fftw_alloc_real(2 * n);
        m_fftOutput = fftw_alloc_complex(n);
        m_fftPlan = fftw_plan_dft_r2c_1d(n, m_fftInput, m_fftOutput, FFTW_MEASURE | FFTW_DESTROY_INPUT);
    }
    
    QMediaPlayer *m_internalPlayer;
    QAudioProbe m_probe;
    QAudioFormat m_audioFormat;
    qint64 m_audioPosition;

    QVector<double> m_magnitudes;
    QQueue<QVector<double>> m_chunkQueue;
    qint64 m_chunkDurationMs;
    qint64 m_chunkPosition;
    qint64 m_chunkProcessedPosition;
    qint32 m_skipChunks;

    double *m_fftInput;
    fftw_complex *m_fftOutput;
    fftw_plan m_fftPlan;

    QTimer m_updateTimer;

private slots:
    void processBuffer(const QAudioBuffer &);

    void updateAudioPosition() {
        m_audioPosition = m_internalPlayer->position();
    }

    // void restoreResources();
};

}  // namespace auvi

#endif  // AUDIO_VISUALIZER_H