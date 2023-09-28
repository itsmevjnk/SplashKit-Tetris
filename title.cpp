#include "title.h"
#include "config.h"

/* create new title data structure */
title_data new_title() {
    title_data result;

    result.frame_num = 0;
    result.header_font = font_named("GameFont");
    result.menu_font = font_named("GameFont");

    return result;
}

/* handle title input */
bool handle_title_input(title_data &title) {
    return false; // game not starting yet
}

/* update title data structure */
void update_title(title_data &title) {
    title.frame_num++;
}

#define TITLE_HEADER_GROW_FRAMES                    (FRAME_RATE * TITLE_HEADER_GROW_TIME)
#define TITLE_HEADER_COLOR_SHIFT_FRAMES             (FRAME_RATE * TITLE_HEADER_COLOR_SHIFT_PERIOD)

/* draw title header */
void draw_header(const title_data &title) {
    int font_size = (title.frame_num < TITLE_HEADER_GROW_FRAMES) ? (TITLE_HEADER_SIZE_INIT + (TITLE_HEADER_SIZE_FINAL - TITLE_HEADER_SIZE_INIT) * title.frame_num / TITLE_HEADER_GROW_FRAMES) : TITLE_HEADER_SIZE_FINAL; // get font size
    
    /* get the actual width and height of the header for positioning */
    int width = text_width("TETRIS", title.header_font, font_size);
    int height = text_height("TETRIS", title.header_font, font_size);

    /* draw the actual header */
    draw_text("TETRIS", hsb_color((double)(title.frame_num % TITLE_HEADER_COLOR_SHIFT_FRAMES) / TITLE_HEADER_COLOR_SHIFT_FRAMES, 1, 1), title.header_font, font_size, TITLE_HEADER_CENTER_X - width / 2, TITLE_HEADER_CENTER_Y - height / 2);
}

/* draw title screen */
void draw_title(const title_data &title) {
    clear_screen(TITLE_BG_COLOR);
    draw_header(title);
}

