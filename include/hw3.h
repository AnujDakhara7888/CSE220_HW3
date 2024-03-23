#include <stdlib.h>

typedef struct GameState
{
    char ***board;
    int **store;
    int rows;
    int cols;
    int firstTile;
    int onBoardThere;
    int mainCheck;
    struct TileChange *changes;
} GameState;

typedef struct TileChange {
    int row;
    char dir;
    int numRowsToBeRemoved;
    int numColsToBeRemoved;
    int col;
    const char *wordPutIn;
} TileChange;

GameState* initialize_game_state(const char *filename);
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
GameState* undo_place_tiles(GameState *game);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);
