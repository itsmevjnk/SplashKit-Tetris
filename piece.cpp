#include "piece.h"
#include "config.h"

using namespace std;

/* draw a cell, given its colour and position, and (optionally) whether the position is absolute */
void draw_cell(piece_colour p_color, piece_position position, bool absolute) {
    /* resolve piece colour */
    color draw_color;
    switch(p_color) {
        case CYAN:
            draw_color = PIECE_COLOR_CYAN;
            break;
        case BLUE:
            draw_color = PIECE_COLOR_BLUE;
            break;
        case ORANGE:
            draw_color = PIECE_COLOR_ORANGE;
            break;
        case YELLOW:
            draw_color = PIECE_COLOR_YELLOW;
            break;
        case GREEN:
            draw_color = PIECE_COLOR_GREEN;
            break;
        case PURPLE:
            draw_color = PIECE_COLOR_PURPLE;
            break;
        case RED:
            draw_color = PIECE_COLOR_RED;
            break;
    }

    /* resolve actual drawing position */
    int x, y;
    if(absolute) {
        x = position.x;
        y = position.y;
    } else {
        x = FIELD_X + FIELD_BORDER_WIDTH + position.x * (PIECE_SIZE + 2 * PIECE_MARGIN) + PIECE_MARGIN;
        y = FIELD_Y + FIELD_BORDER_WIDTH + position.y * (PIECE_SIZE + 2 * PIECE_MARGIN) + PIECE_MARGIN;
    }

    // write_line("Colour: " + color_to_string(draw_color) + ", x = " + to_string(x) + ", y = " + to_string(y));

    /* draw the cell itself */
    fill_rectangle(draw_color, x, y, PIECE_SIZE, PIECE_SIZE);
    draw_rectangle(PIECE_BORDER_COLOR, x + PIECE_BORDER_WIDTH, y + PIECE_PADDING, PIECE_SIZE - 2 * PIECE_PADDING, PIECE_SIZE - 2 * PIECE_PADDING, option_line_width(PIECE_BORDER_WIDTH));
}

/* draw a piece */
void draw_piece(piece p) {
    // write_line("Drawing piece " + to_string(p.type.p_color) + " @ x = " + to_string(p.position.x) + ", y = " + to_string(p.position.y) + ": " + dec_to_hex(p.type.bitmaps[p.rotation]));
    for(int y = 0; y < 4 && p.position.y + y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < 4 && p.position.x + x < FIELD_WIDTH; x++) {
            if(p.type.bitmaps[p.rotation] & (1 << (y * 4 + x)))
                draw_cell(p.type.p_color, {(p.position.x + x), (p.position.y + y)});
        }
    }
}

/* draw a piece, with overriden XY coordinates (absolute by default) */
void draw_piece(piece p, piece_position position, bool absolute) {
    for(int y = 0; y < 4 && (absolute || (p.position.y + y < FIELD_HEIGHT)); y++) {
        for(int x = 0; x < 4 && (absolute || (p.position.x + x < FIELD_WIDTH)); x++) {
            if(p.type.bitmaps[p.rotation] & (1 << (y * 4 + x))) {
                if(absolute)
                    draw_cell(p.type.p_color, {(position.x + x * (PIECE_SIZE + 2 * PIECE_MARGIN)), (position.y + y * (PIECE_SIZE + 2 * PIECE_MARGIN))}, true);
                else
                    draw_cell(p.type.p_color, {(position.x + x), (position.y + y)});
            }
        }
    }
}

