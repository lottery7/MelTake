TARGET = audio_app

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

CONFIG += c++17 console debug

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/audio_decoder.cpp \
    src/spectrum_analyzer.cpp \
    src/audio_visualizer.cpp

INCLUDEPATH += include

HEADERS += \
    mainwindow.h \
    include/audio_decoder.hpp \
    include/spectrum_analyzer.hpp \
    include/audio_visualizer.hpp
    
FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

win32: RC_ICONS += resources/ghost_icon.ico

# Link taglib
win32: LIBS += -LD:/QT/Libs/taglib/finish/lib/ -llibtag.dll
INCLUDEPATH += D:/QT/Libs/taglib/finish/include
DEPENDPATH += D:/QT/Libs/taglib/finish/include

# Link FFTW3
INCLUDEPATH += D:/QT/Libs/fftw/
LIBS += -LD:/QT/Libs/fftw/ -lfftw3-3
