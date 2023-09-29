#ifndef CONFIG_H
#define CONFIG_H

#include "splashkit.h"

/* DEBUG DEFINITIONS */

/**
 * @brief Macro directive to print instances of input rejections caused by collisions.
 * 
 */
// #define DEBUG_INPUT_REJECTIONS

/* COMMON GAME OPTIONS */

/**
 * @brief The game window's title.
 * 
 */
#define WINDOW_TITLE                    "Tetris"

/**
 * @brief The game window's width (in pixels).
 * 
 */
#define WINDOW_WIDTH                    640

/**
 * @brief The game window's height (in pixels).
 * 
 */
#define WINDOW_HEIGHT                   480

/**
 * @brief The title screen's background colour. 
 * 
 */
#define TITLE_BG_COLOR                  COLOR_BLACK

/**
 * @brief The game screen's background colour.
 * 
 */
#define GAME_BG_COLOR                   COLOR_BLUE

/**
 * @brief The game's frame rate (in frames per second).
 * 
 */
#define FRAME_RATE                      60

/**
 * @brief The number of pieces in the next pieces queue, including the falling piece.
 * 
 */
#define NEXT_PIECES_CNT                 4

/* FIELD OPTIONS */

/**
 * @brief The X coordinate of the playing field's top left border corner.
 * 
 */
#define FIELD_X                         (WINDOW_WIDTH / 4 - FIELD_WIDTH_PX / 2)

/**
 * @brief The Y coordinate of the playing field's top left border corner.
 * 
 */
#define FIELD_Y                         ((WINDOW_HEIGHT - FIELD_HEIGHT_PX) / 2)

/**
 * @brief The field's border width (in pixels).
 * 
 */
#define FIELD_BORDER_WIDTH              1

/**
 * @brief The field's border colour.
 * 
 */
#define FIELD_BORDER_COLOR              COLOR_WHITE

/**
 * @brief The field's background colour.
 * 
 */
#define FIELD_BG_COLOR                  COLOR_BLACK

/**
 * @brief The playing field's width (in cells). Tetris guidelines call for 10 columns.
 * 
 */
#define FIELD_WIDTH                     10

/**
 * @brief The playing field's height (in cells). Tetris guidelines call for 20 columns.
 * 
 */
#define FIELD_HEIGHT                    20

/**
 * @brief The HUD's top left border corner X coordinate. Set to -1 to center the HUD to the right half.
 * 
 */
#define HUD_X                           (WINDOW_WIDTH / 2) // set to -1 for auto calculation (center to right half)

/**
 * @brief The HUD's top left border corner Y coordinate. Set to -1 to center the HUD to the top half (if possible) or the window center.
 * 
 */
#define HUD_Y                           -1 // set to -1 for auto calculation (center to top half if possible, else center)

/**
 * @brief The width of the HUD border (in pixels).
 * 
 */
#define HUD_BORDER_WIDTH                FIELD_BORDER_WIDTH

/**
 * @brief The colour of the HUD border.
 * 
 */
#define HUD_BORDER_COLOR                FIELD_BORDER_COLOR

/**
 * @brief The HUD background colour.
 * 
 */
#define HUD_BG_COLOR                    FIELD_BG_COLOR

/**
 * @brief The HUD text height (in pixels).
 * 
 */
#define HUD_TEXT_SIZE                   24 // text height in pixels

/**
 * @brief The HUD text colour.
 * 
 */
#define HUD_TEXT_COLOR                  COLOR_WHITE

/**
 * @brief The number of characters used for the score in the HUD. The score will be zero padded to match this number and aligned with the level field.
 * 
 */
#define HUD_SCORE_WIDTH                 7

/**
 * @brief The number of characters used for the level in the HUD. The level will be space padded to match this number and aligned with the score field.
 * 
 */
#define HUD_LEVEL_WIDTH                 2

/**
 * @brief The HUD content padding width (in pixels).
 * 
 */
#define HUD_PADDING                     4

/**
 * @brief The HUD content width (in pixels). Set to 0 to automatically calculate.
 * 
 */
#define HUD_WIDTH                       0

/**
 * @brief The HUD content height (in pixels). Set to 0 to automatically calculate.
 * 
 */
#define HUD_HEIGHT                      0

/* PIECE OPTIONS */

/**
 * @brief Cell size (in pixels), excluding the margin.
 * 
 */
#define PIECE_SIZE                      20

/**
 * @brief Cell margin width (in pixels).
 * 
 */
#define PIECE_MARGIN                    1

/**
 * @brief Cell padding width (in pixels), i.e. the space between the piece and the inner border.
 * 
 */
#define PIECE_PADDING                   2

/**
 * @brief Cell border width (in pixels).
 * 
 */
#define PIECE_BORDER_WIDTH              1

/**
 * @brief Cell border colour.
 * 
 */
#define PIECE_BORDER_COLOR              COLOR_BLACK

/**
 * @brief Colour for the cyan (I) block.
 * 
 */
#define PIECE_COLOR_CYAN                COLOR_CYAN

/**
 * @brief Colour for the blue (J) block.
 * 
 */
#define PIECE_COLOR_BLUE                COLOR_BLUE

/**
 * @brief Colour for the orange (L) block.
 * 
 */
#define PIECE_COLOR_ORANGE              COLOR_ORANGE

/**
 * @brief Colour for the yellow (O) block.
 * 
 */
#define PIECE_COLOR_YELLOW              COLOR_YELLOW

/**
 * @brief Colour for the green (S) block.
 * 
 */
#define PIECE_COLOR_GREEN               COLOR_GREEN

/**
 * @brief Colour for the purple (T) block.
 * 
 */
#define PIECE_COLOR_PURPLE              COLOR_PURPLE

/**
 * @brief Colour for the red (Z) block.
 * 
 */
#define PIECE_COLOR_RED                 COLOR_RED

/* POINTS */

/**
 * @brief Points added on each force down action.
 * 
 */
#define SCORE_FORCE_DOWN                1

/**
 * @brief Points added when a single row has been cleared.
 * 
 */
#define SCORE_CLEAR_1                   100

/**
 * @brief Points added when two contiguous rows have been cleared.
 * 
 */
#define SCORE_CLEAR_2_CONT              250

/**
 * @brief Points added when two non-contiguous rows have been cleared.
 * 
 */
#define SCORE_CLEAR_2_SPLIT             300

/**
 * @brief Points added when three contiguous rows have been cleared.
 * 
 */
#define SCORE_CLEAR_3_CONT              400

/**
 * @brief Points added when three non-contiguous rows have been cleared.
 * 
 */
#define SCORE_CLEAR_3_SPLIT             450

/**
 * @brief Points added when four rows have been cleared.
 * 
 */
#define SCORE_CLEAR_4                   750

/**
 * @brief Number of points needed for each level-up.
 * 
 */
#define SCORE_LEVEL_UP                  1000

/* SPEED */

/**
 * @brief The base falling speed (in updates per second).
 * 
 */
#define SPEED_BASE                      1

/**
 * @brief The falling speed increment for each level (in updates per second).
 * 
 */
#define SPEED_STEP                      0.25

/**
 * @brief Side moving speed (in moves per second).
 * 
 */
#define SPEED_INPUT_MOVE                10

/**
 * @brief Down forcing speed (in moves per second).
 * 
 */
#define SPEED_INPUT_FORCE_DOWN          20

/**
 * @brief Rotation speed (in rotations per second).
 * 
 */
#define SPEED_INPUT_ROTATE              4

/**
 * @brief Piece swapping speed (in swaps per second).
 * 
 */
#define SPEED_INPUT_SWAP                4

/* GAME OVER */

/**
 * @brief Playing field filling speed (in rows per second).
 * 
 */
#define GAME_OVER_FILL_RATE             10

/**
 * @brief The colour of cells used to fill the playing field on game over (from the piece_colour enum, see piece.h).
 * 
 */
#define GAME_OVER_FILL_COLOR            RED

/**
 * @brief Macro directive to fill the playing field from the bottom; can be commented to fill the playing field from the top.
 * 
 */
// #define GAME_OVER_FILL_FROM_BOTTOM // comment this to fill from top

/**
 * @brief The text size used for the game over screen (in pixels).
 * 
 */
#define GAME_OVER_TEXT_SIZE             18

/* DERIVED VALUES */

/**
 * @brief The total width of the playing field's content (in pixels).
 * 
 */
#define FIELD_WIDTH_PX                  (FIELD_WIDTH * (PIECE_SIZE + 2 * PIECE_MARGIN))

/**
 * @brief The total height of the playing field's content (in pixels).
 * 
 */
#define FIELD_HEIGHT_PX                 (FIELD_HEIGHT * (PIECE_SIZE + 2 * PIECE_MARGIN))

/**
 * @brief The total size of a cell, including its margins (in pixels).
 * 
 */
#define PIECE_TOTAL_SIZE                (PIECE_SIZE + 2 * PIECE_MARGIN)

/* TITLE SCREEN CONFIGURATION */

#define TITLE_HEADER_CENTER_X               (WINDOW_WIDTH / 2)

#define TITLE_HEADER_CENTER_Y               (WINDOW_HEIGHT / 4)

#define TITLE_MENU_CENTER_X                 (WINDOW_WIDTH / 2)

#define TITLE_MENU_CENTER_Y                 (3 * WINDOW_HEIGHT / 4)

#define TITLE_HEADER_SIZE_INIT              (WINDOW_HEIGHT / 16)

#define TITLE_HEADER_SIZE_FINAL             (WINDOW_HEIGHT / 4)

#define TITLE_HEADER_GROW_TIME              3

#define TITLE_HEADER_COLOR_SHIFT_PERIOD     7

#define TITLE_HEADER_SWITCH_PERIOD          1.5

#define TITLE_HEADER_SWITCH_SCROLL_TIME     0.5

#define TITLE_MENU_TEXT_SIZE                32

#define TITLE_MENU_COLOR                    COLOR_WHITE

#endif
