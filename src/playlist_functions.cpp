#include <include/fwd.hpp>
#include "include/playlist_structure.hpp"

using namespace audio_app;

void playlist::add_new_track_to_playlist(const track &track_name) {
    m_tracks.push_back(track_name);
    std::ofstream file_write(TRACK_DATA, std::ios_base::app);
    file_write << track_name.m_track_path << " " << track_name.m_track_name
               << " " << m_playlist_name << "\n";
    /// add to the end of track_data file that the user have this track in this
    /// playlist
}

void playlist::add_old_track_to_playlist(const track &track_name) {
    m_tracks.push_back(track_name);
}

void playlist::delete_track_from_playlist(track &track_name) {
    bool is_track_found = false;
    for (auto iter = m_tracks.begin(); iter != m_tracks.end(); ++iter) {
        if (iter->m_track_name == track_name.m_track_name) {
            m_tracks.erase(iter);
            is_track_found = true;
        }
    }
    if (!is_track_found) {
        throw unable_to_find_the_track_in_playlist();
    } else {
        std::fstream file(TRACK_DATA);
        std::string file_line;
        std::string new_version_file;
        std::string file_track_path, file_track_name, file_track_playlist;
        while (std::getline(file, file_line)) {
            std::istringstream line_read(file_line);
            line_read >> file_track_path >> file_track_name >>
                file_track_playlist;
            if (!(file_track_path == track_name.m_track_path &&
                  file_track_name == track_name.m_track_name &&
                  file_track_playlist == m_playlist_name)) {
                new_version_file.append(
                    file_track_path + " " + file_track_name + " " +
                    file_track_playlist + "\n"
                );
            }
        }
        file.close();
        std::ofstream updated_file(
            TRACK_DATA, std::ios::trunc | std::ios::binary
        );
        updated_file.write(
            new_version_file.c_str(), static_cast<long>(new_version_file.size())
        );
        updated_file.clear();
    }
    /// delete info about the track from track_data file and from the playlist
    /// itself
}
