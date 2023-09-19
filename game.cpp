#include "game.h"
#include "piece.h"

using namespace std;

/**
 * @brief Macro function to find the minimum of two values.
 * 
 */
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))

/**
 * @brief Macro function to find the maximum of two values.
 * 
 */
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))

/* create new game struct */
game_data new_game() {
    game_data result;

    result.score = 0; result.level = 0;
    result.frame_num = 0; result.frame_last_update = 0;
    result.last_input_action = NO_INPUT; result.frame_last_input = 0;

    result.game_over = false; result.game_over_filled = false;

    result.next_pieces = new_pieces(NEXT_PIECES_CNT);
    
    for(int y = 0; y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < FIELD_WIDTH; x++)
            result.playing_field[y][x] = NO_COLOUR;
    }

    /* set up parameters for HUD */
    result.hud_options.hud_font = font_named("GameFont");

    result.hud_options.char_width = text_width("0", result.hud_options.hud_font, HUD_TEXT_SIZE);
    result.hud_options.char_height = text_height("0", result.hud_options.hud_font, HUD_TEXT_SIZE);
    // write_line("Text size: " + to_string(result.hud_options.char_width) + "x" + to_string(result.hud_options.char_height));

#if HUD_WIDTH > 0
    result.hud_options.content_width = HUD_WIDTH;
#else
    result.hud_options.content_width = text_width("SCORE: " + string(HUD_SCORE_WIDTH, '0'), result.hud_options.hud_font, HUD_TEXT_SIZE);
    result.hud_options.content_width = MAX(result.hud_options.content_width, text_width("LEVEL: " + string(HUD_LEVEL_WIDTH, '0'), result.hud_options.hud_font, HUD_TEXT_SIZE));
    result.hud_options.content_width = MAX(result.hud_options.content_width, text_width("<< NEXT >>", result.hud_options.hud_font, HUD_TEXT_SIZE));
#endif

#if HUD_HEIGHT > 0
    result.hud_options.content_height = HUD_HEIGHT;
#else
    result.hud_options.content_height = 4 * result.hud_options.char_height + (NEXT_PIECES_CNT - 1) * 4 * (PIECE_SIZE + 2 * PIECE_MARGIN);
#endif

    // write_line("HUD content size: " + to_string(result.hud_options.content_width) + "x" + to_string(result.hud_options.content_height));

#if HUD_X >= 0
    result.hud_options.start_x = HUD_X + 2 * (HUD_PADDING + HUD_BORDER_WIDTH);
#else
    result.hud_options.start_x = (3 * WINDOW_WIDTH / 4) - (result.hud_options.content_width + 2 * (HUD_PADDING + HUD_BORDER_WIDTH)) / 2;
#endif

#if HUD_Y >= 0
    result.hud_options.start_y = HUD_Y + 2 * (HUD_PADDING + HUD_BORDER_WIDTH);
#else
    if(result.hud_options.content_height <= WINDOW_WIDTH / 2)
        result.hud_options.start_y = (WINDOW_HEIGHT / 4) - (result.hud_options.content_height + 2 * (HUD_PADDING + HUD_BORDER_WIDTH)) / 2;
    else
        result.hud_options.start_y = (WINDOW_HEIGHT / 2) - (result.hud_options.content_height + 2 * (HUD_PADDING + HUD_BORDER_WIDTH)) / 2;
#endif

    return result;
}

/* check collision (overlaps) between the falling piece and its surrounding field */
uint8_t check_collision(const game_data &game, const piece &test_piece) {
    uint8_t result = 0;

    int field_y = test_piece.position.y + test_piece.type->bitmaps[test_piece.rotation].y;
    for(int y = test_piece.type->bitmaps[test_piece.rotation].y; y < test_piece.type->bitmaps[test_piece.rotation].y + test_piece.type->bitmaps[test_piece.rotation].height; y++, field_y++) {
        int field_x = test_piece.position.x + test_piece.type->bitmaps[test_piece.rotation].x;
        if(field_y < 0) result |= COLLISION_CEILING; // definitely ceiling collision; the cell is above the upper bound of the playing field
        else if(field_y >= FIELD_HEIGHT) result |= COLLISION_BOTTOM; // definitely floor collision; the cell is below the lower bound of the playing field
        for(int x = test_piece.type->bitmaps[test_piece.rotation].x; x < test_piece.type->bitmaps[test_piece.rotation].x + test_piece.type->bitmaps[test_piece.rotation].width; x++, field_x++) {
            if(test_piece.type->bitmaps[test_piece.rotation].bitmap & (1 << (y * 4 + x))) {
                /* there is a cell, so let's check here */
                if(field_x < 0) result |= COLLISION_LEFT; // definitely left collision
                else if(field_x >= FIELD_WIDTH) result |= COLLISION_RIGHT; // definitely right collision
                else {
                    /* the cell is within playing field bounds */
                    if(field_y >= 0 && field_y < FIELD_HEIGHT && game.playing_field[field_y][field_x] != NO_COLOUR) result |= COLLISION_LEFT | COLLISION_RIGHT | COLLISION_BOTTOM; // the caller will figure out what this really is
                }
            }
        }
    }

    return result;
}

uint8_t check_collision(const game_data &game) {
    return check_collision(game, game.next_pieces[0]);
}

/* handle game inputs */
bool handle_input(game_data &game) {
    if(game.game_over) {
        return !key_down(RETURN_KEY); // only check if the RETURN key is pressed; if it is, then we start a new game
    } else {
        uint64_t frame_delta = game.frame_num - game.frame_last_input;

        if(key_down(LEFT_KEY) && (game.last_input_action != MOVE_LEFT || frame_delta > FRAME_RATE / SPEED_INPUT_MOVE)) { // move piece to the left
            game.last_input_action = MOVE_LEFT;
            game.frame_last_input = game.frame_num;

            game.next_pieces[0].position.x--; // try shifting it to the left for testing
            if(check_collision(game) & COLLISION_LEFT) {
                game.next_pieces[0].position.x++;
#ifdef DEBUG_INPUT_REJECTIONS
                write_line("Left move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
            }
        }

        if(key_down(RIGHT_KEY) && (game.last_input_action != MOVE_RIGHT || frame_delta > FRAME_RATE / SPEED_INPUT_MOVE)) { // move piece to the right
            game.last_input_action = MOVE_RIGHT;
            game.frame_last_input = game.frame_num;

            game.next_pieces[0].position.x++;
            if(check_collision(game) & COLLISION_RIGHT) {
                game.next_pieces[0].position.x--;
#ifdef DEBUG_INPUT_REJECTIONS
                write_line("Right move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
            }
        }

        if(key_down(DOWN_KEY) && (game.last_input_action != FORCE_DOWN || frame_delta > FRAME_RATE / SPEED_INPUT_FORCE_DOWN)) { // move piece down
            game.last_input_action = FORCE_DOWN;
            game.frame_last_input = game.frame_num;

            game.next_pieces[0].position.y++;
            if(check_collision(game) & COLLISION_BOTTOM) {
                game.next_pieces[0].position.y--;
#ifdef DEBUG_INPUT_REJECTIONS
                write_line("Down move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
            } else game.score += SCORE_FORCE_DOWN;
        }

        if(key_down(UP_KEY) && (game.last_input_action != ROTATE || frame_delta > FRAME_RATE / SPEED_INPUT_ROTATE)) { // rotate piece
            game.last_input_action = ROTATE;
            game.frame_last_input = game.frame_num;

            piece new_piece = game.next_pieces[0]; // rotated piece
            new_piece.rotation = (new_piece.rotation + 1) % 4;

            if(!(check_collision(game, new_piece) & ~COLLISION_CEILING)) game.next_pieces[0] = new_piece; // ignore ceiling collision
            else {
                /* attempt wall kicking */
                bool ok = false; // set if the piece fits
                new_piece.position.x++; // right kick
                if(!(check_collision(game, new_piece) & ~COLLISION_CEILING)) ok = true;
                else {
                    new_piece.position.x -= 2; // left kick
                    if(!(check_collision(game, new_piece) & ~COLLISION_CEILING)) ok = true;
                }
                if(ok) game.next_pieces[0] = new_piece;
#ifdef DEBUG_INPUT_REJECTIONS
                else write_line("Rotation rejected for " + piece_to_string(game.next_pieces[0]));
#endif
            }
        }

        return true;
    }
}

/* draw the playing field */
void draw_field(const game_data &game) {
    /* draw field border and background */
    draw_rectangle(FIELD_BORDER_COLOR, FIELD_X, FIELD_Y, FIELD_WIDTH * (PIECE_SIZE + 2 * PIECE_MARGIN) + 2 * PIECE_MARGIN, FIELD_HEIGHT * (PIECE_SIZE + 2 * PIECE_MARGIN) + 2 * PIECE_MARGIN, option_line_width(FIELD_BORDER_WIDTH));
    fill_rectangle(FIELD_BG_COLOR, FIELD_X + FIELD_BORDER_WIDTH, FIELD_Y + FIELD_BORDER_WIDTH, FIELD_WIDTH * (PIECE_SIZE + 2 * PIECE_MARGIN), FIELD_HEIGHT * (PIECE_SIZE + 2 * PIECE_MARGIN));
    
    /* draw the field (minus the falling piece) */
    for(int y = 0; y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < FIELD_WIDTH; x++) {
            draw_cell(game.playing_field[y][x], {x, y});
        }
    }

    draw_piece(game.next_pieces[0]); // draw the falling piece
}

/**
 * @brief Convert an integer to a string, optionally with padding.
 * 
 * @param num The number to be converted.
 * @param padding The number of characters to pad the number to (optional, defaults to 0 for no padding).
 * @param pad_char The padding character (optional, defaults to zero padding).
 * @return string The decimal string representation of num.
 */
static string int_to_string(int num, int padding = 0, char pad_char = '0') {
    string result = to_string(num);
    if(padding > 0) result = string(padding - result.length(), pad_char) + result;
    return result;
}

/**
 * @brief Macro for the HUD content area's starting X coordinate.
 * 
 */
#define HUD_CONTENT_X                   (game.hud_options.start_x + HUD_BORDER_WIDTH + HUD_PADDING)

/**
 * @brief Macro for the HUD content area's starting Y coordinate.
 * 
 */
#define HUD_CONTENT_Y                   (game.hud_options.start_y + HUD_BORDER_WIDTH + HUD_PADDING)

/* draw the HUD */
void draw_hud(const game_data &game) {
    /* draw HUD border and background */
    draw_rectangle(HUD_BORDER_COLOR, game.hud_options.start_x, game.hud_options.start_y, game.hud_options.content_width + 2 * (HUD_BORDER_WIDTH + HUD_PADDING), game.hud_options.content_height + 2 * (HUD_BORDER_WIDTH + HUD_PADDING));
    fill_rectangle(HUD_BG_COLOR, game.hud_options.start_x + HUD_BORDER_WIDTH, game.hud_options.start_y + HUD_BORDER_WIDTH, game.hud_options.content_width + 2 * HUD_PADDING, game.hud_options.content_height + 2 * HUD_PADDING);

    draw_text("SCORE: " + ((HUD_SCORE_WIDTH < HUD_LEVEL_WIDTH) ? string(HUD_LEVEL_WIDTH - HUD_SCORE_WIDTH, ' ') : "") + int_to_string(game.score, HUD_SCORE_WIDTH), HUD_TEXT_COLOR, game.hud_options.hud_font, HUD_TEXT_SIZE, HUD_CONTENT_X, HUD_CONTENT_Y); // display the current score
    draw_text("LEVEL: " + int_to_string(game.level + 1, MAX(HUD_SCORE_WIDTH, HUD_LEVEL_WIDTH), ' '), HUD_TEXT_COLOR, game.hud_options.hud_font, HUD_TEXT_SIZE, HUD_CONTENT_X, HUD_CONTENT_Y + game.hud_options.char_height); // display the current score
    
    /* draw next pieces */
    int next_str_width = text_width("<< NEXT >>", game.hud_options.hud_font, HUD_TEXT_SIZE); // get width of the text so we can center it
    draw_text("<< NEXT >>", HUD_TEXT_COLOR, game.hud_options.hud_font, HUD_TEXT_SIZE, HUD_CONTENT_X + (game.hud_options.content_width - next_str_width) / 2, HUD_CONTENT_Y + 2.5 * game.hud_options.char_height);
    int next_piece_center_y = HUD_CONTENT_Y + 4 * game.hud_options.char_height + 2 * PIECE_TOTAL_SIZE;
    for(int i = 1; i < NEXT_PIECES_CNT; i++, next_piece_center_y += 4 * PIECE_TOTAL_SIZE) {
        // write_line(to_string(i) + ": " + to_string(piece_width(game.next_pieces[i])) + "x" + to_string(piece_height(game.next_pieces[i])));
        draw_piece(game.next_pieces[i], {(HUD_CONTENT_X + (game.hud_options.content_width - PIECE_TOTAL_SIZE * game.next_pieces[i].type->bitmaps[game.next_pieces[i].rotation].width) / 2), (next_piece_center_y - (PIECE_TOTAL_SIZE * game.next_pieces[i].type->bitmaps[game.next_pieces[i].rotation].height) / 2)}, true, true);
    }
}

/* draw the game over screen */
void draw_game_over(const game_data &game) {
    /* we want to center the text, so we will need to calculate where to put it */
    int width = text_width("GAME OVER", game.hud_options.hud_font, GAME_OVER_TEXT_SIZE);
    int height = text_height("GAME OVER", game.hud_options.hud_font, GAME_OVER_TEXT_SIZE);
    int x = FIELD_X + (FIELD_WIDTH_PX - width) / 2;
    int y = FIELD_Y + FIELD_HEIGHT_PX / 2 - height;
    fill_rectangle(FIELD_BG_COLOR, x, y, width, height);
    draw_text("GAME OVER", HUD_TEXT_COLOR, game.hud_options.hud_font, GAME_OVER_TEXT_SIZE, x, y);

    width = text_width("Press ENTER to restart", game.hud_options.hud_font, GAME_OVER_TEXT_SIZE);
    height = text_height("Press ENTER to restart", game.hud_options.hud_font, GAME_OVER_TEXT_SIZE);
    x = FIELD_X + (FIELD_WIDTH_PX - width) / 2;
    y = FIELD_Y + FIELD_HEIGHT_PX / 2;
    fill_rectangle(FIELD_BG_COLOR, x, y, width, height);
    draw_text("Press ENTER to restart", HUD_TEXT_COLOR, game.hud_options.hud_font, GAME_OVER_TEXT_SIZE, x, y);
}

/* draw entire game */
void draw_game(const game_data &game) {
    draw_field(game);
    if(game.game_over_filled) draw_game_over(game);

    draw_hud(game);

    refresh_screen(FRAME_RATE);
}

/* merge falling piece into playing field */
void merge_piece(game_data &game) {
    int field_y = game.next_pieces[0].position.y;
    for(int y = 0; y < 4; y++, field_y++) {
        if(field_y < 0 || field_y >= FIELD_HEIGHT) continue; // skip through out of bound rows
        int field_x = game.next_pieces[0].position.x;
        for(int x = 0; x < 4; x++, field_x++) {
            if(field_x < 0 || field_x >= FIELD_WIDTH) continue; // skip through out of bound cells
            if(PIECE_ROW(game.next_pieces[0].type->bitmaps[game.next_pieces[0].rotation].bitmap, y) & (1 << x))
                game.playing_field[field_y][field_x] = game.next_pieces[0].type->p_color;
        }
    }
}

/* remove full rows from playing field and return information */
removed_rows remove_full_rows(game_data &game) {
    removed_rows result;
    result.count = 0;

    for(int y = 0; y < FIELD_HEIGHT; y++) {
        /* scan through each row */
        bool full = true;
        for(int x = 0; x < FIELD_WIDTH; x++) {
            if(game.playing_field[y][x] == NO_COLOUR) {
                full = false;
                break; // no need to continue scanning through this row, since it's not full
            }
        }
        
        if(full) {
            /* collapse rows above it down */
            for(int y_c = y - 1; y_c >= 0; y_c--) {
                for(int x = 0; x < FIELD_WIDTH; x++)
                    game.playing_field[y_c + 1][x] = game.playing_field[y_c][x];
            }
            for(int x = 0; x < FIELD_WIDTH; x++) game.playing_field[0][x] = NO_COLOUR; // clear top row

            result.count++;
        }

        result.flags[y] = full;
    }

    return result;
}

/* award score and level-up to player depending on filled rows */
void award_score(game_data &game, const removed_rows &rows) {
    int i;
    switch(rows.count) {
        case 0:
            /* 0 rows - nothing to do here */
            break;
        case 1:
            /* single */
            game.score += SCORE_CLEAR_1;
            break;
        case 4:
            /* tetris */
            game.score += SCORE_CLEAR_4;
            game.level++;
            break;
        case 2:
            /* double */
            for(i = 0; i < FIELD_HEIGHT - 1; i++) { // we can't declare variables in switch-case
                if(rows.flags[i]) {
                    /* we found the first row - check the next one too */
                    if(rows.flags[i + 1]) game.score += SCORE_CLEAR_2_CONT; // continuous
                    else game.score += SCORE_CLEAR_2_SPLIT; // split
                    break;
                }
            }
            break;
        case 3:
            /* triple */
            for(i = 0; i < FIELD_HEIGHT - 2; i++) {
                if(rows.flags[i]) {
                    /* we found the first row - check the next two rows */
                    if(rows.flags[i + 1] && rows.flags[i + 2]) game.score += SCORE_CLEAR_3_CONT; // continuous
                    else game.score += SCORE_CLEAR_3_SPLIT; // split
                    break;
                }
            }
            break;
        default:
            /* does this case even happen? */
            break;
    }
}

/* generate new next piece */
void next_piece(game_data &game) {
    new_pieces(game.next_pieces, 1); // ensure that the new piece is not the same as the just-fallen piece
    game.next_pieces.pop_front();
}

/* check for game over condition */
bool check_game_over(const game_data &game) {
    return (game.next_pieces[0].position.y + game.next_pieces[0].type->bitmaps[game.next_pieces[0].rotation].y < 0); // if collision happened with part of the piece above the border
}

/* update game state */
void update_game(game_data &game) {
    if(game.game_over) {
        if(!game.game_over_filled) {
            int64_t frame_delta = game.frame_num - game.frame_game_over;
        
            if((frame_delta > 0) && (frame_delta % (uint64_t)(FRAME_RATE / GAME_OVER_FILL_RATE) == 0)) {
#ifdef GAME_OVER_FILL_FROM_BOTTOM
                int row = FIELD_HEIGHT - (frame_delta / (uint64_t)(FRAME_RATE / GAME_OVER_FILL_RATE) - 1);
                if(row < 0) {

#else
                int row = frame_delta / (uint64_t)(FRAME_RATE / GAME_OVER_FILL_RATE) - 1;
                if(row >= FIELD_HEIGHT) {
#endif
                    /* we're overfilling */
                    game.game_over_filled = true;
                    return;
                }

                for(int x = 0; x < FIELD_WIDTH; x++) game.playing_field[row][x] = GAME_OVER_FILL_COLOR; // fill the row
            }
        }
    } else {
        uint64_t frame_delta = game.frame_num - game.frame_last_update; // difference from frame number of last update to current frame number

        if((frame_delta > 0) && (frame_delta % (uint64_t)(FRAME_RATE / (SPEED_BASE + game.level * SPEED_STEP)) == 0)) {
            /* it's updating time */
            game.next_pieces[0].position.y++; // descend falling piece
            if(check_collision(game) & COLLISION_BOTTOM) {
                /* falling piece is touching a cell */
                game.next_pieces[0].position.y--; // pull it back up
                merge_piece(game); // merge piece into the playing field
                game.game_over = check_game_over(game); // check for game over condition
                next_piece(game); // pop next piece out

                if(game.game_over) {
                    /* game over */
                    game.frame_game_over = game.frame_num + (uint64_t)(FRAME_RATE / (SPEED_BASE + game.level * SPEED_STEP)); // we want the game to freeze for a bit
                    goto advance_frame; // we'll be back on the next frame
                } else {
                    /* the game's still progressing */
                    removed_rows rows = remove_full_rows(game); // find and remove full rows
                    award_score(game, rows); // then award score to player
                }
            }

            game.frame_last_update = game.frame_num;
        }
    }

advance_frame:
    game.frame_num++; // advance to next frame
}