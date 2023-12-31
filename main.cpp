#include "game.h"
#include "title.h"
#include "settings.h"
#include "config.h"

/**
 * @brief Load resource bundle.
 * 
 */
void load_resources() {
    load_resource_bundle("resources", "resources.txt");
}

/**
 * @brief The main function.
 * 
 * @return int The program's return value.
 */
int main() {
    load_resources(); // load resource bundle
    
    open_window(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);

    bool game_started = false; // set when the game has started (i.e. no longer at title screen)
    game_data game;

    json settings = load_settings(); // load settings from JSON file

    title_data title = new_title(settings);

    while(true) {
        while(!quit_requested()) {
            /* run game routines until the user stops playing or restarts the game after a game over */
            process_events();
            
            if(!game_started) {
                /* title screen */
                // game_started = true; // TODO: add title screen
                game_started = handle_title_input(title);
                if(game_started) {
                    set_level(settings, title.level); // save level setting for persistence
                    game = new_game(settings); // set up new game
                } else {
                    update_title(title);
                    draw_title(title);
                }
            } else {
                /* the actual game */
                game_started = handle_game_input(game);
                if(!game_started) {
                    title = new_title(settings); // reinitialise title
                    break; // get back to title screen (i.e. game over)
                }
                update_game(game);
                draw_game(game);
            }

            refresh_screen(FRAME_RATE);
        }

        if(quit_requested()) break; // quit has been requested and it's not just a game over, so we need to exit
    }

    save_settings(settings); // commit changes to settings JSON file

    return 0;
}
