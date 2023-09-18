#ifndef GAME_H
#define GAME_H

#include "splashkit.h"

#include "piece.h"
#include "config.h"
#include <deque>

using namespace std;

enum game_input_action {
    NO_INPUT,
    MOVE_LEFT,
    MOVE_RIGHT,
    ROTATE,
    FORCE_DOWN
};

struct hud_drawing_options {
    int start_x;
    int start_y;

    int content_width;
    int content_height;

    int char_width;
    int char_height;
    
    font hud_font;
};

struct game_data {
    int score;
    int level;
    piece_colour playing_field[FIELD_HEIGHT][FIELD_WIDTH];
    deque<piece> next_pieces;
    uint64_t frame_num;
    uint64_t frame_last_update;

    /* input */
    game_input_action last_input_action;
    uint64_t frame_last_input;

    /* HUD */
    hud_drawing_options hud_options;

};

struct removed_rows {
    bool flags[FIELD_HEIGHT];
    int count;
};

game_data new_game();
uint8_t check_collision(const game_data &game, const piece &test_piece);
uint8_t check_collision(const game_data &game);
void handle_input(game_data &game);
void draw_game(const game_data &game);
void update_game(game_data &game);

#endif