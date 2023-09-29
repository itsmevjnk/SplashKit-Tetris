#include "settings.h"

json load_settings() {
    return json_from_file("settings.json");
}

void save_settings(json settings) {
    json_to_file(settings, "settings.json");
}

void set_level(json settings, int level) {
    json_set_number(settings, "level", level);
}

int get_level(json settings) {
    if(!json_has_key(settings, "level")) set_level(settings, 0); // defaults to level 1
    return json_read_number_as_int(settings, "level");
}
