#include "game.h"
#include "config.h"

int main() {
    window win = open_window(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    clear_window(win, WINDOW_BG_COLOR);

    /* test pieces */
    for(int type = 0; type < 7; type++) {
        for(int rotation = 0; rotation < 4; rotation++) {
            draw_piece({piece_types[type], rotation, {rotation * 4, type * 4}});
            refresh_screen();
            delay(1000);
        }
    }

    
    delay(10000);
    return 0;
}
