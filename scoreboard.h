#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "splashkit.h"

/**
 * @brief The actual starting X/Y position of the scoreboard.
 * 
 */
#define SCOREBOARD_START                (SCOREBOARD_BORDER_WIDTH + SCOREBOARD_PADDING)

/**
 * @brief Create the scoreboard database if one does not exist yet, then load and return it.
 * 
 * @return database The scoreboard database.
 */
database load_scoreboard();

/**
 * @brief Draw the scoreboard in the window centre.
 * 
 * @param db The scoreboard database.
 * @param last_line The last line to be added after the scoreboard entries (optional). Set this to an empty string to remove the line.
 * @param entries The number of top-scoring entries to display. Defaults to 5.
 */
void draw_scoreboard(database db, string last_line = "PRESS ENTER TO RETURN", int entries = 5);

/**
 * @brief Add a score to the scoreboard.
 * 
 * @param db The scoreboard database.
 * @param name The player's name.
 * @param score The player's score.
 */
void add_score(database db, string name, int score);

/**
 * @brief Open the scoreboard database, add a score to it, then close the database. Note that this will invalidate all other opening database instances.
 * 
 * @param name The player's name.
 * @param score The player's score.
 */
void add_score(string name, int score);

#endif