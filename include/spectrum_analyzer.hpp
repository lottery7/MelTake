#ifndef SPECTRUM_ANALYZER_HPP
#define SPECTRUM_ANALYZER_HPP

#include <QMediaPlayer>
#include <QObject>
#include "audio_decoder.hpp"

namespace audio_app {

class spectrum_analyzer : public QObject {
    Q_OBJECT

public:
    explicit spectrum_analyzer(QObject *parent = nullptr);

    void calculate_frequency_bins();
    void update_spectrum();

    audio_decoder &get_decoder();

    qint64 get_spectrum_size() const;
    const QVector<double> &get_spectrum() const;

    double get_max_magnitude() const;

private:
    double get_freq_from_fft_index(qint64 index) const;

    double m_min_frequency;
    double m_max_frequency;
    qint64 m_spectrum_size;
    audio_decoder m_decoder;
    double m_max_magnitude;
    QVector<double> m_frequency_bins;
    QVector<double> m_spectrum;
};

}  // namespace audio_app

#endif  // SPECTRUM_ANALYZER_HPP
