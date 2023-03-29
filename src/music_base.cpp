#include <iostream>
#include "include/fwd.hpp"

namespace audio_app {

void music_base::beginning_setup() noexcept {
    playlist default_playlist("default");
    playlist_list.push_back(default_playlist);
}

void music_base::assert_or_create_trackdata_file(
    const std::string &trackdata_file
) noexcept {
    setlocale(LC_ALL, "ru");
    std::ifstream file_read(trackdata_file);
    if (!file_read.is_open()) {
        std::ofstream created_file(trackdata_file);
    }
    /// checks if the user has the track_data file - if not, create it
}

playlist &music_base::find_playlist(const std::string &playlist_name) noexcept {
    int k = 0;
    bool is_playlist_found = false;
    for (int i = 0; i < static_cast<int>(playlist_list.size()); i++) {
        if (playlist_list[i].m_playlist_name == playlist_name) {
            k = i;
            is_playlist_found = true;
        }
    }
    if (k == 0 && !is_playlist_found) {
        playlist new_playlist(playlist_name);
        playlist_list.push_back(new_playlist);
        return playlist_list[playlist_list.size() - 1];
    } else {
        //        std::cout<<"we have this playlist! - "<<playlist_name<<"\n";
        return playlist_list[k];
    }
}

void music_base::upload_info_from_trackdata_file(
    const std::string &trackdata_file
) {
    setlocale(LC_ALL, "ru");
    assert_or_create_trackdata_file(trackdata_file);
    std::ifstream file_read(trackdata_file);
    std::string file_line;
    std::string file_track_path, file_track_name, file_track_playlist;
    if (file_read.is_open()) {
        while (std::getline(file_read, file_line)) {
            std::istringstream line_read(file_line);
            line_read >> file_track_path >> file_track_name >>
                file_track_playlist;
            track track_to_add(file_track_name, file_track_path);
            find_playlist(file_track_playlist)
                .add_old_track_to_playlist(track_to_add);
        }
        file_read.close();
    } else {
        throw unable_to_find_users_data();
    }
}

void music_base::app_view() noexcept {
    for (auto &playlist : playlist_list) {
        std::cout << "----------Playlist: " << playlist.m_playlist_name
                  << " ------------\nTracks:\n";
        for (auto &track : playlist.m_tracks) {
            std::cout << "[] " << track.m_track_name << " - from "
                      << track.m_track_path << "\n";
        }
        std::cout << "\n";
    }
    std::cout << "App is designed with " << settings_type.theme_type
              << " theme type and " << settings_type.visualisation_type
              << " visualisation type\n";
}

};  // namespace audio_app
