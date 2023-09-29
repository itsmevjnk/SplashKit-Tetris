#ifndef SETTINGS_H
#define SETTINGS_H

#include "splashkit.h"

json load_settings();

void save_settings(json settings);

void set_level(json settings, int level);

int get_level(json settings);

#endif