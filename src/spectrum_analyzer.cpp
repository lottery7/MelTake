#include "spectrum_analyzer.hpp"
#include <QtMath>

namespace audio_app {

extern const int FFT_SIZE;
extern const int SPECTRUM_SIZE = 40;
const double FREQ_MIN = 64;
const double FREQ_MAX = 8'192;

spectrum_analyzer::spectrum_analyzer(QObject *parent)
    : QObject(parent), m_decoder(this), m_max_magnitude() {
    calculate_frequency_bins();
}

audio_decoder &spectrum_analyzer::get_decoder() {
    return m_decoder;
}

void spectrum_analyzer::calculate_frequency_bins() {
    m_frequency_bins.resize(SPECTRUM_SIZE + 1);
    m_frequency_bins[0] = 0;
    m_frequency_bins[1] = FREQ_MIN;

    double base = qPow(FREQ_MAX / FREQ_MIN, 1. / (SPECTRUM_SIZE - 1));
    for (int i = 1; i < SPECTRUM_SIZE; ++i) {
        m_frequency_bins[i + 1] = m_frequency_bins[i] * base;
    }
}

void spectrum_analyzer::update_spectrum() {
    m_decoder.update_magnitudes();
    QVector<double> magnitudes = m_decoder.get_magnitudes();
    m_max_magnitude = 0;

    m_spectrum.resize(SPECTRUM_SIZE);
    std::fill(m_spectrum.begin(), m_spectrum.end(), 0);

    for (int i = 0, j = 0; i < magnitudes.size() && j < m_spectrum.size();
         i++) {
        double frequency = get_freq_from_fft_index(i);

        while (j < SPECTRUM_SIZE && frequency > m_frequency_bins[j + 1]) {
            j++;
        }

        if (j >= SPECTRUM_SIZE) {
            break;
        }

        if (m_frequency_bins[j] <= frequency &&
            frequency <= m_frequency_bins[j + 1]) {
            m_spectrum[j] = qMax(m_spectrum[j], magnitudes[i]);

            if (j >= SPECTRUM_SIZE / 2) {
                m_spectrum[j] *= 1. + 2.e-2;
            }

            // if (j < 10) {
            m_max_magnitude = qMax(m_max_magnitude, m_spectrum[j]);
            // }
        }
    }
}

const QVector<double> &spectrum_analyzer::get_spectrum() const {
    return m_spectrum;
}

double spectrum_analyzer::get_max_magnitude() const {
    return m_max_magnitude;
}

double spectrum_analyzer::get_freq_from_fft_index(qint64 index) const {
    return 1. * index / FFT_SIZE * m_decoder.get_audio_format().sampleRate();
}

}  // namespace audio_app
