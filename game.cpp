#include "game.h"
#include "piece.h"

/* create new game struct */
game_data new_game() {
    game_data result;

    result.score = 0; result.level = 0;
    result.frame_num = 0; result.frame_last_input = 0; result.frame_last_update = 0;

    result.falling_piece = new_piece();
    for(int i = 0; i < 3; i++) result.next_pieces[i] = new_piece();
    
    for(int y = 0; y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < FIELD_WIDTH; x++)
            result.playing_field[y][x] = NO_COLOUR;
    }

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
            if(test_piece.type.bitmaps[test_piece.rotation] & (1 << (y * 4 + x))) {
                /* there is a cell - go check it out */
                int field_x = test_piece.position.x + x, field_y = test_piece.position.y + y;
                if(field_y < 0 || field_x < 0) continue; // still out of bounds - nothing to check
                if(field_y == FIELD_HEIGHT - 1 || game.playing_field[field_y + 1][field_x] != NO_COLOUR) result |= COLLISION_BOTTOM; // bottom collision
                if(field_x == 0 || game.playing_field[field_y][field_x - 1] != NO_COLOUR) result |= COLLISION_LEFT; // left collision
                if(field_x == FIELD_WIDTH - 1 || game.playing_field[field_y][field_x + 1] != NO_COLOUR) result |= COLLISION_RIGHT; // right collision
            }
        }
    }

    return result;
}

uint8_t check_collision(const game_data &game) {
    return check_collision(game, game.falling_piece);
}

/* perform an AND operation between the test piece and the playing field, useful for checking overlaps */
uint16_t piece_and(const game_data &game, const piece &test_piece) {
    uint16_t result = 0;

    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            if(test_piece.type.bitmaps[test_piece.rotation] & (1 << (y * 4 + x))) {
                /* there is a cell - go check it out */
                int field_x = test_piece.position.x + x, field_y = test_piece.position.y + y;
                if(field_y < 0 || field_x < 0) continue; // still out of bounds - nothing to check
                if(game.playing_field[field_y][field_x] != NO_COLOUR) result |= (1 << ((y * 4) + x));
            }
        }
    }

    return result;
}

/* handle game inputs */
void handle_input(game_data &game) {
    uint64_t frame_delta = game.frame_num - game.frame_last_input;
    if(frame_delta != 0 && frame_delta < (FRAME_RATE * SPEED_INPUT)) return; // we need time

    if(key_down(LEFT_KEY)) { // move piece to the left
        game.frame_last_input = game.frame_num;

        if(!(check_collision(game) & COLLISION_LEFT))
            game.falling_piece.position.x--;
    }

    if(key_down(RIGHT_KEY)) { // move piece to the right
        game.frame_last_input = game.frame_num;

        if(!(check_collision(game) & COLLISION_RIGHT))
            game.falling_piece.position.x++;
    }

    if(key_down(DOWN_KEY)) { // move piece down
        game.frame_last_input = game.frame_num;

        if(!(check_collision(game) & COLLISION_BOTTOM)) {
            game.falling_piece.position.y++;
            game.score += SCORE_FORCE_DOWN;
        }
    }

    if(key_down(UP_KEY)) { // rotate piece
        game.frame_last_input = game.frame_num;

        piece new_piece = game.falling_piece; // rotated piece
        new_piece.rotation = (new_piece.rotation + 1) % 4;

        if(piece_and(game, new_piece) == 0)
            game.falling_piece = new_piece; // no overlaps, so we can swap out the piece for the rotated one
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

    draw_piece(game.falling_piece); // draw the falling piece
}

/* draw entire game */
void draw_game(const game_data &game) {
    draw_field(game);

    refresh_screen(FRAME_RATE);
}

/* merge falling piece into playing field */
void merge_piece(game_data &game) {
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(PIECE_ROW(game.falling_piece.type.bitmaps[game.falling_piece.rotation], y) & (1 << x))
                game.playing_field[game.falling_piece.position.y + y][game.falling_piece.position.x + x] = game.falling_piece.type.p_color;
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
    game.falling_piece = game.next_pieces[0]; // pop next piece out
    for(int i = 1; i < 3; i++) game.next_pieces[i - 1] = game.next_pieces[i]; // push pieces
    game.next_pieces[2] = new_piece(); // add new piece to queue
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
            game.falling_piece.position.y++; // descend falling piece
        }

        game.frame_last_update = game.frame_num;
    }

    game.frame_num++; // advance to next frame
}