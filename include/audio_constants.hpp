#ifndef AUDIO_CONSTANTS_HPP
#define AUDIO_CONSTANTS_HPP

#include <QtCore>
#include <QMetaType>


namespace audio_app {


typedef qint16 SAMPLE_TYPE;
static const int SAMPLE_RATE = 44100;
static const float SAMPLE_SIZE = 16;

static const float FRAMES_PER_SECOND = 50.;
static const float MS_PER_FRAME = (1000 / FRAMES_PER_SECOND);
static const float SAMPLES_PER_FRAME = SAMPLE_RATE / FRAMES_PER_SECOND;


}  // namespace audio_app


#endif  // AUDIO_CONSTANTS_HPP
