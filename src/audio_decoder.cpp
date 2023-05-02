#include "audio_decoder.hpp"
#include <fftw3.h>
#include <QAudioProbe>
#include <QMediaPlayer>
#include <QtMath>
#include <complex>

namespace audio_app {

extern const int SAMPLE_RATE = 44100;
const int SAMPLE_SIZE = 16;
extern const int FFT_SIZE = 16'384;
const int DELAY = 8'000;

audio_decoder::audio_decoder(QObject *parent, QMediaPlayer *player)
    : QObject(parent),
      m_fft_input(fftw_alloc_real(FFT_SIZE * 2)),
      m_fft_output(fftw_alloc_complex(FFT_SIZE)),
      m_window_values(FFT_SIZE, -1.0),
      m_magnitudes(FFT_SIZE / 4),
      m_first_sample_index() {
    m_fft_plan =
        fftw_plan_dft_r2c_1d(FFT_SIZE, m_fft_input, m_fft_output, FFTW_MEASURE);
    set_player(player);
}

void audio_decoder::set_player(QMediaPlayer *player) {
    if (!player) {
        qDebug("audio_decoder: Player is nullptr");
        return;
    }

    reset_buffers();
    connect(
        player, SIGNAL(currentMediaChanged(const QMediaContent &)), this,
        SLOT(reset_buffers())
    );
    connect(
        player, SIGNAL(positionChanged(qint64)), this,
        SLOT(set_audio_position(qint64))
    );
    player->setNotifyInterval(0);

    m_probe.setSource(player);
    connect(
        &m_probe, SIGNAL(audioBufferProbed(const QAudioBuffer &)), this,
        SLOT(process_buffer(const QAudioBuffer &))
    );
}

QVector<double> audio_decoder::get_magnitudes() {
    process_fft();
    for (int i = 0; i < m_magnitudes.size(); ++i) {
        m_magnitudes[i] =
            std::abs(reinterpret_cast<std::complex<double> *>(m_fft_output)[i]);
    }

    return m_magnitudes;
}

audio_decoder::~audio_decoder() {
    fftw_free(m_fft_input);
    fftw_free(m_fft_output);
    fftw_destroy_plan(m_fft_plan);
}

void audio_decoder::process_buffer(const QAudioBuffer &buffer) {
    const SAMPLE_TYPE *raw_data =
        reinterpret_cast<const SAMPLE_TYPE *>(buffer.constData());

    Q_ASSERT(SAMPLE_RATE == buffer.format().sampleRate());
    Q_ASSERT(SAMPLE_SIZE == buffer.format().sampleSize());

    for (int i = 0; i < buffer.sampleCount(); i += 2) {
        m_samples_buffer.push_back(qMax(raw_data[i], raw_data[i + 1]));
    }
}

void audio_decoder::process_fft() {
    qint64 audio_pos = m_audio_position + DELAY;

    if (audio_pos >= m_samples_buffer.size() + m_first_sample_index) {
        return;
    }

    for (int i = 0; i < FFT_SIZE; ++i) {
        qint64 buff_index = audio_pos - FFT_SIZE + i + 1 - m_first_sample_index;
        if (buff_index >= 0) {
            m_fft_input[2 * i] = m_samples_buffer[buff_index] * get_window(i);
            m_fft_input[2 * i + 1] = 0;
        }
    }

    qint64 first_sample_index = audio_pos - FFT_SIZE + 1;
    if (first_sample_index > m_first_sample_index + 10'000) {
        m_samples_buffer = QVector<SAMPLE_TYPE>(
            m_samples_buffer.begin() + first_sample_index -
                m_first_sample_index,
            m_samples_buffer.end()
        );
        m_first_sample_index = first_sample_index;
    }

    fftw_execute(m_fft_plan);
}

double audio_decoder::get_window(int n) {
    Q_ASSERT(0 <= n && n < FFT_SIZE);
    if (m_window_values[n] != -1.0) {
        return m_window_values[n];
    }
    double N = FFT_SIZE - 1;
    return m_window_values[n] = 0.5 - 0.5 * qCos(2 * M_PI * n / N);
}

void audio_decoder::reset_buffers() {
    m_samples_buffer.clear();
    m_first_sample_index = 0;
}

void audio_decoder::set_audio_position(qint64 new_position) {
    m_audio_position = qRound(new_position * SAMPLE_RATE / 1'000.0);
}

}  // namespace audio_app
