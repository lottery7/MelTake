#include "audio_decoder.hpp"
#include <fftw3.h>
#include <QtMath>
#include <complex>

namespace audio_app {

const int SAMPLE_SIZE = 16;
extern const int FFT_SIZE = 4096;
const int DELAY = 4'096;

audio_decoder::audio_decoder(QObject *parent)
    : QObject(parent),
      m_prev_buffer_start_time(),
      m_fft_input(fftw_alloc_real(FFT_SIZE)),
      m_fft_output(fftw_alloc_complex(FFT_SIZE / 2 + 1)),
      m_fft_plan(fftw_plan_dft_r2c_1d(
          FFT_SIZE,
          m_fft_input,
          m_fft_output,
          FFTW_MEASURE
      )),
      m_window_values(FFT_SIZE, -1),
      m_magnitudes(FFT_SIZE / 2 + 1),
      m_first_sample_index() {
}

audio_decoder::~audio_decoder() {
    fftw_free(m_fft_input);
    fftw_free(m_fft_output);
    fftw_destroy_plan(m_fft_plan);
}

bool audio_decoder::set_player(QMediaPlayer *player) {
    if (!player || !m_probe.setSource(player)) {
        return false;
    }

    m_player = player;
    reset_samples_buffer();

    connect(
        &m_probe, SIGNAL(audioBufferProbed(const QAudioBuffer &)), this,
        SLOT(process_buffer(const QAudioBuffer &))
    );

    return true;
}

void audio_decoder::update_magnitudes() {
    process_fft();
    for (int i = 0; i < m_magnitudes.size(); ++i) {
        m_magnitudes[i] =
            std::abs(reinterpret_cast<std::complex<double> *>(m_fft_output)[i]);
    }
}

QVector<double> audio_decoder::get_magnitudes() const {
    return m_magnitudes;
}

QAudioFormat audio_decoder::get_audio_format() const {
    return m_current_audio_format;
}

void audio_decoder::process_buffer(const QAudioBuffer &buffer) {
    m_current_audio_format = buffer.format();

    if (buffer.startTime() < m_prev_buffer_start_time) {
        // Audio file has changed (another audio or replay)
        reset_samples_buffer();
    }

    m_prev_buffer_start_time = buffer.startTime();
    const SAMPLE_TYPE *raw_data =
        reinterpret_cast<const SAMPLE_TYPE *>(buffer.constData());

    Q_ASSERT(SAMPLE_SIZE == buffer.format().sampleSize());

    for (int i = 0; i < buffer.sampleCount();
         i += buffer.format().channelCount()) {
        // Get sample from only one channel
        // If you get maximum from all channels, then you will have harmonic
        // distortion
        m_samples_buffer.push_back(raw_data[i]);
    }
}

void audio_decoder::process_fft() {
    qint64 audio_pos = get_audio_position() + DELAY;

    if (audio_pos >= m_samples_buffer.size() + m_first_sample_index) {
        qDebug() << "Audio is too fast: delay is "
                 << audio_pos - m_samples_buffer.size() - m_first_sample_index;
    }

    for (int i = 0; i < FFT_SIZE; ++i) {
        qint64 buffer_index =
            audio_pos - FFT_SIZE + i + 1 - m_first_sample_index;

        if (buffer_index >= 0 && buffer_index < m_samples_buffer.size()) {
            double window = get_cached_window_value(i);
            m_fft_input[i] = m_samples_buffer[buffer_index] * window;
        } else {
            m_fft_input[i] = 0;
        }
    }

    fftw_execute(m_fft_plan);

    // Cut buffer to prevent high memory usage
    qint64 first_sample_index = audio_pos - FFT_SIZE + 1;

    if (first_sample_index > m_first_sample_index + 10'000) {
        m_samples_buffer = QVector<SAMPLE_TYPE>(
            m_samples_buffer.begin() + first_sample_index -
                m_first_sample_index,
            m_samples_buffer.end()
        );
        m_first_sample_index = first_sample_index;
    }
}

double audio_decoder::get_cached_window_value(int n) {
    Q_ASSERT(0 <= n && n < FFT_SIZE);

    if (m_window_values[n] != -1) {
        return m_window_values[n];
    }

    double N = FFT_SIZE - 1;
    return m_window_values[n] = 0.54 - 0.46 * qCos(2 * M_PI * n / N);
}

void audio_decoder::reset_samples_buffer() {
    m_samples_buffer.resize(0);
    m_first_sample_index = 0;
}

qint64 audio_decoder::get_audio_position() const {
    if (m_current_audio_format.sampleRate() == -1) {
        return 0;
    }

    return qFloor(
        m_player->position() * m_current_audio_format.sampleRate() / 1'000.0
    );
}

}  // namespace audio_app
