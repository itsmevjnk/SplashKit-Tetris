#include "scoreboard.h"
#include "config.h"
#include "utils.h"
#include <sys/stat.h>

using namespace std;

/* create scoreboard if one does not exist yet and load it */
database load_scoreboard() {
    struct stat buffer;
    bool db_exists = (stat("Resources/databases/scoreboard.db", &buffer) == 0);
    database result = open_database("scoreboard", "scoreboard.db"); // this will create the DB for us if it's not there yet
    if(!db_exists) {
        /* new database */
        write_line("Creating DB.");
        query_result r = run_sql(result, "CREATE TABLE scoreboard (name TEXT, score INTEGER);");
        free_query_result(r);
    }
    return result;
}

/* display scoreboard in the centre of the window */
void draw_scoreboard(database db, string last_line, int entries) {
    /* fetch top entries from scoreboard database */
    query_result result = run_sql(db, "SELECT name, score FROM scoreboard ORDER BY score DESC LIMIT " + to_string(entries));

    font scoreboard_font = font_named("GameFont"); // get display font

    /* calculate scoreboard width */
    int title_width = text_width(" -- SCOREBOARD -- ", scoreboard_font, SCOREBOARD_TITLE_TEXT_SIZE); // title width
    int width = title_width;
    int line_width = text_width(string(SCOREBOARD_NAME_MAXLEN, 'A') + string(" ") + string(HUD_SCORE_WIDTH, '0'), scoreboard_font, SCOREBOARD_CONTENT_TEXT_SIZE); // record line width
    width = MAX(width, line_width);
    int last_line_width = 0; // width of the last line
    if(last_line.length() > 0) {
        last_line_width = text_width(last_line, scoreboard_font, SCOREBOARD_CONTENT_TEXT_SIZE);
        width = MAX(width, last_line_width);
    }
    width += 2 * SCOREBOARD_START;

    /* calculate scoreboard height */
    int height = 2 * SCOREBOARD_START;
    int title_height = text_height(" -- SCOREBOARD -- ", scoreboard_font, SCOREBOARD_TITLE_TEXT_SIZE); // title height
    height += title_height;
    int line_height = text_height(string(SCOREBOARD_NAME_MAXLEN, 'A') + string(" ") + string(HUD_SCORE_WIDTH, '0'), scoreboard_font, SCOREBOARD_CONTENT_TEXT_SIZE); // record line height
    height += entries * line_height;
    if(last_line.length() > 0) height += line_height;

    /* prepare bitmap for drawing scoreboard */
    bitmap scoreboard = create_bitmap("Scoreboard", width, height);
    clear_bitmap(scoreboard, SCOREBOARD_BG_COLOR);
    draw_rectangle_on_bitmap(scoreboard, SCOREBOARD_BORDER_COLOR, 0, 0, width, height, option_line_width(SCOREBOARD_BORDER_WIDTH));

    /* draw bitmap elements */
    draw_text_on_bitmap(scoreboard, " -- SCOREBOARD -- ", SCOREBOARD_TEXT_COLOR, scoreboard_font, SCOREBOARD_TITLE_TEXT_SIZE, (width - title_width) / 2, SCOREBOARD_START + 0);
    bool next_row = true;
    for(int i = 0; i < entries; i++) {
        if(!has_row(result)) next_row = false; // no more rows to read

        string line = "---";
        if(next_row) {
            line = query_column_for_string(result, 0); // insert name
            line += string(SCOREBOARD_NAME_MAXLEN + 1 - line.length(), ' '); // insert padding between name and score
            line += int_to_string(query_column_for_int(result, 1), HUD_SCORE_WIDTH); // insert padded score
            next_row = get_next_row(result); // advance to next row
        }

        draw_text_on_bitmap(scoreboard, line, SCOREBOARD_TEXT_COLOR, scoreboard_font, SCOREBOARD_CONTENT_TEXT_SIZE, (width - line_width) / 2, SCOREBOARD_START + title_height + i * line_height);
    }
    if(last_line.length() > 0) {
        draw_text_on_bitmap(scoreboard, last_line, SCOREBOARD_TEXT_COLOR, scoreboard_font, SCOREBOARD_CONTENT_TEXT_SIZE, (width - last_line_width) / 2, SCOREBOARD_START + title_height + entries * line_height);
    }

    /* draw bitmap on screen */
    draw_bitmap(scoreboard, (WINDOW_WIDTH - width) / 2, (WINDOW_HEIGHT - height) / 2);
    free_bitmap(scoreboard); // clean up
}

/* add score to scoreboard */
void add_score(database db, string name, int score) {
    query_result r = run_sql(db, "INSERT INTO scoreboard (name, score) VALUES ('" + name + "', " + to_string(score) + ");");
    free_query_result(r);
}

void add_score(string name, int score) {
    database db = load_scoreboard();
    add_score(db, name, score);
    free_database(db);
}