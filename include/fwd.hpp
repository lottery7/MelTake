#ifndef FWD_HPP
#define FWD_HPP

#include <fstream>
#include "playlist_structure.hpp"
#include "settings_structure.hpp"
#include "track_structure.hpp"
#include <sstream>
#define TRACK_DATA "track_data.txt"

namespace audio_app {

struct music_base {
    std::vector<playlist> playlist_list;
    settings settings_type;

};

struct unable_to_find_users_data : std::runtime_error {
    explicit unable_to_find_users_data()
        : std::runtime_error("Unable to find user's track data file") {
    }
};

struct unable_to_find_the_track_in_playlist : std::runtime_error {
    explicit unable_to_find_the_track_in_playlist()
        : std::runtime_error("Unable to find the track in the playlist") {
    }
};

}  // namespace audio_app
#endif  // FWD_HPP
