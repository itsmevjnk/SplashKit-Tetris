#ifndef TITLE_H
#define TITLE_H

#include <bits/stdc++.h>
#include "splashkit.h"

/**
 * @brief Enumeration of available options in the menu.
 * 
 */
enum title_selection {
    START_GAME,
    HI_SCORES
};

/**
 * @brief The title screen data structure.
 * 
 * @field selection The current menu selection.
 * @field level The starting level.
 * 
 * @field frame_num The current frame number since the title data struct was initialised. Used for time-keeping.
 * @field frame_last_ud The frame number where the last accepted up/down input (for menu selection) occurred.
 * @field frame_last_lr The frame number where the last accepted left/right input (for level selection) occurred.
 * 
 * @field header_font The font used for the header. Must also support the Cyrillic script.
 * @field menu_font The font used for the menu.
 * 
 * @field menu_width The menu section's width (in pixels).
 * @field menu_height The menu section's height (in pixels).
 * @field menu_xoff The menu section's X offset from the centre X/Y coordinates specified in config.h (in pixels).
 * 
 * @field scoreboard The scoreboard database, used for displaying the scoreboard.
 * @field show_scoreboard Set when the scoreboard is requested by the player.
 * 
 */
struct title_data {
    title_selection selection;
    int level;

    uint64_t frame_num;
    uint64_t frame_last_ud;
    uint64_t frame_last_lr;

    font header_font;
    font menu_font;

    int menu_width;
    int menu_height;
    int menu_xoff;

    database scoreboard;
    bool show_scoreboard = false;
};

/**
 * @brief Create a new title data structure with the supplied starting level.
 * 
 * @param level The starting level (zero-based).
 * @return title_data The resulting title data structure.
 */
title_data new_title(int level = 0);

/**
 * @brief Create a new title data structure, with the starting level fetched from a settings JSON structure.
 * 
 * @param settings The settings JSON structure.
 * @return title_data The resulting title data structure.
 */
title_data new_title(json settings);

/**
 * @brief Handle input in the title screen.
 * 
 * @param title The title data structure.
 * @return true Returned when the game has not left the title screen.
 * @return false Returned when the game has left the title screen, and when it is time to start the actual game.
 */
bool handle_title_input(title_data &title);

/**
 * @brief Update the title data structure on every frame.
 * 
 * @param title The title data structure.
 */
void update_title(title_data &title);

/**
 * @brief Draw the title header.
 * 
 * @param title The title data structure.
 */
void draw_header(const title_data &title);

/**
 * @brief Draw the title menu.
 * 
 * @param title The title data structure.
 */
void draw_menu(const title_data &title);

/**
 * @brief Draw the title screen.
 * 
 * @param title The title data structure.
 */
void draw_title(const title_data &title);

#endif