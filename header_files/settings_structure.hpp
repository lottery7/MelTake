#ifndef TRACK_STRUCTURE_HPP
#define TRACK_STRUCTURE_HPP

#include <iostream>
#include <utility>

namespace audio_app {

struct track {
    std::string m_track_name;
    std::string m_track_path;

    track() : m_track_name() {
    }

    explicit track(std::string track_name, std::string track_path)
        : m_track_name(std::move(track_name)),
          m_track_path(std::move(track_path)) {
    }

    track(track &&another) noexcept = default;

    track &operator=(track &&another) noexcept = default;

    track(const track &another) = default;

    track &operator=(const track &) = delete;

    ~track() = default;

    //    friend bool operator==(const track &lhs, const track &rhs);

    void play_track(const track &track_);

    void stop_track(const track &track_);

    void edit_track(track &track_);
};

}  // namespace audio_app
#endif  // TRACK_STRUCTURE_HPP
