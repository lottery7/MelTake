#ifndef PLAYLIST_STRUCTURE_HPP
#define PLAYLIST_STRUCTURE_HPP

#include <iostream>
#include <track_structure.hpp>
#include <vector>

namespace audio_app {
struct playlist {
    std::string m_playlist_name;
    std::vector<track> m_tracks{};

    playlist() = default;

    [[maybe_unused]] explicit playlist(std::string name)
        : m_playlist_name(std::move(name)), m_tracks(){};

    playlist(playlist &&) noexcept = default;
    playlist &operator=(playlist &&) noexcept = default;

    playlist(const playlist &) = default;
    playlist &operator=(const playlist &) = default;

    ~playlist() {
        m_playlist_name = nullptr;
        m_tracks.clear();
    }
};
}  // namespace audio_app
#endif  // PLAYLIST_STRUCTURE_HPP
