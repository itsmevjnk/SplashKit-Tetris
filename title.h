#ifndef TITLE_H
#define TITLE_H

#include <bits/stdc++.h>
#include "splashkit.h"

typedef struct {
    uint64_t frame_num;

    font header_font;
    font menu_font;
} title_data;

title_data new_title();

bool handle_title_input(title_data &title);

void update_title(title_data &title);

void draw_title(const title_data &title);

#endif