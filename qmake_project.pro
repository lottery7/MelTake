QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/playlist_functions.cpp \
    src/settings_functions.cpp

HEADERS += \
    mainwindow.h \
    include/fwd.hpp \
    include/playlist_structure.hpp \
    include/settings_structure.hpp \
    include/track_structure.hpp

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -LC:/Users/HUAWEI/Downloads/taglib-1.13.tar/taglib-1.13/taglib-1.13/build/lib/ -ltag

INCLUDEPATH += C:/Users/HUAWEI/Downloads/taglib-1.13.tar/taglib-1.13/taglib-1.13/build/include
DEPENDPATH += C:/Users/HUAWEI/Downloads/taglib-1.13.tar/taglib-1.13/taglib-1.13/build/include
