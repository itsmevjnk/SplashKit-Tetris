#ifndef PIECE_H
#define PIECE_H

#include "splashkit.h"
#include <deque>

using namespace std;

#define PIECE_ROW(bitmap, row)  ((bitmap >> ((row) * 4)) & 0x0F) // macro to extract the bitmap of a single row
#define PIECE_COL(bitmap, col)  ( ((PIECE_ROW(bitmap, 0) & ((1 << (col)))) ? (1 << 0) : 0) \
                                | ((PIECE_ROW(bitmap, 1) & ((1 << (col)))) ? (1 << 1) : 0) \
                                | ((PIECE_ROW(bitmap, 2) & ((1 << (col)))) ? (1 << 2) : 0) \
                                | ((PIECE_ROW(bitmap, 3) & ((1 << (col)))) ? (1 << 3) : 0)) // macro to extract the bitmap of a single column
                                
enum piece_colour {
    NO_COLOUR,  // no colour (i.e. black)
    CYAN,       // I piece
    BLUE,       // J piece
    ORANGE,     // L piece
    YELLOW,     // O piece
    GREEN,      // S piece
    PURPLE,     // T piece
    RED         // Z piece
};

struct piece_bitmap {
    uint16_t bitmap;
    uint8_t x, y; // coordinates of the top-left corner
    uint8_t width, height;
};

struct piece_type {
    piece_colour p_color;
    piece_bitmap bitmaps[4];
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

piece new_piece();
void new_pieces(deque<piece> &pieces, int n);
deque<piece> new_pieces(int n);
void draw_cell(piece_colour color, const piece_position &position, bool absolute = false);
void draw_piece(const piece &p);
void draw_piece(const piece &p, const piece_position &position, bool absolute = true, bool tight = false);
string piece_to_string(const piece &p);

#endif