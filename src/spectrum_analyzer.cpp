#include "spectrum_analyzer.hpp"
#include <QtMath>

namespace audio_app {

extern const int FFT_SIZE;
extern const int SAMPLE_RATE;
extern const int SPECTRUM_SIZE = 40;
const double FREQ_MIN = 20;
const double FREQ_MAX = 3'000;

spectrum_analyzer::spectrum_analyzer(QObject *parent)
    : QObject(parent), m_decoder(this), m_frequency_bins(SPECTRUM_SIZE + 1) {
    update_frequency_bins();
}

audio_decoder &spectrum_analyzer::get_decoder() {
    return m_decoder;
}

void spectrum_analyzer::update_frequency_bins() {
    double base = qPow(FREQ_MAX / FREQ_MIN, 1.f / (SPECTRUM_SIZE - 1));

    m_frequency_bins[0] = FREQ_MIN;
    for (int i = 0; i < SPECTRUM_SIZE - 1; ++i) {
        m_frequency_bins[i + 1] = m_frequency_bins[i] * base;
    }
    m_frequency_bins[SPECTRUM_SIZE] = SAMPLE_RATE / 2;
}

QVector<double> spectrum_analyzer::get_spectrum() {
    QVector<double> magnitudes = m_decoder.get_magnitudes();
    QVector<double> spectrum(SPECTRUM_SIZE);
    m_max_magnitude = 0;

    for (int i = 0, j = 0; i < magnitudes.size() && j < spectrum.size(); ++i) {
        double frequency = 1. * i / FFT_SIZE * SAMPLE_RATE;
        double &spectrum_value = spectrum[j];

        while (j < SPECTRUM_SIZE && frequency > m_frequency_bins[j + 1]) {
            ++j;
        }

        if (j >= SPECTRUM_SIZE) {
            break;
        }

        if (m_frequency_bins[j] <= frequency &&
            frequency <= m_frequency_bins[j + 1]) {
            spectrum_value = qMax(spectrum_value, magnitudes[i]);
            m_max_magnitude = qMax(m_max_magnitude, spectrum_value);
        }
    }

    return spectrum;
}

double spectrum_analyzer::get_max_magnitude() {
    return m_max_magnitude;
}

}  // namespace audio_app
