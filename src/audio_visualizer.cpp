#include "audio_constants.hpp"
#include "audio_visualizer.hpp"
// #include "audio_decoder.hpp"
#include <qmath.h>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include <algorithm>
#include <complex>
#include <string>
#include <fftw3.h>


namespace audio_app {


static const float SPECTRUM_SIZE = 64;
static const int DFT_SIZE = 16384;
int DELAY = DFT_SIZE / 2;
const bool INTERPOLATION = false;
int MEAN_SIZE = 10;

audio_visualizer::audio_visualizer(QWidget *parent) 
    : QWidget(parent)

    , m_dft_input(fftwf_alloc_real(DFT_SIZE * 2))
    , m_dft_output(fftwf_alloc_complex(DFT_SIZE))

    , m_first_sample_index(0)
    , m_magnitudes(DFT_SIZE / 4)
    , m_spectrum_values(SPECTRUM_SIZE)
    , m_spectrum(SPECTRUM_SIZE)
    , m_old_spectrum(SPECTRUM_SIZE)

    , m_frequency_bins(SPECTRUM_SIZE + 1)
{
    setWindowTitle("Visualizer");
    
    m_fps_timer.start();
    m_delay_timer.start();

    m_dft_plan = fftwf_plan_dft_r2c_1d(DFT_SIZE, m_dft_input, m_dft_output, FFTW_MEASURE);

    m_frequency_bins[0] = 20;
    static float base = qPow(
        (SAMPLE_RATE / 2 - m_frequency_bins[0]) / m_frequency_bins[0],
        1. / SPECTRUM_SIZE
    );
    for (int i = 0; i < SPECTRUM_SIZE; ++i) {
        m_frequency_bins[i + 1] = m_frequency_bins[i] * base;
    }
}


void audio_visualizer::set_player(QMediaPlayer *player) {
    m_player = player;
    connect(
        m_player, SIGNAL(currentMediaChanged(const QMediaContent &)),
        this, SLOT(reset_buffers())
    );

    m_probe.setSource(m_player);
    connect(
        &m_probe, SIGNAL(audioBufferProbed(const QAudioBuffer &)),
        this, SLOT(process_buffer(const QAudioBuffer &))
    );
}


void audio_visualizer::add_sample(SAMPLE_TYPE sample) {
    m_samples_buffer.push_back(sample);
}


void audio_visualizer::paintEvent(QPaintEvent *) {
    if (m_player->state() == QMediaPlayer::PlayingState) {
        process_dft();
        compute_magnitudes();
        if (INTERPOLATION) {
            backup_spectrum();
        }
        compute_spectrum();
    }


    draw_rects();
    draw_info();

    update();
}


float audio_visualizer::linear_interpolation(float x1, float x2, float step, float max) {
    return x1 + step / max * (x2 - x1);
}


float audio_visualizer::to_decibels(float value) {
    Q_ASSERT(value > 0);
    return 20 * qLn(value);
}


void audio_visualizer::process_buffer(const QAudioBuffer &buffer) {
    const SAMPLE_TYPE *raw_data = reinterpret_cast<const SAMPLE_TYPE *>(buffer.constData());

    Q_ASSERT(SAMPLE_RATE == buffer.format().sampleRate());
    Q_ASSERT(SAMPLE_SIZE == buffer.format().sampleSize());

    for (int i = 0; i < buffer.sampleCount(); i += 2) {
        m_samples_buffer.push_back(qMax(raw_data[i], raw_data[i + 1]));
    }
}


void audio_visualizer::draw_rects() {
    QPainter painter(this);

    int w = width() * 4 / 5;
    int h = height();
    int dx = (width() - w) / 2;
    int dy = 0;

    float rect_fill_factor = 0.8;

    float rect_area = 1. * w / m_spectrum.size();
    float rect_width = rect_area * rect_fill_factor;
    float rect_spacing = rect_area * (1 - rect_fill_factor) / 2;

    for (int i = 0; i < m_spectrum.size(); ++i) {
        float spectrum_value{};
        if (INTERPOLATION) {
            spectrum_value = linear_interpolation(
                m_old_spectrum[i], m_spectrum[i],
                1, 2
            );
        } else {
            spectrum_value = m_spectrum[i];
        }

        if (spectrum_value) {
            int c = 255 - qMin(spectrum_value / 8'000, 180.f);
            spectrum_value *= height() / 7'200'000.f;
            float x = dx + i * rect_area + rect_spacing;
            float y = dy + h / 2 - spectrum_value / 2;

            QColor rect_color(c, c, c);
            painter.fillRect(x, y, rect_width, spectrum_value, rect_color);
        }
    }
}


void audio_visualizer::draw_info() {
    QPainter painter(this);
    painter.setPen(Qt::black);
    std::string text(
        std::string("FPS: ") + std::to_string(1'000 / qMax(m_fps_timer.restart(), 1ll))
        + std::string(" Delay: ") + std::to_string(m_delay_timer.restart())
    );
    painter.drawText(0, fontMetrics().height(), QString(text.c_str()));
}


void audio_visualizer::process_dft() {
    m_delay_timer.restart();
    qint64 audio_pos = get_audio_position() + DELAY;
    
    if (audio_pos >= m_samples_buffer.size() + m_first_sample_index) {
        return;
    }

    for (int i = 0; i < DFT_SIZE; ++i) {
        qint64 buff_index = audio_pos - DFT_SIZE + i + 1 - m_first_sample_index;
        if (buff_index >= 0) {
            float windowing = 0.54 * - 0.46 * qCos(2.0 * M_PI * i / (DFT_SIZE - 1));
            m_dft_input[2 * i] = m_samples_buffer[buff_index] * windowing;
            m_dft_input[2 * i + 1] = 0;
        }
    }

    qint64 first_sample_index = audio_pos - DFT_SIZE + 1;
    if (first_sample_index > m_first_sample_index) {
        m_samples_buffer = QVector<SAMPLE_TYPE>(
            m_samples_buffer.begin() + first_sample_index - m_first_sample_index,
            m_samples_buffer.end()
        );
        m_first_sample_index = first_sample_index;
    }

    fftwf_execute(m_dft_plan);
}


void audio_visualizer::compute_magnitudes() {
    for (int i = 0; i < m_magnitudes.size(); ++i) {
        m_magnitudes[i] = std::abs(reinterpret_cast<std::complex<float> *>(m_dft_output)[i]);
    }
}


void audio_visualizer::backup_spectrum() {
    Q_ASSERT(m_spectrum.size() == m_old_spectrum.size());
    // if (m_old_spectrum == m_spectrum) {
    //     qDebug("Frame stacking");
    // }
    for (int i = 0; i < m_spectrum.size(); ++i) {
        m_old_spectrum[i] = m_spectrum[i];
    }
}


void audio_visualizer::compute_spectrum() {
    Q_ASSERT(m_spectrum.size() == SPECTRUM_SIZE);
    Q_ASSERT(m_spectrum.size() == m_spectrum_values.size());

    for (int i = 0; i < m_spectrum_values.size(); ++i) {
        m_spectrum_values[i].append(0);
        while (m_spectrum_values[i].size() > MEAN_SIZE) {
            m_spectrum_values[i].removeFirst();
        }
    }

    for (int i = 0, j = 0; i < m_magnitudes.size() && j < m_spectrum.size(); ++i) {
        float frequency = 1. * i / DFT_SIZE * SAMPLE_RATE;
        float &spectrum_value = m_spectrum_values[j].last();

        if (m_frequency_bins[j] > frequency) {
            continue;
        }

        while (j < SPECTRUM_SIZE && frequency > m_frequency_bins[j + 1]) {
            ++j;
        }

        if (j >= SPECTRUM_SIZE) {
            break;
        }

        if (m_frequency_bins[j] <= frequency && frequency <= m_frequency_bins[j + 1]) {
            spectrum_value = qMax(spectrum_value, m_magnitudes[i]);
        }
    }

    for (int i = 0; i < m_spectrum_values.size(); ++i) {
        qint64 sum = 0;
        for (float spectrum_value : m_spectrum_values[i]) {
            sum += spectrum_value;
        }
        m_spectrum[i] = sum / m_spectrum_values[i].size();
    }

}


// void audio_visualizer::scale_spectrum() {

// }


qint64 audio_visualizer::get_audio_position() {
    Q_ASSERT(m_player->position() <= m_player->duration());
    qint64 pos = qRound(m_player->position() * SAMPLE_RATE / 1'000.0);
    // qDebug("Audio position: %lld, samples probed: %d", pos, m_samples_buffer.size());
    return pos;
}


void audio_visualizer::reset_buffers() {
    m_samples_buffer.clear();
    m_first_sample_index = 0;
    for (int i = 0; i < m_spectrum_values.size(); ++i) {
        m_spectrum_values[i].clear();
    }
}


}  // namespace audio_app
