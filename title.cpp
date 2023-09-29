#include "title.h"
#include "utils.h"
#include "config.h"
#include "settings.h"
#include "scoreboard.h"

/* create new title data structure */
title_data new_title(int level) {
    title_data result;

    result.selection = START_GAME;
    result.level = level;
    result.frame_num = 0; result.frame_last_lr = 0; result.frame_last_ud = 0;

    result.header_font = font_named("GameFont");
    result.menu_font = font_named("GameFont");

    result.scoreboard = load_scoreboard();

    /* calculate menu X offset */
    result.menu_xoff = text_width("\x10 ", result.menu_font, TITLE_MENU_TEXT_SIZE);

    /* calculate menu width */
    result.menu_width = text_width("START GAME", result.menu_font, TITLE_MENU_TEXT_SIZE);
    result.menu_width = MAX(result.menu_width, text_width("HIGH SCORES", result.menu_font, TITLE_MENU_TEXT_SIZE));
    result.menu_width = MAX(result.menu_width, text_width("LEVEL: \x11 " + string(HUD_SCORE_WIDTH, '0') + " \x10", result.menu_font, TITLE_MENU_TEXT_SIZE));
    result.menu_width += result.menu_xoff;

    /* calculate menu height */
    result.menu_height = 3 * text_height("0", result.menu_font, TITLE_MENU_TEXT_SIZE);

    return result;
}

title_data new_title(json settings) {
    return new_title(get_level(settings));
}

/* handle title input */
bool handle_title_input(title_data &title) {
    if(key_released(RETURN_KEY)) {
        switch(title.selection) {
            case START_GAME:
                free_database(title.scoreboard); // close scoreboard database
                return true; // start the game
            case HI_SCORES:
                title.show_scoreboard = !title.show_scoreboard;
                break; // TODO
        }
    }

    if(!title.show_scoreboard) { // block all input if showing scoreboard
        if(key_down(UP_KEY) && (title.frame_last_ud == 0 || title.frame_num - title.frame_last_ud >= FRAME_RATE / SPEED_INPUT_MENU)) {
            title.frame_last_ud = title.frame_num;
            if((int)title.selection > 0) title.selection = (title_selection)((int)title.selection - 1);
        }

        if(key_down(DOWN_KEY) && (title.frame_last_ud == 0 || title.frame_num - title.frame_last_ud >= FRAME_RATE / SPEED_INPUT_MENU)) {
            title.frame_last_ud = title.frame_num;
            if((int)title.selection < 1) title.selection = (title_selection)((int)title.selection + 1);
        }

        if(key_down(LEFT_KEY) && (title.frame_last_lr == 0 || title.frame_num - title.frame_last_lr >= FRAME_RATE / SPEED_INPUT_MENU)) {
            title.frame_last_lr = title.frame_num;
            if(title.level > 0) title.level--;
        }

        if(key_down(RIGHT_KEY) && (title.frame_last_lr == 0 || title.frame_num - title.frame_last_lr >= FRAME_RATE / SPEED_INPUT_MENU)) {
            title.frame_last_lr = title.frame_num;
            title.level++;
        }
    }

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

/* draw title menu */
void draw_menu(const title_data &title) {
    int char_height = title.menu_height / 3; // character height
    bitmap menu = create_bitmap("Menu", title.menu_width, title.menu_height); // menu bitmap for ease of drawing

    /* draw menu selections */
    draw_text_on_bitmap(menu, "START GAME", TITLE_MENU_COLOR, title.header_font, TITLE_MENU_TEXT_SIZE, title.menu_xoff, 0);
    draw_text_on_bitmap(menu, "HIGH SCORES", TITLE_MENU_COLOR, title.header_font, TITLE_MENU_TEXT_SIZE, title.menu_xoff, char_height);
    draw_text_on_bitmap(menu, string("LEVEL: ") + ((title.level > 0) ? string("\x11 ") : string("  ")) + int_to_string(title.level + 1, 2) + string(" \x10"), TITLE_MENU_COLOR, title.header_font, TITLE_MENU_TEXT_SIZE, title.menu_xoff, 2 * char_height);

    /* draw pointer */
    draw_text_on_bitmap(menu, "\x10", TITLE_MENU_COLOR, title.header_font, TITLE_MENU_TEXT_SIZE, 0, (int)title.selection * char_height);

    /* finally draw the bitmap to the window */
    draw_bitmap(menu, TITLE_MENU_CENTER_X - (title.menu_width - title.menu_xoff) / 2 + title.menu_xoff, TITLE_MENU_CENTER_Y - title.menu_height / 2);
    free_bitmap(menu);
}

/* draw title screen */
void draw_title(const title_data &title) {
    clear_screen(TITLE_BG_COLOR);
    draw_header(title);
    draw_menu(title);
    if(title.show_scoreboard) draw_scoreboard(title.scoreboard);
}

