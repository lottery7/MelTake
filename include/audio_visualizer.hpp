#ifndef AUDIO_VISUALIZER_HPP
#define AUDIO_VISUALIZER_HPP

#include <fftw3.h>
#include <QAudioProbe>
#include <QElapsedTimer>
#include <QList>
#include <QMediaPlayer>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include "spectrum_analyzer.hpp"

namespace audio_app {

class audio_visualizer : public QWidget {
    Q_OBJECT

public:
    explicit audio_visualizer(
        QWidget *parent = nullptr,
        QMediaPlayer *player = nullptr
    );
    void set_player(QMediaPlayer *player);

public slots:
    void clear_buffer();
    void set_audio_state(QMediaPlayer::State new_state);
    void set_audio_volume(int new_volume);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void draw_rects();
    void draw_info();
    void update_spectrum();

    spectrum_analyzer m_spectrum_analyzer;
    QMediaPlayer::State m_audio_state;
    int m_audio_volume;

    QVector<QList<double>> m_spectrum_buffer;
    QVector<double> m_spectrum;

    qint64 m_nsecs_between_frames;
    qint64 m_fps;
    qint64 m_time_sum;
    qint64 m_fps_counter;

    QTimer m_update_timer;
    QElapsedTimer m_fps_timer;
};

}  // namespace audio_app

#endif  // AUDIO_VISUALIZER_HPP
