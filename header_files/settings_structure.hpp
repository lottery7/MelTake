#ifndef SETTINGS_STRUCTURE_HPP
#define SETTINGS_STRUCTURE_HPP

#include <iostream>

namespace audio_app {
struct settings {
    void back_to_default() {
        theme_type = 0;
        visualisation_type = 0;
    }

    int theme_type = 0;
    int visualisation_type = 0;

    settings() = default;
    settings(const settings &) = default;
    settings &operator=(const settings &) = default;
    settings(settings &&) = default;
    settings &operator=(settings &&) = default;

    ~settings() {
        back_to_default();
    };

    void set_theme(int type);
    void set_visualisation(int type);
};
}  // namespace audio_app
#endif  // SETTINGS_STRUCTURE_HPP
