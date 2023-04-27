#ifndef AUDIO_VISUALIZER_HPP
#define AUDIO_VISUALIZER_HPP


#include "audio_constants.hpp"
// #include "audio_decoder.hpp"
#include <QAudioProbe>
#include <QMediaPlayer>
#include <QTimer>
#include <QVector>
#include <QList>
#include <QWidget>
#include <complex>
#include <fftw3.h>


namespace audio_app {

class audio_visualizer : public QWidget {
    Q_OBJECT

public:
    explicit audio_visualizer(QWidget *parent = nullptr);
    void set_player(QMediaPlayer *player);
    static float linear_interpolation(float x1, float x2, float step, float max);
    static float to_decibels(float value);

public slots:
    void add_sample(SAMPLE_TYPE sample);
    void reset_buffers();
    
protected:
    void paintEvent(QPaintEvent *) override;

private slots:
    void process_buffer(const QAudioBuffer &buffer);

private:
    void draw_rects();
    void draw_info();
    void process_dft();
    void compute_magnitudes();
    void backup_spectrum();
    void compute_spectrum();
    qint64 get_audio_position();

    QMediaPlayer *m_player;
    // audio_decoder m_decoder;
    QAudioProbe m_probe;

    float *m_dft_input;
    fftwf_complex *m_dft_output;
    fftwf_plan m_dft_plan;

    QVector<SAMPLE_TYPE> m_samples_buffer;
    qint64 m_first_sample_index;
    QVector<float> m_magnitudes;
    QVector<QList<float>> m_spectrum_values;
    QVector<float> m_spectrum;
    QVector<float> m_old_spectrum;
    QVector<float> m_frequency_bins;

    QElapsedTimer m_fps_timer;
    QElapsedTimer m_delay_timer;
};


}  // namespace audio_app

#endif  // AUDIO_VISUALIZER_HPP
