#ifndef PIECE_H
#define PIECE_H

#include "splashkit.h"
#include <deque>

using namespace std;

/**
 * @brief Macro to extract a single row from a piece bitmap.
 * 
 */
#define PIECE_ROW(bitmap, row)  ((bitmap >> ((row) * 4)) & 0x0F)

/**
 * @brief Macro to extract a single column from a piece bitmap.
 * 
 */
#define PIECE_COL(bitmap, col)  ( ((PIECE_ROW(bitmap, 0) & ((1 << (col)))) ? (1 << 0) : 0) \
                                | ((PIECE_ROW(bitmap, 1) & ((1 << (col)))) ? (1 << 1) : 0) \
                                | ((PIECE_ROW(bitmap, 2) & ((1 << (col)))) ? (1 << 2) : 0) \
                                | ((PIECE_ROW(bitmap, 3) & ((1 << (col)))) ? (1 << 3) : 0))

/**
 * @brief Enumeration of piece colours, indicative of piece type.
 * 
 */
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

/**
 * @brief Bitmap information for a piece's rotation.
 * 
 * @field bitmap The piece's bitmap (4x4), also see PIECE_ROW() and PIECE_COL().
 * @field x The piece's top left corner X coordinate within the bitmap.
 * @field y The piece's top left corner Y coordinate within the bitmap.
 * @field width The piece's width.
 * @field height The piece's height.
 * 
 */
struct piece_bitmap {
    uint16_t bitmap;
    uint8_t x, y;
    uint8_t width, height;
};

/**
 * @brief Piece type information.
 * 
 * @field p_color The piece's colour.
 * @field bitmaps Array of bitmaps for each rotated variant of the piece.
 * 
 */
struct piece_type {
    piece_colour p_color;
    piece_bitmap bitmaps[4];
};

/**
 * @brief Piece/cell position structure.
 * 
 * @field x The X coordinate.
 * @field y The Y coordinate.
 * 
 */
struct piece_position {
    int x, y;
};

/**
 * @brief Piece information.
 * 
 * @field type Pointer to the piece's type information structure.
 * @field rotation The piece's rotated variant index (0 to 3).
 * @field position The piece bitmap's position in the playing field.
 * 
 */
struct piece {
    const piece_type *type;
    int rotation;
    piece_position position;
};

/**
 * @brief Array of piece types.
 * 
 */
extern const piece_type piece_types[7]; // NOTE: extern is used so that g++ wouldn't complain

/**
 * @brief Position (or reposition) a piece to the top of the playing field.
 * 
 * @param p The piece to be (re)positioned.
 */
void position_piece(piece &p);

/**
 * @brief Generate a random piece and place it right above the playing field for descent.
 * 
 * @return piece The resulting piece.
 */
piece new_piece();

/**
 * @brief Generate one or more new pieces and add them to a double-ended queue (deque), ensuring that all pieces in the queue are unique.
 * 
 * @param pieces The pieces queue to operate on.
 * @param n The number of new pieces to add.
 */
void new_pieces(deque<piece> &pieces, int n);

/**
 * @brief Generate a double-ended queue (deque) of new pieces.
 * 
 * @param n The number of new pieces.
 * @return deque<piece> The resulting queue.
 */
deque<piece> new_pieces(int n);

/**
 * @brief Draw a cell on the game window.
 * 
 * @param color The cell's colour.
 * @param position The cell's position within the playing field, or on the screen (in pixels); this is dictated by the absolute parameter.
 * @param absolute Drawing mode (absolute or relative); when this is set, the function will treat position as the cell's screen position in pixels.
 */
void draw_cell(piece_colour color, const piece_position &position, bool absolute = false);

/**
 * @brief Draw a piece on the screen using its internally-stored position on the playing field.
 * 
 * @param p The piece to be drawn.
 */
void draw_piece(const piece &p);

/**
 * @brief Draw a piece on the screen using an externally provided (optionally absolute; see draw_cell()) position, and optionally draw it tightly.
 * 
 * @param p The piece to be drawn.
 * @param position The piece's position within the playing field or on the screen (in pixels). This is dictated by the absolute parameter.
 * @param absolute Drawing mode (absolute or relative); when this is set, the function will treat position as the piece's screen position in pixels.
 * @param tight Tight drawing mode; when this is set, the position will be treated as the position of the actual start of the piece and not the start of the piece's bitmap.
 */
void draw_piece(const piece &p, const piece_position &position, bool absolute = true, bool tight = false);

/**
 * @brief Calculate the centre point coordinates of a given piece.
 * 
 * @param p The input piece.
 * @param offset Whether to only calculate the centre point offset (as opposed to the absolute position within the playing field). Disabled by default.
 * @return piece_position The piece's centre position.
 */
piece_position piece_centre_point(const piece &p, bool offset = false);

/**
 * @brief Generate a descriptive string of a piece. Used for debugging.
 * 
 * @param p The piece.
 * @return string The descriptive string of p.
 */
string piece_to_string(const piece &p);

#endif