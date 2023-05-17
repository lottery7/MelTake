#include "audio_visualizer.hpp"
#include <QDateTime>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPainterPath>
#include <QStringList>
#include <QVector>
#include <QWidget>
#include <QtMath>
#include <algorithm>
#include <complex>
#include <string>
#include "spectrum_analyzer.hpp"

// WIP:
// 0) Add colors from the cover
// 1) Add real-time shader compiling on change | DONE!
// 1.5) Prevent crashing on real-time shader coding

namespace audio_app {

extern const int SPECTRUM_SIZE;
const int FPS_LIMIT = 60;
const int MEAN_SIZE = 2;
const QString FRAGMENT_SHADER_PATH("../shaders/main.frag");
const QString VERTEX_SHADER_CODE = (
"#version 330\n"
"layout(location = 0) in vec2 vertex_position;\n"
"void main() {\n"
"    gl_Position = vec4(vertex_position, 0, 1);"
"}\n"
);

audio_visualizer::audio_visualizer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_audio_volume(),
      m_audio_position(),
      m_audio_duration(),
      m_spectrum_analyzer(this),
      m_spectrum_buffer(SPECTRUM_SIZE),
      m_spectrum(SPECTRUM_SIZE),
      m_nsecs_between_frames(),
      m_fps(),
      m_time_sum(),
      m_fps_counter(),
      m_update_timer(this),
      m_fragment_shader(),
      m_watcher(QStringList(FRAGMENT_SHADER_PATH), this) {
    setWindowTitle("Visualizer");

    connect(
        &m_watcher, SIGNAL(fileChanged(const QString &)), this,
        SLOT(compile_fragment_shader())
    );

    m_update_timer.setTimerType(Qt::TimerType::PreciseTimer);
    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(update()));

    m_update_timer.start(1000 / FPS_LIMIT);
    m_fps_timer.start();
    m_time.start();
}

audio_visualizer::~audio_visualizer() {
    m_vertex_buffer.destroy();
    delete m_fragment_shader;
}

void audio_visualizer::set_player(QMediaPlayer *player) {
    if (!player) {
        print_error("Player is a nullptr");
        return;
    }

    connect(
        player, SIGNAL(stateChanged(QMediaPlayer::State)), this,
        SLOT(set_audio_state(QMediaPlayer::State))
    );
    connect(
        player, SIGNAL(volumeChanged(int)), this, SLOT(set_audio_volume(int))
    );
    connect(
        player, SIGNAL(positionChanged(qint64)), this,
        SLOT(set_audio_position(qint64))
    );
    connect(
        player, SIGNAL(durationChanged(qint64)), this,
        SLOT(set_audio_duration(qint64))
    );

    m_spectrum_analyzer.get_decoder().set_player(player);
}

void audio_visualizer::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!m_vertex_buffer.create()) {
        print_error("Failed to create vertex buffer");
        close();
    }
    if (!m_vertex_buffer.bind()) {
        print_error("Failed to bind vertex buffer");
        close();
    }

    const QVector<GLfloat> vertices = {
        -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1,
    };
    m_vertex_buffer.allocate(
        vertices.begin(), vertices.size() * sizeof(GLfloat)
    );

    if (!m_shader_program.addShaderFromSourceCode(
            QOpenGLShader::Vertex, VERTEX_SHADER_CODE
        )) {
        print_error("Failed to load vertex shader");
        close();
    }

    m_fragment_shader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    if (!m_shader_program.addShader(m_fragment_shader)) {
        print_error("Failed to add fragment shader");
        close();
    }

    compile_fragment_shader();
}

void audio_visualizer::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void audio_visualizer::paintGL() {
    bool has_updated = false;
    if (m_audio_state == QMediaPlayer::PlayingState &&
        m_fps_timer.nsecsElapsed() >= m_nsecs_between_frames) {
        m_time_sum += m_fps_timer.nsecsElapsed();
        m_fps_timer.restart();
        ++m_fps_counter;
        if (m_time_sum >= 1e9) {
            m_fps = 1e9 * m_fps_counter / m_time_sum;
            m_time_sum = 0;
            m_fps_counter = 0;
        }

        update_spectrum();
        has_updated = true;
    }

    if (!m_shader_program.bind()) {
        print_error("Failed to bind shader program");
        return;
    }

    if (!m_vertex_buffer.bind()) {
        print_error("Failed to bind vertex buffer");
        return;
    }

    m_shader_program.enableAttributeArray(0);
    m_shader_program.setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));

    m_shader_program.setUniformValue("u_time", m_time.elapsed() / 1'000.f);
    m_shader_program.setUniformValue(
        "u_resolution", QVector2D(width(), height())
    );
    m_shader_program.setUniformValueArray(
        "u_spectrum", m_spectrum.constBegin(), SPECTRUM_SIZE, 1
    );
    m_shader_program.setUniformValue("u_spectrum_size", SPECTRUM_SIZE);
    m_shader_program.setUniformValue(
        "u_max_magnitude", (GLfloat)m_spectrum_analyzer.get_max_magnitude()
    );
    m_shader_program.setUniformValue(
        "u_audio_position", (GLfloat)m_audio_position
    );

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (has_updated) {
        m_nsecs_between_frames =
            qMax(1'000'000'000 / FPS_LIMIT - m_fps_timer.nsecsElapsed(), 0ll);
    }
}

void audio_visualizer::compile_fragment_shader() {
    // Create new temporary shader to prevent OpenGL crashing
    QOpenGLShader *new_shader =
        new QOpenGLShader(QOpenGLShader::Fragment, this);
    if (!new_shader->compileSourceFile(FRAGMENT_SHADER_PATH)) {
        print_error(m_fragment_shader->log());
        delete new_shader;
        return;
    }

    m_shader_program.removeShader(m_fragment_shader);
    delete m_fragment_shader;
    m_fragment_shader = new_shader;
    m_shader_program.addShader(m_fragment_shader);

    if (!m_shader_program.link()) {
        print_error("Failed to link shader program");
        return;
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

void audio_visualizer::set_audio_position(qint64 new_position) {
    m_audio_position = (double)new_position / m_audio_duration;
}

void audio_visualizer::set_audio_duration(qint64 new_duration) {
    m_audio_duration = new_duration;
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

void audio_visualizer::print_error(const QString &error) const {
    qDebug() << "audio_visualizer: " << error;
}

}  // namespace audio_app
