#ifndef SETTINGS_H
#define SETTINGS_H

#include "splashkit.h"

/**
 * @brief Load the settings JSON file.
 * 
 * @return json The parsed settings JSON struct.
 */
json load_settings();

/**
 * @brief Save settings to the JSON file.
 * 
 * @param settings The settings JSON struct.
 */
void save_settings(json settings);

/**
 * @brief Set the starting level in the settings JSON struct.
 * 
 * @param settings The settings JSON struct.
 * @param level The starting level.
 */
void set_level(json settings, int level);

/**
 * @brief Get the starting level in the settings JSON struct.
 * 
 * @param settings The settings JSON struct.
 * @return int The starting level.
 */
int get_level(json settings);

#endif