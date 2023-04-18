#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H


#include <QAudioProbe>
#include <QMediaPlayer>
#include <QTimer>
#include <QQueue>
#include <QVector>
#include <QWidget>

#include <complex>

#include <fftw3.h>


namespace audio_app {

class audio_visualizer : public QWidget {
    Q_OBJECT

public:
    explicit audio_visualizer(QWidget *parent = nullptr);

    void set_media_player(QMediaPlayer *player) {
        m_internal_player = player;
        m_probe.setSource(m_internal_player);
    }
    
protected:
    void paintEvent(QPaintEvent *) override;

private:
    qint64 get_audio_pos() {
        return m_internal_player->position();
    }

    static double linear_interpolation(double x1, double x2, double step, double max);
    void draw_spectrum_peaks();
    void draw_spectrum_rects();
    void process_dft();
    void compute_magnitudes();
    void backup_spectrum();
    void compute_spectrum();

    QMediaPlayer *m_internal_player;
    QAudioProbe m_probe;
    QAudioFormat m_audio_format;

    double *m_dft_input;
    fftw_complex *m_dft_output;
    fftw_plan m_dft_plan;

    QVector<double> m_magnitudes;
    QVector<double> m_old_spectrum;
    QVector<double> m_spectrum;
    QQueue<double> m_collected_samples;

    bool m_new_samples_arrived;
    qint32 m_frames_stacked;
    qint32 m_max_frames_stacked;
    QTimer m_update_timer;

private slots:
    void process_buffer(const QAudioBuffer &buffer);
};

}  // namespace audio_app

#endif  // AUDIO_VISUALIZER_H