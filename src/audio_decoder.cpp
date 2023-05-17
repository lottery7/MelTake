#include "audio_decoder.hpp"
#include <fftw3.h>
#include <QAudioProbe>
#include <QMediaPlayer>
#include <QtMath>
#include <complex>

namespace audio_app {

extern const int SAMPLE_RATE = 44100;
const int SAMPLE_SIZE = 16;
extern const int FFT_SIZE = 10'000;
const int DELAY = 8'192;

audio_decoder::audio_decoder(QObject *parent, QMediaPlayer *player)
    : QObject(parent),
      m_audio_position(),
      m_prev_buffer_start_time(),
      m_fft_input(fftw_alloc_real(FFT_SIZE)),
      m_fft_output(fftw_alloc_complex(FFT_SIZE / 2 + 1)),
      m_fft_plan(fftw_plan_dft_r2c_1d(
          FFT_SIZE,
          m_fft_input,
          m_fft_output,
          FFTW_MEASURE
      )),
      m_window_values(FFT_SIZE, -1.0),
      m_magnitudes(FFT_SIZE / 2 + 1),
      m_first_sample_index() {
    set_player(player);
}

void audio_decoder::set_player(QMediaPlayer *player) {
    if (!player) {
        qDebug("audio_decoder: Player is nullptr");
        return;
    }

    // New player => need to reset buffer
    reset_sample_buffer();

    // We need small interval to synchronize buffer with audio
    player->setNotifyInterval(5);
    connect(
        player, SIGNAL(positionChanged(qint64)), this,
        SLOT(set_audio_position(qint64))
    );

    // QAudioProbe objects emits pieces of buffer with samples
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
    if (buffer.startTime() < m_prev_buffer_start_time) {
        // Audio file changed (another audio or replay)
        reset_sample_buffer();
    }
    m_prev_buffer_start_time = buffer.startTime();
    const SAMPLE_TYPE *raw_data =
        reinterpret_cast<const SAMPLE_TYPE *>(buffer.constData());

    Q_ASSERT(SAMPLE_RATE == buffer.format().sampleRate());
    Q_ASSERT(SAMPLE_SIZE == buffer.format().sampleSize());

    for (int i = 0; i < buffer.sampleCount(); i += buffer.format().channelCount()) {
        // Get sample from only one channel
        // If you get maximum from all channels, then you will have harmonic distortion
        m_samples_buffer.push_back(raw_data[i]);
    }
}

void audio_decoder::process_fft() {
    qint64 audio_pos = m_audio_position + DELAY;

    if (audio_pos >= m_samples_buffer.size() + m_first_sample_index) {
        qDebug() << "Audio is too fast: delay is "
                 << audio_pos - m_samples_buffer.size() - m_first_sample_index;
    }

    for (int i = 0; i < FFT_SIZE; ++i) {
        qint64 buffer_index =
            audio_pos - FFT_SIZE + i + 1 - m_first_sample_index;
        if (buffer_index >= 0 && buffer_index < m_samples_buffer.size()) {
            m_fft_input[i] = m_samples_buffer[buffer_index] * hamming(i);
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

double audio_decoder::hamming(int n) {
    Q_ASSERT(0 <= n && n < FFT_SIZE);
    if (m_window_values[n] != -1.0) {
        return m_window_values[n];
    }
    double N = FFT_SIZE - 1;
    return m_window_values[n] = 0.54 - 0.46 * qCos(2 * M_PI * n / N);
}

void audio_decoder::reset_sample_buffer() {
    m_samples_buffer.resize(0);
    m_first_sample_index = 0;
}

void audio_decoder::set_audio_position(qint64 new_position) {
    double new_position_samples = qFloor(new_position * SAMPLE_RATE / 1'000.0);
    m_audio_position = new_position_samples;
}

}  // namespace audio_app
