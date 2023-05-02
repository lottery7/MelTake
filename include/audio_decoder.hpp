#ifndef AUDIO_DECODER_HPP
#define AUDIO_DECODER_HPP

#include <fftw3.h>
#include <QAudioBuffer>
#include <QAudioProbe>
#include <QMediaPlayer>
#include <QObject>
#include <QVector>

namespace audio_app {

typedef qint16 SAMPLE_TYPE;

class audio_decoder : public QObject {
    Q_OBJECT

public:
    explicit audio_decoder(
        QObject *parent = nullptr,
        QMediaPlayer *player = nullptr
    );
    void set_player(QMediaPlayer *player);
    QVector<double> get_magnitudes();
    ~audio_decoder();

private slots:
    void process_buffer(const QAudioBuffer &buffer);
    void reset_buffers();
    void set_audio_position(qint64 new_position);

private:
    void process_fft();
    double get_window(int n);

    QAudioProbe m_probe;
    qint64 m_audio_position;

    double *m_fft_input;
    fftw_complex *m_fft_output;
    fftw_plan m_fft_plan;

    QVector<double> m_window_values;
    QVector<double> m_magnitudes;
    QVector<SAMPLE_TYPE> m_samples_buffer;
    qint64 m_first_sample_index;
};

}  // namespace audio_app

#endif  // AUDIO_DECODER_HPP
