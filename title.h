#ifndef TITLE_H
#define TITLE_H

#include <bits/stdc++.h>
#include "splashkit.h"

enum title_selection {
    START_GAME,
    HI_SCORES
};

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

title_data new_title(int level = 0);

title_data new_title(json settings);

bool handle_title_input(title_data &title);

void update_title(title_data &title);

void draw_title(const title_data &title);

#endif