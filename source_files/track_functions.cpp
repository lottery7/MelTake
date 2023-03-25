#include "include/fwd.hpp"

using namespace audio_app;

[[maybe_unused]] void track::edit_track(track &track_) {
    std::cout << "enter a new name for the track " << track_.m_track_name
              << ":\n";
    std::string new_track_name;
    std::cin >> new_track_name;
    track_.m_track_name = new_track_name;
}

[[maybe_unused]] void track::play_track(const track &track_) {
    // play track
}

[[maybe_unused]] void track::stop_track(const track &track_) {
    // pause track
}

//
// bool operator==(const track & lhs, const track &rhs) {
//    return lhs.m_track_name==rhs.m_track_name;
//}
