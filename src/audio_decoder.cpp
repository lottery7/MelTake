#include "audio_decoder.hpp"
#include <fftw3.h>
#include <QtMath>
#include <complex>

namespace audio_app {

audio_decoder::audio_decoder(QObject *parent)
    : QObject(parent),
      m_prev_buffer_start_time(INT64_MAX),
      m_buffering_start_time_mcs(),
      m_fft_size(16'384),
      m_fft_window_size(4096),
      m_fft_input(fftw_alloc_real(m_fft_size)),
      m_fft_output(fftw_alloc_complex(m_fft_size / 2 + 1)),
      m_fft_plan(fftw_plan_dft_r2c_1d(
          m_fft_size,
          m_fft_input,
          m_fft_output,
          FFTW_MEASURE
      )),
      m_window_values(m_fft_size, -1),
      m_magnitudes(m_fft_size / 2 + 1),
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

qint32 audio_decoder::get_fft_size() const {
    return m_fft_size;
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
        m_buffering_start_time_mcs = buffer.startTime();
    }
    m_prev_buffer_start_time = buffer.startTime();

    for (int i = 0; i < buffer.sampleCount();
         i += buffer.format().channelCount()) {
        // Get sample from only one channel
        // If you get maximum from all channels, then you will have harmonic
        // distortion

        double sample = 0;

        switch (buffer.format().sampleType()) {
            case QAudioFormat::SampleType::Float: {
                sample = reinterpret_cast<const float *>(buffer.constData())[i];
            } break;

            case QAudioFormat::SampleType::UnSignedInt: {
                sample =
                    reinterpret_cast<const quint16 *>(buffer.constData())[i];
            } break;

            case QAudioFormat::SampleType::SignedInt: {
                sample =
                    reinterpret_cast<const qint16 *>(buffer.constData())[i];
            } break;

            case QAudioFormat::SampleType::Unknown: {
                qDebug() << "audio_decoder: Audio format is not supported";
                return;
            } break;
        }

        m_samples_buffer.push_back(sample);
    }
}

void audio_decoder::process_fft() {
    qint64 buffer_start = get_audio_position() - m_first_sample_index;

    if (buffer_start >= m_samples_buffer.size()) {
        qDebug() << "Audio is too fast: delay is "
                 << buffer_start - m_samples_buffer.size() + 1;
    }

    for (int i = 0; i < m_fft_size; i++) {
        qint64 buffer_index = buffer_start + i;

        if (i < m_fft_window_size && buffer_index >= 0 &&
            buffer_index < m_samples_buffer.size()) {
            double window = get_cached_window_value(i);
            m_fft_input[i] = m_samples_buffer[buffer_index] * window;
        } else {
            m_fft_input[i] = 0;
        }
    }

    fftw_execute(m_fft_plan);

    // Cut buffer to prevent high memory usage
    if (buffer_start > 10'000) {
        m_samples_buffer = QVector<double>(
            m_samples_buffer.begin() + buffer_start, m_samples_buffer.end()
        );
        m_first_sample_index += buffer_start;
    }
}

double audio_decoder::get_cached_window_value(int n) {
    Q_ASSERT(0 <= n && n < m_fft_size);

    if (m_window_values[n] != -1) {
        return m_window_values[n];
    }

    double N = m_fft_window_size - 1;
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
        (m_player->position() - m_buffering_start_time_mcs) *
        m_current_audio_format.sampleRate() / 1'000.0
    );
}

}  // namespace audio_app
