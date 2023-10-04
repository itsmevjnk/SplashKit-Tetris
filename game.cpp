#include "game.h"
#include "piece.h"
#include "utils.h"
#include "settings.h"
#include "scoreboard.h"

using namespace std;

/* create new game struct */
game_data new_game(int level) {
    game_data result;

    result.score = 0; result.level = level; result.score_lvlup = 0;
    result.frame_num = 0; result.frame_last_update = 0;
    result.frame_last_move = 0; result.frame_last_down = 0; result.frame_last_rotate = 0;

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

    clear_screen(GAME_BG_COLOR);

    return result;
}

game_data new_game(json settings) {
    return new_game(get_level(settings));
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

/* handle game over input */
bool handle_game_over(game_data &game) {
    if(!game.game_over_filled) return true; // lock input until stuff's actually happening

    if(key_released(RETURN_KEY) && !game.show_scoreboard) {
        /* save record to scoreboard */
        // write_line(text_input() + " " + to_string(game.score)); // TODO
        add_score(game.scoreboard, text_input(), game.score);
    }

    if(key_released(RETURN_KEY) || key_released(ESCAPE_KEY)) {
        if(!game.show_scoreboard) {
            game.show_scoreboard = true; // switch to scoreboard
        } else {
            free_database(game.scoreboard); // close database
            return false; // start new game
        }
    }

    return true;
}

/* handle left move */
void handle_left_move(game_data &game) {
    game.next_pieces[0].position.x--; // try shifting it to the left for testing
    if(check_collision(game) & COLLISION_LEFT) {
        game.next_pieces[0].position.x++;
#ifdef DEBUG_INPUT_REJECTIONS
        write_line("Left move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
    } else game.frame_last_move = game.frame_num;
}

/* handle right move */
void handle_right_move(game_data &game) {
    game.next_pieces[0].position.x++;
    if(check_collision(game) & COLLISION_RIGHT) {
        game.next_pieces[0].position.x--;
#ifdef DEBUG_INPUT_REJECTIONS
        write_line("Right move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
    } else game.frame_last_move = game.frame_num;
}

/* handle down move/force */
void handle_down_move(game_data &game) {
    game.next_pieces[0].position.y++;
    if(check_collision(game) & COLLISION_BOTTOM) {
        game.next_pieces[0].position.y--;
#ifdef DEBUG_INPUT_REJECTIONS
        write_line("Down move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
    } else {
        game.score += SCORE_FORCE_DOWN;
        game.frame_last_down = game.frame_num;
    }
}

/* handle piece rotation */
void handle_rotate(game_data &game) {
    piece new_piece = game.next_pieces[0]; // rotated piece
    new_piece.rotation = (new_piece.rotation + 1) % 4;

    bool ok = false; // set if the piece fits

    if(!(check_collision(game, new_piece) & ~COLLISION_CEILING)) ok = true; // ignore ceiling collision
    else {
        /* attempt wall kicking */
        new_piece.position.x++; // right kick
        if(!(check_collision(game, new_piece) & ~COLLISION_CEILING)) ok = true;
        else {
            new_piece.position.x -= 2; // left kick
            if(!(check_collision(game, new_piece) & ~COLLISION_CEILING)) ok = true;
        }
    }

    if(ok) {
        game.next_pieces[0] = new_piece;
        game.frame_last_rotate = game.frame_num;
    }
#ifdef DEBUG_INPUT_REJECTIONS
    else write_line("Rotation rejected for " + piece_to_string(game.next_pieces[0]));
#endif
}

/* handle piece swap */
void handle_swap(game_data &game) {
    piece new_piece = game.next_pieces[1]; // the piece that we'll be swapping with

    piece_position current_centre = piece_centre_point(game.next_pieces[0]); // the current falling piece's centre point coordinates
    piece_position new_centre = piece_centre_point(new_piece, true); // the new falling piece's centre point offset

    /* translate the new piece such that its centre point matches that of the old piece */
    new_piece.position.x = current_centre.x - new_centre.x;
    new_piece.position.y = current_centre.y - new_centre.y;

    /* check if the new piece fits */
    if(!(check_collision(game, new_piece) & ~COLLISION_CEILING)) {
        /* yes, it fits */
        position_piece(game.next_pieces[0]); // reposition back to the top of the field
        game.next_pieces.push_back(game.next_pieces[0]); // push the old piece to the back
        game.next_pieces.pop_front(); // pop the old piece off
        game.next_pieces[0] = new_piece; // replace the new piece with the one with the calculated values

        game.frame_last_swap = game.frame_num;
    }
#ifdef DEBUG_INPUT_REJECTIONS
    else write_line("Swap rejected for " + piece_to_string(game.next_pieces[0]) + " (attempted to swap for " + piece_to_string(new_piece) + ")");
#endif   
}

/* handle game inputs */
bool handle_game_input(game_data &game) {
    if(game.game_over) return handle_game_over(game);
    else {
        if(key_down(LEFT_KEY) && (game.frame_last_move == 0 || game.frame_num - game.frame_last_move >= FRAME_RATE / SPEED_INPUT_MOVE))
            handle_left_move(game);

        if(key_down(RIGHT_KEY) && (game.frame_last_move == 0 || game.frame_num - game.frame_last_move >= FRAME_RATE / SPEED_INPUT_MOVE))
            handle_right_move(game);

        if(key_down(DOWN_KEY) && (game.frame_last_down == 0 || game.frame_num - game.frame_last_down >= FRAME_RATE / SPEED_INPUT_FORCE_DOWN))
            handle_down_move(game);

        if(key_down(UP_KEY) && (game.frame_last_rotate == 0 || game.frame_num - game.frame_last_rotate >= FRAME_RATE / SPEED_INPUT_ROTATE))
            handle_rotate(game);

        if(key_down(SPACE_KEY) && (game.frame_last_swap == 0 || game.frame_num - game.frame_last_swap >= FRAME_RATE / SPEED_INPUT_SWAP))
            handle_swap(game);

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

/* draw the scoreboard input window */
void draw_scoreboard_input(const game_data &game) {
    /* calculate window width */
    int title_width = text_width("PLEASE ENTER YOUR NAME", game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE);
    int decoration_width = text_width("\x10 ", game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE);
    int line_width_max = 2 * decoration_width + text_width(string(SCOREBOARD_NAME_MAXLEN, 'A'), game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE);
    int width = MAX(title_width, line_width_max) + 2 * SCOREBOARD_START;

    /* calculate window height */
    int height = 2 * SCOREBOARD_START;
    int line_height = text_height(string(SCOREBOARD_NAME_MAXLEN, 'A'), game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE);
    height += 2 * line_height;

    /* create and prepare window bitmap */
    bitmap scoreboard_input = create_bitmap("ScoreboardInput", width, height);
    clear_bitmap(scoreboard_input, SCOREBOARD_BG_COLOR);
    draw_rectangle_on_bitmap(scoreboard_input, SCOREBOARD_BORDER_COLOR, 0, 0, width, height, option_line_width(SCOREBOARD_BORDER_WIDTH));

    /* draw text elements */
    draw_text_on_bitmap(scoreboard_input, "PLEASE ENTER YOUR NAME", SCOREBOARD_TEXT_COLOR, game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE, (width - title_width) / 2, SCOREBOARD_START + 0);
    draw_text_on_bitmap(scoreboard_input, "\x10 ", SCOREBOARD_TEXT_COLOR, game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE, SCOREBOARD_START + 0, SCOREBOARD_START + line_height);
    draw_text_on_bitmap(scoreboard_input, " \x11", SCOREBOARD_TEXT_COLOR, game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE, width - SCOREBOARD_START - decoration_width, SCOREBOARD_START + line_height);
    
    string player_name = text_input();
    int line_width = text_width(player_name, game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE);
    draw_text_on_bitmap(scoreboard_input, player_name, SCOREBOARD_TEXT_COLOR, game.hud_options.hud_font, SCOREBOARD_CONTENT_TEXT_SIZE, (width - line_width) / 2, SCOREBOARD_START + line_height);

    /* now compose the bitmap onto the window */
    draw_bitmap(scoreboard_input, (WINDOW_WIDTH - width) / 2, (WINDOW_HEIGHT - height) / 2);
    free_bitmap(scoreboard_input);
}

/* draw the game over screen */
void draw_game_over(const game_data &game) {
    /* we want to center the text, so we will need to calculate where to put it */
    int width = text_width("GAME OVER", game.hud_options.hud_font, GAME_OVER_TEXT_SIZE);
    int height = text_height("GAME OVER", game.hud_options.hud_font, GAME_OVER_TEXT_SIZE);
    int x = FIELD_X + (FIELD_WIDTH_PX - width) / 2;
    int y = FIELD_Y + (FIELD_HEIGHT_PX - height) / 2;
    fill_rectangle(FIELD_BG_COLOR, x, y, width, height);
    draw_text("GAME OVER", HUD_TEXT_COLOR, game.hud_options.hud_font, GAME_OVER_TEXT_SIZE, x, y);

    if(!game.show_scoreboard) draw_scoreboard_input(game); // we need to draw scoreboard input too
    else draw_scoreboard(game.scoreboard);
}

/* draw entire game */
void draw_game(const game_data &game) {
    draw_field(game);
    draw_hud(game);

    if(game.game_over_filled) draw_game_over(game);
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
            game.score_lvlup = game.score; game.level++; // levelling up based on tetris clearance
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

    /* levelling up based on score */
    if(game.score - game.score_lvlup >= SCORE_LEVEL_UP) {
        game.score_lvlup += SCORE_LEVEL_UP;
        game.level++;
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
                    game.scoreboard = load_scoreboard(); // open database
                    start_reading_text({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, "PLAYER"); // start reading text input (for getting player name)
                    return;
                }

                for(int x = 0; x < FIELD_WIDTH; x++) game.playing_field[row][x] = GAME_OVER_FILL_COLOR; // fill the row
            }
        } else if(reading_text()) {
            /* implement max length limit */
            string player_name = text_input();
            if(player_name.length() > SCOREBOARD_NAME_MAXLEN) {
                end_reading_text();
                start_reading_text({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, player_name.substr(0, SCOREBOARD_NAME_MAXLEN));
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