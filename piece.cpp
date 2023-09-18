#include "piece.h"
#include "config.h"

using namespace std;

#define FIELD_DRAW_X            (FIELD_X + FIELD_BORDER_WIDTH)
#define FIELD_DRAW_Y            (FIELD_Y + FIELD_BORDER_WIDTH)

/* generate a new random piece */
piece new_piece() {
    piece result;
    
    result.type = piece_types[rnd(6)]; // see piece_types.cpp
    result.rotation = rnd(3); // there are 4 possible rotated variants for each piece, also see piece_types.cpp
    
    /* set piece's Y coordinate depending on its bottom border - we want to keep the bottommost row off the canvas*/
    result.position.y = -4;
    if(PIECE_ROW(result.type.bitmaps[result.rotation], 3) == 0) {
        /* last row is not blank, so we need to dig further up */
        for(int y = 3; y > 0; y--) {
            if(PIECE_ROW(result.type.bitmaps[result.rotation], y) == 0 && PIECE_ROW(result.type.bitmaps[result.rotation], y - 1) != 0) {
                result.position.y = -y;
                break;
            }
        }
    }

    /* find piece's X coordinate bounds */
    int x_lower = 0, x_upper = FIELD_WIDTH - 4;
    bool find_lower = (PIECE_COL(result.type.bitmaps[result.rotation], 0) == 0); // if the first column has a cell, we will just use the default value above
    bool find_upper = (PIECE_COL(result.type.bitmaps[result.rotation], 3) == 0); // if the last column has a cell, we will just use the default value above
    for(int x = 0; x < 3 && (find_lower || find_upper); x++) {
        if(find_lower) {
            /* find lower bound */
            if(PIECE_COL(result.type.bitmaps[result.rotation], x) == 0 && PIECE_COL(result.type.bitmaps[result.rotation], x + 1) != 0) {
                x_lower = -(x + 1);
                find_lower = false;
            }
        }
        
        if(find_upper) {
            /* find upper bound */
            if(PIECE_COL(result.type.bitmaps[result.rotation], x) != 0 && PIECE_COL(result.type.bitmaps[result.rotation], x + 1) == 0) {
                x_upper = FIELD_WIDTH - (x + 1);
                find_upper = false;
            }
        }
    }
    result.position.x = rnd(x_lower, x_upper); // finally set X coordinate

    return result;
}

/* generate one or more pieces and append them to the pieces double-ended queue (deque), ensuring that each piece's type are unique */
void new_pieces(deque<piece> &pieces, int n) {
    do {
        piece result = new_piece(); // generate a piece

        /* check for duplication */
        bool duplicate = false;
        for(int i = 0; i < pieces.size(); i++) {
            if(pieces[i].type.p_color == result.type.p_color) {
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
    // write_line("Drawing piece " + to_string(p.type.p_color) + " @ x = " + to_string(p.position.x) + ", y = " + to_string(p.position.y) + ": " + dec_to_hex(p.type.bitmaps[p.rotation]));
    for(int y = 0; y < 4 && p.position.y + y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < 4 && p.position.x + x < FIELD_WIDTH; x++) {
            if(p.type.bitmaps[p.rotation] & (1 << (y * 4 + x)))
                draw_cell(p.type.p_color, {(p.position.x + x), (p.position.y + y)});
        }
    }
}

/* draw a piece, with overriden XY coordinates (absolute by default), and optionally use the XY coordinates as the start of the actual cell (tight drawing) */
void draw_piece(const piece &p, const piece_position &position, bool absolute, bool tight) {
    int cell_y = position.y;
    int dy = (absolute) ? PIECE_TOTAL_SIZE : 1; // cell_y increment step

    for(int y = 0; y < 4 && (absolute || tight || (cell_y < FIELD_HEIGHT)); y++) {
        int cell_x = position.x;

        bool empty_row = (PIECE_ROW(p.type.bitmaps[p.rotation], y) == 0);

        if(!tight || !empty_row) {
            /* only draw this row if either tight drawing is not enabled or the row is not empty */
            for(int x = 0; x < 4 && (absolute || tight || (cell_x < FIELD_WIDTH)); x++) {
                bool empty_col = (PIECE_COL(p.type.bitmaps[p.rotation], x) == 0); // only needed for tight drawing

                if(p.type.bitmaps[p.rotation] & (1 << (y * 4 + x))) {
                    draw_cell(p.type.p_color, {cell_x, cell_y}, absolute);
                }

                if(!empty_col) cell_x += (absolute) ? PIECE_TOTAL_SIZE : 1;
            }

            cell_y += dy;
        }
    }
}

/* get a piece's width */
int piece_width(const piece &p) {
    bool in_piece = false; // set when we have entered a piece
    int result = 0;

    for(int x = 0; x < 4; x++) {
        if(PIECE_COL(p.type.bitmaps[p.rotation], x) != 0) {
            /* there is a cell in here */
            if(!in_piece) in_piece = true;
            result++;
        } else {
            /* there are no cells - if in_piece is true, it means we are no longer in the piece */
            if(in_piece) break; // just exit here
        }
    }

    return result;
}

/* get a piece's height */
int piece_height(const piece &p) {
    bool in_piece = false; // set when we have entered a piece
    int result = 0;

    for(int y = 0; y < 4; y++) {
        if(PIECE_ROW(p.type.bitmaps[p.rotation], y) != 0) {
            /* there is a cell in here */
            if(!in_piece) in_piece = true;
            result++;
        } else {
            /* there are no cells - if in_piece is true, it means we are no longer in the piece */
            if(in_piece) break; // just exit here
        }
    }

    return result;
}

/* FOR DEBUGGING - get a descriptive string of a piece */
string piece_to_string(const piece &p) {
    string type = "Unknown";
    switch(p.type.p_color) {
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