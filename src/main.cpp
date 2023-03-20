#include "include/fwd.hpp"

int main() {
    // upload user's track_data from previous usage
    audio_app::music_base user_music_base;
    user_music_base.beginning_setup();
    user_music_base.upload_info_from_trackdata_file();
    // how the app structure looks
    user_music_base.app_view();
}
