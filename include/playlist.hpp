#ifndef PLAYLIST_STRUCTURE_HPP
#define PLAYLIST_STRUCTURE_HPP

#include "track_structure.hpp"
#include <iostream>
#include <utility>
#include <vector>

namespace audio_app {
struct playlist {
    std::string m_playlist_name;
    std::vector<track> m_tracks;

    playlist() = default;

    explicit playlist(std::string name) : m_playlist_name(std::move(name)){};

    void add_new_track_to_playlist(const track &track_name);
    void add_old_track_to_playlist(const track &track_name);
    void delete_track_from_playlist(track &track_name);
};

}  // namespace audio_app
#endif  // PLAYLIST_STRUCTURE_HPP
