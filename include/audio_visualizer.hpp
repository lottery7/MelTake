#ifndef AUDIO_VISUALIZER_HPP
#define AUDIO_VISUALIZER_HPP

#include <fftw3.h>
#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QList>
#include <QMediaPlayer>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include "spectrum_analyzer.hpp"

namespace audio_app {

class audio_visualizer : public QOpenGLWidget, private QOpenGLFunctions {
    Q_OBJECT

public:
    explicit audio_visualizer(QWidget *parent = nullptr);
    ~audio_visualizer();

    void set_player(QMediaPlayer *player);
    void set_fragment_shader_path(const QString &path);

public slots:
    void clear_buffer();

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private slots:
    bool compile_fragment_shader();

private:
    bool should_update();
    void update_spectrum();
    void update_fps();
    void draw();
    void print_debug(const QString &error) const;

    const QMediaPlayer *m_player;

    spectrum_analyzer m_spectrum_analyzer;
    qint32 m_spectrum_buffer_size;
    QVector<QList<double>> m_spectrum_buffer;
    QVector<float> m_spectrum;

    /* Fields for FPS control */
    qint64 m_fps;
    qint64 m_fps_limit;
    qint64 m_fps_counter;
    QElapsedTimer m_fps_timer;
    qint64 m_nsecs_between_frames;
    qint64 m_time_sum;

    QString m_fragment_shader_path;
    QOpenGLBuffer m_vertex_buffer;
    QOpenGLShader *m_fragment_shader;
    QOpenGLShaderProgram m_shader_program;

    QFileSystemWatcher m_watcher;
    QElapsedTimer m_time;
};

}  // namespace audio_app

#endif  // AUDIO_VISUALIZER_HPP
