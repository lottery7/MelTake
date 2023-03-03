cmake_minimum_required(VERSION 3.22)
project(audio_app)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(audio_app fwd.hpp
        playlist_functions.cpp playlist_structure.hpp
        track_functions.cpp track_structure.hpp
        settings_functions.cpp settings_structure.hpp
        main.cpp)
