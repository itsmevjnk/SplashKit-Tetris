#include "title.h"
#include "config.h"

using namespace std;

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

#define TITLE_HEADER_GROW_FRAMES                    (int)(FRAME_RATE * TITLE_HEADER_GROW_TIME)
#define TITLE_HEADER_COLOR_SHIFT_FRAMES             (int)(FRAME_RATE * TITLE_HEADER_COLOR_SHIFT_PERIOD)
#define TITLE_HEADER_SWITCH_FRAMES                  (int)(FRAME_RATE * TITLE_HEADER_SWITCH_PERIOD)
#define TITLE_HEADER_SWITCH_SCROLL_FRAMES           (int)(FRAME_RATE * TITLE_HEADER_SWITCH_SCROLL_TIME)

/* draw title header */
void draw_header(const title_data &title) {
    int font_size = (title.frame_num < TITLE_HEADER_GROW_FRAMES) ? (TITLE_HEADER_SIZE_INIT + (TITLE_HEADER_SIZE_FINAL - TITLE_HEADER_SIZE_INIT) * title.frame_num / TITLE_HEADER_GROW_FRAMES) : TITLE_HEADER_SIZE_FINAL; // get font size
    
    /* get the actual width and height of the header for positioning - assuming the text is strictly monospace, i.e. Russian text has the same size as English text */
    int width = text_width("TETRIS", title.header_font, font_size);
    int height = text_height("TETRIS", title.header_font, font_size);

    /* get header colour */
    color header_color = hsb_color((double)(title.frame_num % TITLE_HEADER_COLOR_SHIFT_FRAMES) / TITLE_HEADER_COLOR_SHIFT_FRAMES, 1, 1);

    /* calculate alpha for scrolling effect */
    int scroll_phase = title.frame_num % TITLE_HEADER_SWITCH_FRAMES;
    bool show_en = (((title.frame_num / TITLE_HEADER_SWITCH_FRAMES)) % 2 == 0); // set if the English text is to be shown
    int ystart_1 = -height, ystart_2 = 0; // starting Y position for top text and bottom text respectively
    if(scroll_phase >= TITLE_HEADER_SWITCH_FRAMES - TITLE_HEADER_SWITCH_SCROLL_FRAMES) {
        /* scrolling */
        ystart_1 = -height + height * (scroll_phase - (TITLE_HEADER_SWITCH_FRAMES - TITLE_HEADER_SWITCH_SCROLL_FRAMES)) / TITLE_HEADER_SWITCH_SCROLL_FRAMES;
        ystart_2 = ystart_1 + height;
    }
    // write_line(to_string(ystart_1) + " " + to_string(ystart_2));

    /* draw header onto bitmap first */
    bitmap header = create_bitmap("HeaderText", width, height);
    draw_text_on_bitmap(header, "TETRIS", header_color, title.header_font, font_size, 0, (show_en) ? ystart_2 : ystart_1);
    draw_text_on_bitmap(header, "ТЕТРИС", header_color, title.header_font, font_size, 0, (show_en) ? ystart_1 : ystart_2);

    
    /* draw the actual header */
    // draw_text((show_en) ? "TETRIS" : "ТЕТРИС", header_color, title.header_font, font_size, TITLE_HEADER_CENTER_X - width / 2, TITLE_HEADER_CENTER_Y - height / 2);
    draw_bitmap(header, TITLE_HEADER_CENTER_X - width / 2, TITLE_HEADER_CENTER_Y - height / 2);
    free_bitmap(header);
}

/* draw title screen */
void draw_title(const title_data &title) {
    clear_screen(TITLE_BG_COLOR);
    draw_header(title);
}

