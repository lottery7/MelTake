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

class audio_visualizer : public QWidget {
    Q_OBJECT

public:
    explicit audio_visualizer(QWidget *parent = nullptr);

    ~audio_visualizer() {
        freeFFT();
    }

    void set_media_player(QMediaPlayer *player) {
        m_internal_player = player;
        m_probe.setSource(m_internal_player);
        // connect(m_internal_player, SIGNAL(mediaChanged()), this, SLOT(restore_resources()));
    }

    void set_audio_format(QAudioFormat format);
    
protected:
    void paintEvent(QPaintEvent *) override;

private:
    QVector<double> compute_magnitudes(const QVector<double> &audio_data);
    QVector<double> normalize_data(const void *raw_data, int start, int end);
    int get_samples_per_chunk();

    // bool should_skip_chunk() {
    //     return m_audio_pos > m_chunk_cur_pos + m_chunk_duration_ms;
    // }
    
    void freeFFT() {
        fftw_destroy_plan(m_fft_plan);
        fftw_free(m_fft_input);
        fftw_free(m_fft_output);
    }

    void initFFT(int n) {
        m_fft_input = fftw_alloc_real(2 * n);
        m_fft_output = fftw_alloc_complex(n);
        m_fft_plan = fftw_plan_dft_r2c_1d(n, m_fft_input, m_fft_output, FFTW_MEASURE | FFTW_DESTROY_INPUT);
    }
    
    QMediaPlayer *m_internal_player;
    QAudioProbe m_probe;
    QAudioFormat m_audio_format;
    qint64 m_audio_pos;

    QVector<double> m_magnitudes;
    QQueue<QVector<double>> m_chunk_queue;
    qint64 m_chunk_duration_ms;
    qint64 m_chunk_pos;
    qint64 m_chunk_processed_pos;
    qint32 m_skip_chunks;

    double *m_fft_input;
    fftw_complex *m_fft_output;
    fftw_plan m_fft_plan;

    QTimer m_update_timer;

private slots:
    void process_buffer(const QAudioBuffer &buffer);

    void update_audio_pos() {
        m_audio_pos = m_internal_player->position();
    }

    // void restore_resources();
};

}  // namespace auvi

#endif  // AUDIO_VISUALIZER_H