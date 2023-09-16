#ifndef GAME_H
#define GAME_H

#include "piece.h"
#include "config.h"

struct game_data {
    int score;
    int level;
    piece_colour playing_field[FIELD_HEIGHT][FIELD_WIDTH];
    piece falling_piece;
    piece next_pieces[3];
    uint64_t frame_num;
    uint64_t frame_last_update;
    uint64_t frame_last_input;
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