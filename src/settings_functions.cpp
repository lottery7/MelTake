#include <include/fwd.hpp>
#include "include/settings_structure.hpp"

using namespace audio_app;

void settings::set_theme(int type) {
    theme_type = type;
}

void settings::set_visualisation(int type) {
    visualisation_type = type;
}

void settings::back_to_default() {
    theme_type = 0;
    visualisation_type = 0;
}
