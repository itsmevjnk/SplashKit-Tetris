#ifndef PIECE_H
#define PIECE_H

#include "splashkit.h"

enum piece_colour {
    CYAN,       // I piece
    BLUE,       // J piece
    ORANGE,     // L piece
    YELLOW,     // O piece
    GREEN,      // S piece
    PURPLE,     // T piece
    RED         // Z piece
};

struct piece_type {
    piece_colour p_color;
    uint16_t bitmaps[4];
};

struct piece_position {
    int x, y;
};

struct piece {
    piece_type type;
    int rotation;
    piece_position position;
};

extern const piece_type piece_types[7]; // NOTE: extern is used so that g++ wouldn't complain

void draw_cell(piece_colour color, piece_position position, bool absolute = false);
void draw_piece(piece p);
void draw_piece(piece p, piece_position position, bool absolute = true);

#endif