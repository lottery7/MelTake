#include "spectrum_analyzer.hpp"
#include <QtMath>

namespace audio_app {

spectrum_analyzer::spectrum_analyzer(QObject *parent)
    : QObject(parent),
      m_min_frequency(32),
      m_max_frequency(16'384),
      m_spectrum_size(64),
      m_decoder(this),
      m_max_magnitude() {
    calculate_frequency_bins();
}

audio_decoder &spectrum_analyzer::get_decoder() {
    return m_decoder;
}

qint64 spectrum_analyzer::get_spectrum_size() const {
    return m_spectrum_size;
}

void spectrum_analyzer::calculate_frequency_bins() {
    m_frequency_bins.resize(m_spectrum_size + 1);
    m_frequency_bins[0] = 0;
    m_frequency_bins[1] = m_min_frequency;

    double base =
        qPow(m_max_frequency / m_min_frequency, 1. / (m_spectrum_size - 1));
    for (int i = 1; i < m_spectrum_size; ++i) {
        m_frequency_bins[i + 1] = m_frequency_bins[i] * base;
    }
}

void spectrum_analyzer::update_spectrum() {
    m_decoder.update_magnitudes();
    QVector<double> magnitudes = m_decoder.get_magnitudes();
    m_max_magnitude = 0;

    m_spectrum.resize(m_spectrum_size);
    std::fill(m_spectrum.begin(), m_spectrum.end(), 0);

    for (int i = 0, j = 0; i < magnitudes.size() && j < m_spectrum.size();
         i++) {
        double frequency = get_freq_from_fft_index(i);

        while (j < m_spectrum_size && frequency > m_frequency_bins[j + 1]) {
            j++;
        }

        if (j >= m_spectrum_size) {
            break;
        }

        if (m_frequency_bins[j] <= frequency &&
            frequency <= m_frequency_bins[j + 1]) {
            m_spectrum[j] = qMax(m_spectrum[j], magnitudes[i]);

            // if (j >= m_spectrum_size / 2) {
            //     m_spectrum[j] *= 1. + 2.e-2;
            // }

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
    // return m_max_magnitude;
    double sum = 0;
    for (const double &magnitude : m_spectrum) {
        sum += magnitude;
    }

    return sum / m_spectrum.size();
}

double spectrum_analyzer::get_freq_from_fft_index(qint64 index) const {
    return 1. * index / m_decoder.get_fft_size() *
           m_decoder.get_audio_format().sampleRate();
}

}  // namespace audio_app
