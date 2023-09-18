#ifndef CONFIG_H
#define CONFIG_H

#include "splashkit.h"

/* debug definitions */
// #define DEBUG_INPUT_REJECTIONS // print instances of input (move/rotate) rejections

/* game parameters */

#define WINDOW_TITLE                    "Tetris"
#define WINDOW_WIDTH                    640
#define WINDOW_HEIGHT                   480
#define WINDOW_BG_COLOR                 COLOR_BLUE
#define FRAME_RATE                      60
#define NEXT_PIECES_CNT                 4 // number of pieces in the queue, including the falling piece

#define FIELD_X                         (WINDOW_WIDTH / 4 - FIELD_WIDTH_PX / 2) // center to left half
#define FIELD_Y                         ((WINDOW_HEIGHT - FIELD_HEIGHT_PX) / 2) // center
#define FIELD_BORDER_WIDTH              1
#define FIELD_BORDER_COLOR              COLOR_WHITE
#define FIELD_BG_COLOR                  COLOR_BLACK
#define FIELD_WIDTH                     10 // TODO: set to 10
#define FIELD_HEIGHT                    20 // TODO: set to 20

#define HUD_X                           (WINDOW_WIDTH / 2) // set to -1 for auto calculation (center to right half)
#define HUD_Y                           -1 // set to -1 for auto calculation (center to top half if possible, else center)
#define HUD_BORDER_WIDTH                FIELD_BORDER_WIDTH
#define HUD_BORDER_COLOR                FIELD_BORDER_COLOR
#define HUD_BG_COLOR                    FIELD_BG_COLOR
#define HUD_TEXT_SIZE                   24 // text height in pixels
#define HUD_TEXT_COLOR                  COLOR_WHITE
#define HUD_SCORE_WIDTH                 7 // number of digits for score
#define HUD_LEVEL_WIDTH                 2 // number of digits for level
#define HUD_PADDING                     4
#define HUD_WIDTH                       0 // excluding border and padding, set to 0 for auto calculation
#define HUD_HEIGHT                      0 // excluding border and padding, set to 0 for auto calculation

#define PIECE_SIZE                      20
#define PIECE_MARGIN                    1
#define PIECE_PADDING                   2
#define PIECE_BORDER_WIDTH              1
#define PIECE_BORDER_COLOR              COLOR_BLACK

#define PIECE_COLOR_CYAN                COLOR_CYAN
#define PIECE_COLOR_BLUE                COLOR_BLUE
#define PIECE_COLOR_ORANGE              COLOR_ORANGE
#define PIECE_COLOR_YELLOW              COLOR_YELLOW
#define PIECE_COLOR_GREEN               COLOR_GREEN
#define PIECE_COLOR_PURPLE              COLOR_PURPLE
#define PIECE_COLOR_RED                 COLOR_RED

#define SCORE_FORCE_DOWN                1
#define SCORE_CLEAR_1                   100
#define SCORE_CLEAR_2_CONT              200
#define SCORE_CLEAR_2_SPLIT             250
#define SCORE_CLEAR_3_CONT              300
#define SCORE_CLEAR_3_SPLIT             350
#define SCORE_CLEAR_4                   500

#define SPEED_BASE                      1 // in updates per second
#define SPEED_STEP                      0.25
#define SPEED_INPUT_MOVE                0.1 // in seconds
#define SPEED_INPUT_FORCE_DOWN          0.05
#define SPEED_INPUT_ROTATE              0.25

/* DERIVED VALUES */
#define FIELD_WIDTH_PX                  (FIELD_WIDTH * (PIECE_SIZE + 2 * PIECE_MARGIN))
#define FIELD_HEIGHT_PX                 (FIELD_HEIGHT * (PIECE_SIZE + 2 * PIECE_MARGIN))

#define PIECE_TOTAL_SIZE                (PIECE_SIZE + 2 * PIECE_MARGIN)

#endif
