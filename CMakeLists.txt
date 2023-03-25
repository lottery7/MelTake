cmake_minimum_required(VERSION 3.5)
project(audio_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Multimedia REQUIRED)

#include_directories(header_files)

set(HEADERS
    include/fwd.hpp;
    include/playlist_structure.hpp;
    include/settings_structure.hpp;
    include/track_structure.hpp;
)

set(SOURCES
    src/playlist_functions.cpp
    src/settings_functions.cpp
    src/track_functions.cpp
    src/music_base.cpp
)


add_executable(${PROJECT_NAME}
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${HEADERS}
        ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ./)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Multimedia)
