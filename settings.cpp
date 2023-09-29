#include "settings.h"
#include <sys/stat.h>

/* load settings.json */
json load_settings() {
    struct stat buffer;
    if(stat("Resources/json/settings.json", &buffer) != 0) return create_json(); // file does not exist, so we'll just return a blank JSON struct
    return json_from_file("settings.json");
}

/* save settings.json */
void save_settings(json settings) {
    json_to_file(settings, "settings.json");
}

/* set starting level */
void set_level(json settings, int level) {
    json_set_number(settings, "level", level);
}

/* get starting level */
int get_level(json settings) {
    if(!json_has_key(settings, "level")) set_level(settings, 0); // defaults to level 1
    return json_read_number_as_int(settings, "level");
}
