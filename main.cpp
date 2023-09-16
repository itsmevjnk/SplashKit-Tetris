#include "game.h"
#include "config.h"

int main() {
    window win = open_window(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    clear_window(win, WINDOW_BG_COLOR);

    game_data game = new_game(); // set up new game

    while(!quit_requested()) {
        process_events();
        handle_input(game);
        update_game(game);
        draw_game(game);
    }

    return 0;
}
