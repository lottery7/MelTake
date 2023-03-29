#ifndef TRACK_STRUCTURE_HPP
#define TRACK_STRUCTURE_HPP

#include <iostream>

namespace audio_app {

struct track {
    std::string track_name;

    track() : track_name() {
    }

    track(track &&another) noexcept = default;

    track &operator=(track &&another) noexcept = default;

    track(const track &another) = default;

    track &operator=(const track &) = delete;

    ~track() = default;

    void play_track(const track &track_);

    void stop_track(const track &track_);

    void edit_track(track &track_);
};

}  // namespace audio_app
#endif  // TRACK_STRUCTURE_HPP
