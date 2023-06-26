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
#include <QSurface>
#include <QVector>
#include <QWidget>
#include <QtMath>
#include <algorithm>
#include <complex>
#include <string>
#include "spectrum_analyzer.hpp"

// WIP:
// * Add colors from the cover
// * Add settings panel

namespace audio_app {

audio_visualizer::audio_visualizer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_player(),
      m_spectrum_analyzer(this),
      m_spectrum_buffer_size(32),
      m_spectrum_buffer(m_spectrum_analyzer.get_spectrum_size()),
      m_spectrum(m_spectrum_analyzer.get_spectrum_size()),
      m_fps(),
      m_fps_limit(240),
      m_fps_counter(),
      m_nsecs_between_frames(),
      m_time_sum(),
      m_fragment_shader_path(":/shaders/main.frag"),
      m_fragment_shader() {
    setWindowTitle("Visualizer");
    m_fps_timer.start();
    m_time.start();
}

audio_visualizer::~audio_visualizer() {
    m_vertex_buffer.destroy();
    delete m_fragment_shader;
}

void audio_visualizer::set_player(QMediaPlayer *player) {
    m_player = player;
    m_spectrum_analyzer.get_decoder().set_player(player);
}

void audio_visualizer::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!m_vertex_buffer.create()) {
        print_debug("Failed to create vertex buffer");
        return;
    }

    if (!m_vertex_buffer.bind()) {
        print_debug("Failed to bind vertex buffer");
        return;
    }

    const QVector<GLfloat> vertices = {
        -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1,
    };
    m_vertex_buffer.allocate(
        vertices.begin(), vertices.size() * sizeof(GLfloat)
    );

    if (!m_shader_program.addShaderFromSourceCode(
            QOpenGLShader::Vertex,
            "#version 330\n"
            "layout(location = 0) in vec2 vertex_position;\n"
            "void main() {\n"
            "   gl_Position = vec4(vertex_position, 0, 1);\n"
            "}\n"
        )) {
        print_debug("Failed to load vertex shader");
        return;
    }

    m_fragment_shader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    if (!m_shader_program.addShader(m_fragment_shader)) {
        print_debug("Failed to load fragment shader");
        return;
    }

    if (!compile_fragment_shader()) {
        print_debug("Failed to compile fragment shader");
        return;
    }
}

void audio_visualizer::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
}

void audio_visualizer::paintGL() {
    bool has_updated = false;
    if (should_update()) {
        update_fps();
        update_spectrum();
        has_updated = true;
    }

    draw();

    if (has_updated) {
        m_nsecs_between_frames =
            qMax(1'000'000'000 / m_fps_limit - m_fps_timer.nsecsElapsed(), 0ll);
    }

    emit update();
}

bool audio_visualizer::compile_fragment_shader() {
    if (m_fragment_shader_path.isEmpty()) {
        print_debug("The fragment shader path is empty");
        return false;
    }

    if (!QFile::exists(m_fragment_shader_path)) {
        print_debug(
            "Failed to load fragment shader from \"" + m_fragment_shader_path +
            "\""
        );
        return false;
    }

    // Create new temporary shader to prevent OpenGL crashing (doesn't work)
    QOpenGLShader *new_shader =
        new QOpenGLShader(QOpenGLShader::Fragment, this);

    if (!new_shader->compileSourceFile(m_fragment_shader_path)) {
        delete new_shader;
        return false;
    }

    // Delete old shader and add the new one
    m_shader_program.removeShader(m_fragment_shader);
    delete m_fragment_shader;
    m_fragment_shader = new_shader;
    m_shader_program.addShader(m_fragment_shader);

    if (!m_shader_program.link()) {
        print_debug("Failed to link shader program");
        return false;
    }

    return true;
}

void audio_visualizer::clear_buffer() {
    for (auto &values : m_spectrum_buffer) {
        values.clear();
    }
}

void audio_visualizer::update_spectrum() {
    m_spectrum_analyzer.update_spectrum();
    QVector<double> spectrum = m_spectrum_analyzer.get_spectrum();

    // Remove redundant values in spectrum buffer
    for (int i = 0; i < m_spectrum_buffer.size(); ++i) {
        m_spectrum_buffer[i].append(spectrum[i]);
        while (m_spectrum_buffer[i].size() > m_spectrum_buffer_size) {
            m_spectrum_buffer[i].removeFirst();
        }
    }

    // Calculate and smooth spectrum
    for (int i = 0; i < m_spectrum_buffer.size(); ++i) {
        m_spectrum[i] = 0;
        for (int j = 0; j < m_spectrum_buffer[i].size(); j++) {
            m_spectrum[i] += m_spectrum_buffer[i][j] *
                             pow((j + 1.) / m_spectrum_buffer[i].size(), 4.);
        }
    }
}

bool audio_visualizer::should_update() {
    return m_player->state() == QMediaPlayer::PlayingState &&
           m_fps_timer.nsecsElapsed() >= m_nsecs_between_frames;
}

void audio_visualizer::update_fps() {
    m_time_sum += m_fps_timer.nsecsElapsed();
    m_fps_timer.restart();
    m_fps_counter++;

    if (m_time_sum >= 1e9) {
        m_fps = 1e9 * m_fps_counter / m_time_sum;
        m_time_sum = 0;
        m_fps_counter = 0;
    }
}

void audio_visualizer::draw() {
    makeCurrent();

    if (!m_shader_program.isLinked()) {
        print_debug("Shader program is not linked");
        return;
    }

    if (!m_shader_program.bind()) {
        print_debug("Failed to bind shader program");
        return;
    }

    if (!m_vertex_buffer.bind()) {
        print_debug("Failed to bind vertex buffer");
        return;
    }

    m_shader_program.enableAttributeArray(0);
    m_shader_program.setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));

    // Uniforms
    m_shader_program.setUniformValue("u_time", m_time.elapsed() * 1e-3f);
    m_shader_program.setUniformValue(
        "u_resolution", QVector2D(width(), height())
    );
    m_shader_program.setUniformValueArray(
        "u_spectrum", m_spectrum.constBegin(), m_spectrum.size(), 1
    );
    m_shader_program.setUniformValue("u_spectrum_size", m_spectrum.size());
    m_shader_program.setUniformValue(
        "u_max_magnitude", (GLfloat)m_spectrum_analyzer.get_max_magnitude()
    );
    m_shader_program.setUniformValue(
        "u_audio_position", (GLfloat)m_player->position() / m_player->duration()
    );

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void audio_visualizer::print_debug(const QString &text) const {
    qDebug() << "audio_visualizer: " << text;
}

}  // namespace audio_app
