#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
    const char *actual_filename = "./tests/actual_outputs/test_output.txt";
    int num_tiles_placed;
    GameState *game = initialize_game_state("./tests/boards/board08.txt");
    game = place_tiles(game, 6, 12, 'H', " CANDY", &num_tiles_placed);
    save_game_state(game, actual_filename);
    free_game_state(game);
    return 0;
}
