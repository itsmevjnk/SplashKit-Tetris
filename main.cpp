#include "game.h"
#include "config.h"

void load_resources() {
    load_resource_bundle("resources", "resources.txt");
}

int main() {
    load_resources(); // load resource bundle
    
    window win = open_window(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    clear_window(win, WINDOW_BG_COLOR);

    while(true) {
        game_data game = new_game(); // set up new game

        while(!quit_requested()) {
            /* run game routines until the user stops playing or restarts the game after a game over */
            process_events();
            if(handle_input(game) == false) break; // create new game (i.e. game over)
            update_game(game);
            draw_game(game);
        }

        if(quit_requested()) break; // quit has been requested and it's not just a game over, so we need to exit
    }

    return 0;
}
