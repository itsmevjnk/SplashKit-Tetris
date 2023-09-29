#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "splashkit.h"

/**
 * @brief The actual starting X/Y position of the scoreboard
 * 
 */
#define SCOREBOARD_START                (SCOREBOARD_BORDER_WIDTH + SCOREBOARD_PADDING)

database load_scoreboard();

void draw_scoreboard(database db, string last_line = "PRESS ENTER TO RETURN", int entries = 5);

#endif