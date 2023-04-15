#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H


#include <QAudioProbe>
#include <QMediaPlayer>
#include <QTimer>
#include <QQueue>
#include <QVector>
#include <QWidget>

// #include <QOpenGLWidget>
// #include <QOpenGLFunctions>

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
    // void initializeGL() override;
    // void resizeGL(int, int) override;
    // void paintGL() override;

    void paintEvent(QPaintEvent *) override;

private:
    qint64 get_audio_pos() {
        return m_internal_player->position();
    }

    QMediaPlayer *m_internal_player;
    QAudioProbe m_probe;
    QAudioFormat m_audio_format;

    double *m_dft_input;
    fftw_complex *m_dft_output;
    fftw_plan m_dft_plan;
    qint64 m_dft_input_index;

    QQueue<double> m_samples_queue;

private slots:
    void process_buffer(const QAudioBuffer &buffer);
};

}  // namespace audio_app

#endif  // AUDIO_VISUALIZER_H