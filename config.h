#ifndef CONFIG_H
#define CONFIG_H

#include "splashkit.h"

#define WINDOW_TITLE                    "Tetris"
#define WINDOW_WIDTH                    640
#define WINDOW_HEIGHT                   480
#define WINDOW_BG_COLOR                 COLOR_BLUE
#define FRAME_RATE                      60

#define FIELD_X                         10 // TODO
#define FIELD_Y                         10 // TODO
#define FIELD_BORDER_WIDTH              1
#define FIELD_BORDER_COLOR              COLOR_WHITE
#define FIELD_BG_COLOR                  COLOR_BLACK
#define FIELD_WIDTH                     10 // TODO: set to 10
#define FIELD_HEIGHT                    20 // TODO: set to 20

#define PIECE_SIZE                      14
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
#define SPEED_INPUT                     0.25 // in seconds

#endif
