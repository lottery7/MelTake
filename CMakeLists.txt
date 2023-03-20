cmake_minimum_required(VERSION 3.22)
project(audio_app)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(audio_app include/fwd.hpp
        src/playlist_functions.cpp include/playlist_structure.hpp
        src/track_functions.cpp include/track_structure.hpp
        src/settings_functions.cpp include/settings_structure.hpp
        src/main.cpp src/music_base.cpp)
