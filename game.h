#ifndef GAME_H
#define GAME_H

#include "splashkit.h"

#include "piece.h"
#include "config.h"
#include <deque>

using namespace std;

/**
 * @brief HUD drawing options; this is used primarily in draw_hud().
 * 
 * @field start_x The X coordinate of the HUD content's top left corner (within the padding and border).
 * @field start_y The Y coordinate of the HUD content's top left corner (within the padding and border).
 * @field content_width The width of the HUD content box.
 * @field content_height The height of the HUD content box.
 * @field char_width The HUD character width.
 * @field char_height The HUD character height.
 * @field hud_font THe HUD font structure, for use with SplashKit.
 * 
 */
struct hud_drawing_options {
    int start_x;
    int start_y;

    int content_width;
    int content_height;

    int char_width;
    int char_height;
    
    font hud_font;
};

/**
 * @brief The game data structure.
 * 
 * @field score The player's score.
 * @field level The player's level (zero-based).
 * 
 * @field score_lvlup The player's score on the last level increment.
 * 
 * @field playing_field The playing field.
 * @field next_pieces The falling and next pieces queue.
 * 
 * @field frame_num The current frame number.
 * 
 * @field frame_last_update The frame number of the last game update (in normal operations mode).
 * 
 * @field frame_last_move The frame number of the last accepted left/right move action.
 * @field frame_last_down The frame number of the last accepted down move action.
 * @field frame_last_rotate The frame number of the last accepted rotation action.
 * @field frame_last_swap The frame number of the last accepted piece swap action.
 * 
 * @field hud_options HUD drawing options.
 * 
 * @field game_over Game over flag.
 * @field game_over_filled Set after the playing field has been filled for the game over screen.
 * @field frame_game_over The frame number where the game over condition was detected.
 * @field show_scoreboard Set when the game scoreboard is to be shown after asking the player to add their score.
 * 
 * @field scoreboard The scoreboard database. This is only opened upon setting of game_over_filled, and is closed when the game returns back to the title screen.
 * 
 */
struct game_data {
    int score;
    int level;

    int score_lvlup;

    piece_colour playing_field[FIELD_HEIGHT][FIELD_WIDTH];
    deque<piece> next_pieces;

    uint64_t frame_num;

    uint64_t frame_last_update;

    /* input */
    uint64_t frame_last_move;
    uint64_t frame_last_down;
    uint64_t frame_last_rotate;
    uint64_t frame_last_swap;

    /* HUD */
    hud_drawing_options hud_options;

    /* game over */
    bool game_over;
    bool game_over_filled;
    uint64_t frame_game_over;
    bool show_scoreboard;

    database scoreboard;
};

/**
 * @brief Removed rows information.
 * 
 * @field flags Array of flags indicating which rows have been removed.
 * @field count The number of removed rows.
 * 
 */
struct removed_rows {
    bool flags[FIELD_HEIGHT];
    int count;
};

/**
 * @brief Create a new game given the starting level.
 * 
 * @param level The game's starting level (defaults to 1st level).
 * @return game_data The created game data structure.
 */
game_data new_game(int level = 0);

/**
 * @brief Create a new game given the settings JSON struct to load the level from.
 * 
 * @param settings The settings JSON structure.
 * @return game_data The created game data structure.
 */
game_data new_game(json settings);

/**
 * @brief Bitmask for left side collision. Returned by check_collision().
 * 
 */
#define COLLISION_LEFT          (1 << 0)

/**
 * @brief Bitmask for right side collision. Returned by check_collision().
 * 
 */
#define COLLISION_RIGHT         (1 << 1)

/**
 * @brief Bitmask for ceiling collision. Returned by check_collision().
 * 
 */
#define COLLISION_CEILING       (1 << 2)

/**
 * @brief Bitmask for bottom (or floor) collision. Returned by check_collision().
 * 
 */
#define COLLISION_BOTTOM        (1 << 3)

/**
 * @brief Check for collision between a test piece and a game's playing field.
 * 
 * @param game The game whose playing field will be used to check against.
 * @param test_piece The test piece to check against.
 * @return uint8_t Collision status flags; see COLLISION_LEFT, COLLISION_RIGHT, COLLISION_CEILING and COLLISION_BOTTOM.
 */
uint8_t check_collision(const game_data &game, const piece &test_piece);

/**
 * @brief Check for collision between the falling piece in a game and the playing field.
 * 
 * @param game The game data structure to check.
 * @return uint8_t Collision status flags; see COLLISION_LEFT, COLLISION_RIGHT, COLLISION_CEILING and COLLISION_BOTTOM.
 */
uint8_t check_collision(const game_data &game);

/**
 * @brief Handle inputs during game over.
 * 
 * @param game The game data structure.
 * 
 * @return true Returned if the player has not chosen to start a new game.
 * @return false Returned if the player has chosen to start a new game.
 */
bool handle_game_over(game_data &game);

/**
 * @brief Handle left move action.
 * 
 * @param game The game data structure.
 */
void handle_left_move(game_data &game);

/**
 * @brief Handle right move action.
 * 
 * @param game The game data structure.
 */
void handle_right_move(game_data &game);

/**
 * @brief Handle down move action.
 * 
 * @param game The game data structure.
 */
void handle_down_move(game_data &game);

/**
 * @brief Handle piece rotation.
 * 
 * @param game The game data structure.
 */
void handle_rotate(game_data &game);

/**
 * @brief Handle piece swapping action.
 * 
 * @param game The game data structure.
 */
void handle_swap(game_data &game);

/**
 * @brief Handle user inputs.
 * 
 * @param game The game data structure.
 * @return true Returned if the game can proceed as normal.
 * @return false Returned if a new game is to be created (i.e. after a game over).
 */
bool handle_game_input(game_data &game);

/**
 * @brief Draw the playing field and the falling piece.
 * 
 * @param game The game data structure.
 */
void draw_field(const game_data &game);

/**
 * @brief Draw the game's HUD.
 * 
 * @param game The game data structure.
 */
void draw_hud(const game_data &game);

/**
 * @brief Draw the scoreboard input window in the centre of the window.
 * 
 * @param game The game data structure.
 */
void draw_scoreboard_input(const game_data &game);

/**
 * @brief Draw the game over text on top of the playing field. This is supposed to be called after game.game_over_filled is set.
 * 
 * @param game The game data structure.
 */
void draw_game_over(const game_data &game);

/**
 * @brief Draw a frame of the game and refresh the screen.
 * 
 * @param game The game data structure.
 */
void draw_game(const game_data &game);

/**
 * @brief Merge a game's falling piece into its playing field.
 * 
 * @param game The game data structure.
 */
void merge_piece(game_data &game);

/**
 * @brief Detect and remove filled rows in a game's playing field.
 * 
 * @param game The game data structure.
 * @return removed_rows A data structure containing information on the removed rows.
 */
removed_rows remove_full_rows(game_data &game);

/**
 * @brief Award score to the player depending on the number and pattern of removed filled rows.
 * 
 * @param game The game data structure.
 * @param rows Information on removed rows, returned by remove_full_rows().
 */
void award_score(game_data &game, const removed_rows &rows);

/**
 * @brief Pop the falling piece out of the game's next pieces queue and add a new one to its end.
 * 
 * @param game The game data structure.
 */
void next_piece(game_data &game);

/**
 * @brief Check for the game over condition.
 * 
 * @param game The game data structure.
 * @return true Returned on game over (i.e. the falling piece is overflowing from the playing field)/
 * @return false Returned if the game can proceed as usual.
 */
bool check_game_over(const game_data &game);

/**
 * @brief Perform the game's logic; this is called on each frame.
 * 
 * @param game The game data structure.
 */
void update_game(game_data &game);

#endif