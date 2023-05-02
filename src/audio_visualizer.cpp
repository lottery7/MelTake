#include "audio_visualizer.hpp"
#include <fftw3.h>
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include <QWidget>
#include <QtMath>
#include <algorithm>
#include <complex>
#include <string>
#include "spectrum_analyzer.hpp"

namespace audio_app {

extern const int SPECTRUM_SIZE;
const int FPS_LIMIT = 60;
const int MEAN_SIZE = 5;

audio_visualizer::audio_visualizer(QWidget *parent, QMediaPlayer *player)
    : QWidget(parent),
      m_spectrum_analyzer(this),
      m_audio_volume(),
      m_spectrum_buffer(SPECTRUM_SIZE),
      m_spectrum(SPECTRUM_SIZE),
      m_nsecs_between_frames(),
      m_fps(),
      m_time_sum(),
      m_fps_counter(),
      m_update_timer(this),
      m_fps_timer() {
    setWindowTitle("Visualizer");
    set_player(player);

    m_update_timer.setTimerType(Qt::TimerType::PreciseTimer);
    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_update_timer.start(1000 / FPS_LIMIT);
    m_fps_timer.start();
}

void audio_visualizer::set_player(QMediaPlayer *player) {
    if (!player) {
        qDebug("audio_visualizer: Player is nullptr");
        return;
    }

    connect(
        player, SIGNAL(stateChanged(QMediaPlayer::State)), this,
        SLOT(set_audio_state(QMediaPlayer::State))
    );

    connect(
        player, SIGNAL(volumeChanged(int)), this, SLOT(set_audio_volume(int))
    );

    m_spectrum_analyzer.get_decoder().set_player(player);
}

void audio_visualizer::paintEvent(QPaintEvent *) {
    bool has_updated = false;

    if (m_audio_state == QMediaPlayer::PlayingState &&
        m_fps_timer.nsecsElapsed() >= m_nsecs_between_frames) {
        m_time_sum += m_fps_timer.nsecsElapsed();
        m_fps_timer.restart();
        ++m_fps_counter;
        if (m_time_sum >= 1'000'000'000) {
            m_fps = 1'000'000'000 * m_fps_counter / m_time_sum;
            m_time_sum = 0;
            m_fps_counter = 0;
        }

        update_spectrum();
        has_updated = true;
    }

    draw_rects();
    draw_info();

    if (has_updated) {
        m_nsecs_between_frames =
            qMax(1'000'000'000 / FPS_LIMIT - m_fps_timer.nsecsElapsed(), 0ll);
    }
}

void audio_visualizer::clear_buffer() {
    for (auto &values : m_spectrum_buffer) {
        values.clear();
    }
}

void audio_visualizer::set_audio_state(QMediaPlayer::State new_state) {
    m_audio_state = new_state;
}

void audio_visualizer::set_audio_volume(int new_volume) {
    m_audio_volume = new_volume;
}

void audio_visualizer::draw_rects() {
    QPainter painter(this);

    double w = width() * 4.0 / 5;
    double h = height();
    double dx = (width() - w) / 2;
    double dy = 0;

    double rect_fill_factor = 0.8;

    double rect_area = w / m_spectrum.size();
    double rect_width = rect_area * rect_fill_factor;
    double rect_spacing = rect_area * (1 - rect_fill_factor) / 2;

    for (int i = 0; i < m_spectrum.size(); ++i) {
        double spectrum_value = m_spectrum[i] * m_audio_volume;
        int c = 255 - std::clamp(spectrum_value * 3e-7, 0.0, 180.0);
        spectrum_value *= height() * 7e-10;

        double x = dx + i * rect_area + rect_spacing;
        spectrum_value = qPow(spectrum_value, 0.9);
        double y = dy + h / 2 - spectrum_value / 2;

        QColor rect_color(c, c, c);
        painter.fillRect(x, y, rect_width, spectrum_value, rect_color);
    }
}

void audio_visualizer::draw_info() {
    QPainter painter(this);
    painter.setPen(Qt::black);
    std::string text(std::string("FPS: ") + std::to_string(m_fps));
    painter.drawText(0, fontMetrics().height(), QString(text.c_str()));
}

void audio_visualizer::update_spectrum() {
    Q_ASSERT(m_spectrum.size() == SPECTRUM_SIZE);
    Q_ASSERT(m_spectrum.size() == m_spectrum_buffer.size());

    QVector<double> spectrum = m_spectrum_analyzer.get_spectrum();

    Q_ASSERT(spectrum.size() == m_spectrum.size());

    for (int i = 0; i < m_spectrum_buffer.size(); ++i) {
        m_spectrum_buffer[i].append(spectrum[i]);
        while (m_spectrum_buffer[i].size() > MEAN_SIZE) {
            m_spectrum_buffer[i].removeFirst();
        }
    }

    for (int i = 0; i < m_spectrum_buffer.size(); ++i) {
        qint64 sum = 0;
        for (double spectrum_value : m_spectrum_buffer[i]) {
            sum += spectrum_value;
        }
        m_spectrum[i] = sum / m_spectrum_buffer[i].size();
    }
}

}  // namespace audio_app
