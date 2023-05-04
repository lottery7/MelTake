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
    audio_decoder &get_decoder();
    void update_frequency_bins();
    QVector<double> get_spectrum();
    double get_max_magnitude();

private:
    audio_decoder m_decoder;
    double m_max_magnitude;
    QVector<double> m_frequency_bins;
};

}  // namespace audio_app

#endif  // SPECTRUM_ANALYZER_HPP
