#ifndef AUDIO_DECODER_HPP
#define AUDIO_DECODER_HPP

#include <fftw3.h>
#include <QAudioBuffer>
#include <QAudioProbe>
#include <QMediaPlayer>
#include <QObject>
#include <QVector>

namespace audio_app {

class audio_decoder : public QObject {
    Q_OBJECT

public:
    explicit audio_decoder(QObject *parent = nullptr);
    // explicit audio_decoder(QObject *parent = nullptr);
    ~audio_decoder();

    bool set_player(QMediaPlayer *player);

    qint32 get_fft_size() const;

    void update_magnitudes();
    QVector<double> get_magnitudes() const;

    QAudioFormat get_audio_format() const;

    double get_cached_window_value(int n);

private slots:
    void process_buffer(const QAudioBuffer &buffer);
    void reset_samples_buffer();

    qint64 get_audio_position() const;

private:
    void process_fft();

    QAudioProbe m_probe;
    const QMediaPlayer *m_player;
    QAudioFormat m_current_audio_format;
    qint64 m_prev_buffer_start_time;
    qint64 m_buffering_start_time_mcs;

    qint32 m_fft_size;
    qint32 m_fft_window_size;
    double *m_fft_input;
    fftw_complex *m_fft_output;
    fftw_plan m_fft_plan;

    QVector<double> m_window_values;
    QVector<double> m_magnitudes;
    QVector<double> m_samples_buffer;
    qint64 m_first_sample_index;
};

}  // namespace audio_app

#endif  // AUDIO_DECODER_HPP
