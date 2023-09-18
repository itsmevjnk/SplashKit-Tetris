#include "game.h"
#include "config.h"

void load_resources() {
    load_resource_bundle("resources", "resources.txt");
}

int main() {
    load_resources(); // load resource bundle
    
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
