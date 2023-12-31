#include "piece.h"
#include "config.h"

using namespace std;

/**
 * @brief The playing field content area's top left corner X coordinate.
 * 
 */
#define FIELD_DRAW_X            (FIELD_X + FIELD_BORDER_WIDTH)

/**
 * @brief The playing field content area's top left corner Y coordinate.
 * 
 */
#define FIELD_DRAW_Y            (FIELD_Y + FIELD_BORDER_WIDTH)

/* (re)position a piece */
void position_piece(piece &p) {
    p.position.y = -(p.type->bitmaps[p.rotation].height + p.type->bitmaps[p.rotation].y);
    p.position.x = rnd(-p.type->bitmaps[p.rotation].x, FIELD_WIDTH - (p.type->bitmaps[p.rotation].x + p.type->bitmaps[p.rotation].width));
}

/* generate a new random piece */
piece new_piece() {
    piece result;
    
    result.type = &piece_types[rnd(6)]; // see piece_types.cpp
    result.rotation = rnd(3); // there are 4 possible rotated variants for each piece, also see piece_types.cpp
    
    position_piece(result);

    return result;
}

/* generate one or more pieces and append them to the pieces double-ended queue (deque), ensuring that each piece's type are unique */
void new_pieces(deque<piece> &pieces, int n) {
    do {
        piece result = new_piece(); // generate a piece

        /* check for duplication */
        bool duplicate = false;
        for(int i = 0; i < pieces.size(); i++) {
            if(pieces[i].type->p_color == result.type->p_color) {
                duplicate = true;
                break;
            }
        }

        if(!duplicate) {
            /* append to vector if it's not unique */
            pieces.push_back(result);
            n--;
        }
    } while(n > 0);
}

deque<piece> new_pieces(int n) {
    deque<piece> result;
    new_pieces(result, n);
    return result;
}

/* draw a cell, given its colour and position, and (optionally) whether the position is absolute */
void draw_cell(piece_colour p_color, const piece_position &position, bool absolute) {
    /* resolve piece colour */
    color draw_color;
    switch(p_color) {
        case NO_COLOUR: return; // nothing to be drawn
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
        if(position.x < 0 || position.y < 0) return; // do not draw out of bound
        x = FIELD_DRAW_X + FIELD_BORDER_WIDTH + position.x * PIECE_TOTAL_SIZE;
        y = FIELD_DRAW_Y + FIELD_BORDER_WIDTH + position.y * PIECE_TOTAL_SIZE;
    }

    // write_line("Colour: " + color_to_string(draw_color) + ", x = " + to_string(x) + ", y = " + to_string(y));

    /* draw the cell itself */
    fill_rectangle(draw_color, x, y, PIECE_SIZE, PIECE_SIZE);
    draw_rectangle(PIECE_BORDER_COLOR, x + PIECE_PADDING, y + PIECE_PADDING, PIECE_SIZE - 2 * PIECE_PADDING, PIECE_SIZE - 2 * PIECE_PADDING, option_line_width(PIECE_BORDER_WIDTH));
}

/* draw a piece */
void draw_piece(const piece &p) {
    for(int y = 0; y < 4 && p.position.y + y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < 4 && p.position.x + x < FIELD_WIDTH; x++) {
            if(p.type->bitmaps[p.rotation].bitmap & (1 << (y * 4 + x)))
                draw_cell(p.type->p_color, {(p.position.x + x), (p.position.y + y)});
        }
    }
}

/* draw a piece, with overriden XY coordinates (absolute by default), and optionally use the XY coordinates as the start of the actual cell (tight drawing) */
void draw_piece(const piece &p, const piece_position &position, bool absolute, bool tight) {
    int cell_y = position.y;

    int d = (absolute) ? PIECE_TOTAL_SIZE : 1; // cell_x/y increment step

    int y0 = (tight) ? p.type->bitmaps[p.rotation].y : 0;
    for(int y = y0; y - y0 < ((tight) ? p.type->bitmaps[p.rotation].height : 4) && (absolute || (cell_y < FIELD_HEIGHT)); y++, cell_y += d) {
        int cell_x = position.x;

        int x0 = (tight) ? p.type->bitmaps[p.rotation].x : 0;
        for(int x = x0; x - x0 < ((tight) ? p.type->bitmaps[p.rotation].width : 4) && (absolute || (cell_x < FIELD_WIDTH)); x++, cell_x += d) {
            if(p.type->bitmaps[p.rotation].bitmap & (1 << (y * 4 + x))) {
                draw_cell(p.type->p_color, {cell_x, cell_y}, absolute);
            }
        }
    }
}

/* calculate centre point of a piece */
piece_position piece_centre_point(const piece &p, bool offset) {
    piece_position result;

    result.x = ((offset) ? 0 : p.position.x) + p.type->bitmaps[p.rotation].x + p.type->bitmaps[p.rotation].width / 2;
    result.y = ((offset) ? 0 : p.position.y) + p.type->bitmaps[p.rotation].y + p.type->bitmaps[p.rotation].height / 2;

    return result;
}

/* FOR DEBUGGING - get a descriptive string of a piece */
string piece_to_string(const piece &p) {
    string type = "Unknown";
    switch(p.type->p_color) {
        case CYAN: type = "I"; break;
        case BLUE: type = "J"; break;
        case ORANGE: type = "L"; break;
        case YELLOW: type = "O"; break;
        case GREEN: type = "S"; break;
        case PURPLE: type = "T"; break;
        case RED: type = "Z"; break;
        default: break;
    }
    return type + " piece (rotation " + to_string(p.rotation) + ") @ (" + to_string(p.position.x) + "," + to_string(p.position.y) + ")";
}