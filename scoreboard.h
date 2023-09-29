#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "splashkit.h"

database load_scoreboard();

void draw_scoreboard(database db, string last_line = "PRESS ENTER TO RETURN", int entries = 5);

#endif