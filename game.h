#ifndef GAME_H
#define GAME_H

#include "piece.h"
#include "config.h"

struct game_data {
    int score;
    int level;
    color playing_field[FIELD_HEIGHT][FIELD_WIDTH];
    piece falling_piece;
    piece next_pieces[3];
    uint64_t frame_num;
};

#endif