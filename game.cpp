#include "game.h"
#include "piece.h"

using namespace std;

#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))

/* create new game struct */
game_data new_game() {
    game_data result;

    result.score = 0; result.level = 0;
    result.frame_num = 0; result.frame_last_update = 0;
    result.last_input_action = NO_INPUT; result.frame_last_input = 0;

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
    result.hud_options.start_x = HUD_X;
#else
    result.hud_options.start_x = (3 * WINDOW_WIDTH / 4) - (result.hud_options.content_width + 2 * (HUD_PADDING + HUD_BORDER_WIDTH)) / 2;
#endif

#if HUD_Y >= 0
    result.hud_options.start_y = HUD_Y;
#else
    if(result.hud_options.content_height <= WINDOW_WIDTH / 2)
        result.hud_options.start_y = (WINDOW_HEIGHT / 4) - (result.hud_options.content_height + 2 * (HUD_PADDING + HUD_BORDER_WIDTH)) / 2;
    else
        result.hud_options.start_y = (WINDOW_HEIGHT / 2) - (result.hud_options.content_height + 2 * (HUD_PADDING + HUD_BORDER_WIDTH)) / 2;
#endif

    return result;
}

/* check collision between the falling piece and its surrounding field */
#define COLLISION_LEFT          (1 << 0)
#define COLLISION_RIGHT         (1 << 1)
#define COLLISION_BOTTOM        (1 << 2)

uint8_t check_collision(const game_data &game, const piece &test_piece) {
    uint8_t result = 0;
    
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(test_piece.type.bitmaps[test_piece.rotation].bitmap & (1 << (y * 4 + x))) {
                /* there is a cell - go check it out */
                int field_x = test_piece.position.x + x, field_y = test_piece.position.y + y;
                // if(field_y < 0 || field_x < 0) continue; // still out of bounds - nothing to check
                if(field_y == FIELD_HEIGHT - 1 || (field_y >= -1 && game.playing_field[field_y + 1][field_x] != NO_COLOUR)) result |= COLLISION_BOTTOM; // bottom collision
                if(field_x <= 0 || (field_y >= 0 && game.playing_field[field_y][field_x - 1] != NO_COLOUR)) result |= COLLISION_LEFT; // left collision
                if(field_x >= FIELD_WIDTH - 1 || (field_y >= 0 && game.playing_field[field_y][field_x + 1] != NO_COLOUR)) result |= COLLISION_RIGHT; // right collision
            }
        }
    }

    return result;
}

uint8_t check_collision(const game_data &game) {
    return check_collision(game, game.next_pieces[0]);
}

/* detect horizontal collision/overlaps, used for wall kicks */
uint8_t check_horiz_collision_overlap(const game_data &game, const piece &test_piece) {
    uint8_t result = 0;

    /* test left edge */
    int field_x = test_piece.position.x + test_piece.type.bitmaps[test_piece.rotation].x;
    if(field_x < 0) result |= COLLISION_LEFT; // left edge is out of bounds
    else for(int y = MAX(-test_piece.position.y, 0); y < 4 && test_piece.position.y + y < FIELD_HEIGHT; y++) {
        int field_y = test_piece.position.y + y;
        if(game.playing_field[field_y][field_x] != NO_COLOUR) {
            result |= COLLISION_LEFT;
            break;
        }
    }

    /* test right edge */
    field_x += test_piece.type.bitmaps[test_piece.rotation].width - 1;
    if(field_x >= FIELD_WIDTH) result |= COLLISION_RIGHT; // right edge is out of bounds
    else for(int y = MAX(-test_piece.position.y, 0); y < 4 && test_piece.position.y + y < FIELD_HEIGHT; y++) {
        int field_y = test_piece.position.y + y;
        if(game.playing_field[field_y][field_x] != NO_COLOUR) {
            result |= COLLISION_RIGHT;
            break;
        }
    }

    return result;
}

/* handle game inputs */
void handle_input(game_data &game) {
    uint64_t frame_delta = game.frame_num - game.frame_last_input;

    if(key_down(LEFT_KEY) && (game.last_input_action != MOVE_LEFT || frame_delta > FRAME_RATE * SPEED_INPUT_MOVE)) { // move piece to the left
        game.last_input_action = MOVE_LEFT;
        game.frame_last_input = game.frame_num;

        if(!(check_collision(game) & COLLISION_LEFT))
            game.next_pieces[0].position.x--;
#ifdef DEBUG_INPUT_REJECTIONS
        else
            write_line("Left move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
    }

    if(key_down(RIGHT_KEY) && (game.last_input_action != MOVE_RIGHT || frame_delta > FRAME_RATE * SPEED_INPUT_MOVE)) { // move piece to the right
        game.last_input_action = MOVE_RIGHT;
        game.frame_last_input = game.frame_num;

        if(!(check_collision(game) & COLLISION_RIGHT))
            game.next_pieces[0].position.x++;
#ifdef DEBUG_INPUT_REJECTIONS
        else
            write_line("Right move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
    }

    if(key_down(DOWN_KEY) && (game.last_input_action != FORCE_DOWN || frame_delta > FRAME_RATE * SPEED_INPUT_FORCE_DOWN)) { // move piece down
        game.last_input_action = FORCE_DOWN;
        game.frame_last_input = game.frame_num;

        if(!(check_collision(game) & COLLISION_BOTTOM)) {
            game.next_pieces[0].position.y++;
            game.score += SCORE_FORCE_DOWN;
        }
#ifdef DEBUG_INPUT_REJECTIONS
        else
            write_line("Down move rejected for " + piece_to_string(game.next_pieces[0]));
#endif
    }

    if(key_down(UP_KEY) && (game.last_input_action != ROTATE || frame_delta > FRAME_RATE * SPEED_INPUT_ROTATE)) { // rotate piece
        game.last_input_action = ROTATE;
        game.frame_last_input = game.frame_num;

        piece new_piece = game.next_pieces[0]; // rotated piece
        new_piece.rotation = (new_piece.rotation + 1) % 4;

        /* check for overlaps/wall kicks */
        uint8_t check = check_horiz_collision_overlap(game, new_piece);
        if(check != (COLLISION_LEFT | COLLISION_RIGHT)) {
            /* if we have a collision on both the left and right sides, it means our new piece is blocked by other cells */
            bool ok = true; // set if we can place the piece (optionally after wall kicking)

            if(check & COLLISION_LEFT) {
                /* rightward kick and try again */
                new_piece.position.x++;
                ok = (check_horiz_collision_overlap(game, new_piece) == 0);
            } else if(check & COLLISION_RIGHT) {
                /* leftward kick and try again */
                new_piece.position.x--;
                ok = (check_horiz_collision_overlap(game, new_piece) == 0);
            }

            if(ok) game.next_pieces[0] = new_piece; // any problems have been corrected, and we can now place our piece
        }
#ifdef DEBUG_INPUT_REJECTIONS
        else
            write_line("Rotation rejected for " + piece_to_string(game.next_pieces[0]));
#endif
    }
}

/* draw the playing field */
void draw_field(const game_data &game) {
    // draw field border and background
    draw_rectangle(FIELD_BORDER_COLOR, FIELD_X, FIELD_Y, FIELD_WIDTH * (PIECE_SIZE + 2 * PIECE_MARGIN) + 2 * PIECE_MARGIN, FIELD_HEIGHT * (PIECE_SIZE + 2 * PIECE_MARGIN) + 2 * PIECE_MARGIN, option_line_width(FIELD_BORDER_WIDTH));
    fill_rectangle(FIELD_BG_COLOR, FIELD_X + FIELD_BORDER_WIDTH, FIELD_Y + FIELD_BORDER_WIDTH, FIELD_WIDTH * (PIECE_SIZE + 2 * PIECE_MARGIN), FIELD_HEIGHT * (PIECE_SIZE + 2 * PIECE_MARGIN));
    
    // draw the field (minus the falling piece)
    for(int y = 0; y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < FIELD_WIDTH; x++) {
            draw_cell(game.playing_field[y][x], {x, y});
        }
    }

    draw_piece(game.next_pieces[0]); // draw the falling piece
}

/* convert an integer to a string, optionally with zero padding */
string int_to_string(int num, int padding = 0, char pad_char = '0') {
    string result = to_string(num);
    if(padding > 0) result = string(padding - result.length(), pad_char) + result;
    return result;
}

/* HUD content start coordinates - only usable if game_data struct is named game */
#define HUD_CONTENT_X                   (game.hud_options.start_x + HUD_BORDER_WIDTH + HUD_PADDING)
#define HUD_CONTENT_Y                   (game.hud_options.start_y + HUD_BORDER_WIDTH + HUD_PADDING)

/* draw the HUD */
void draw_hud(const game_data &game) {
    // draw HUD border and background
    draw_rectangle(HUD_BORDER_COLOR, game.hud_options.start_x, game.hud_options.start_y, game.hud_options.content_width + 2 * (HUD_BORDER_WIDTH + HUD_PADDING), game.hud_options.content_height + 2 * (HUD_BORDER_WIDTH + HUD_PADDING));
    fill_rectangle(HUD_BG_COLOR, game.hud_options.start_x + HUD_BORDER_WIDTH, game.hud_options.start_y + HUD_BORDER_WIDTH, game.hud_options.content_width + 2 * HUD_PADDING, game.hud_options.content_height + 2 * HUD_PADDING);

    draw_text("SCORE: " + ((HUD_SCORE_WIDTH < HUD_LEVEL_WIDTH) ? string(HUD_LEVEL_WIDTH - HUD_SCORE_WIDTH, ' ') : "") + int_to_string(game.score, HUD_SCORE_WIDTH), HUD_TEXT_COLOR, game.hud_options.hud_font, HUD_TEXT_SIZE, HUD_CONTENT_X, HUD_CONTENT_Y); // display the current score
    draw_text("LEVEL: " + int_to_string(game.level + 1, MAX(HUD_SCORE_WIDTH, HUD_LEVEL_WIDTH), ' '), HUD_TEXT_COLOR, game.hud_options.hud_font, HUD_TEXT_SIZE, HUD_CONTENT_X, HUD_CONTENT_Y + game.hud_options.char_height); // display the current score
    
    // draw next pieces
    int next_str_width = text_width("<< NEXT >>", game.hud_options.hud_font, HUD_TEXT_SIZE); // get width of the text so we can center it
    draw_text("<< NEXT >>", HUD_TEXT_COLOR, game.hud_options.hud_font, HUD_TEXT_SIZE, HUD_CONTENT_X + (game.hud_options.content_width - next_str_width) / 2, HUD_CONTENT_Y + 2.5 * game.hud_options.char_height);
    int next_piece_center_y = HUD_CONTENT_Y + 4 * game.hud_options.char_height + 2 * PIECE_TOTAL_SIZE;
    for(int i = 1; i < NEXT_PIECES_CNT; i++, next_piece_center_y += 4 * PIECE_TOTAL_SIZE) {
        // write_line(to_string(i) + ": " + to_string(piece_width(game.next_pieces[i])) + "x" + to_string(piece_height(game.next_pieces[i])));
        draw_piece(game.next_pieces[i], {(HUD_CONTENT_X + (game.hud_options.content_width - PIECE_TOTAL_SIZE * game.next_pieces[i].type.bitmaps[game.next_pieces[i].rotation].width) / 2), (next_piece_center_y - (PIECE_TOTAL_SIZE * game.next_pieces[i].type.bitmaps[game.next_pieces[i].rotation].height) / 2)}, true, true);
    }
}

/* draw entire game */
void draw_game(const game_data &game) {
    draw_field(game);
    draw_hud(game);

    refresh_screen(FRAME_RATE);
}

/* merge falling piece into playing field */
void merge_piece(game_data &game) {
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(PIECE_ROW(game.next_pieces[0].type.bitmaps[game.next_pieces[0].rotation].bitmap, y) & (1 << x))
                game.playing_field[game.next_pieces[0].position.y + y][game.next_pieces[0].position.x + x] = game.next_pieces[0].type.p_color;
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

/* update game state */
void update_game(game_data &game) {
    uint64_t frame_delta = game.frame_num - game.frame_last_update; // difference from frame number of last update to current frame number

    if((frame_delta > 0) && (frame_delta % (uint64_t)(FRAME_RATE / (SPEED_BASE + game.level * SPEED_STEP)) == 0)) {
        /* it's updating time */
        if(check_collision(game) & COLLISION_BOTTOM) {
            /* falling piece is touching a cell */
            merge_piece(game); // merge piece into the playing field
            next_piece(game); // pop next piece out
            removed_rows rows = remove_full_rows(game); // find and remove full rows
            award_score(game, rows); // then award score to player
        } else {
            game.next_pieces[0].position.y++; // descend falling piece
        }

        game.frame_last_update = game.frame_num;
    }

    game.frame_num++; // advance to next frame
}