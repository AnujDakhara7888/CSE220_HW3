#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)

GameState* initialize_game_state(const char *filename) {
    FILE* fp = fopen(filename,"r");
    if(fp==NULL)
    {
        return NULL;
    }
    else
    {
        int rowCount=0;
        int colCount=0;
        char c;
        
        GameState *state = (GameState*)(malloc(sizeof(GameState)));
        int make = 0;
        while ((c = fgetc(fp)) != EOF) 
        {
            if (c == '\n') 
            {
                make=1;
                rowCount++;
            } 
            else 
            {
                if(make==0)
                {
                    colCount++; 
                }
            }
        }
        state->rows=rowCount;
        state->cols=colCount;
        
        state->board = (char***)malloc((state->rows) * sizeof(char**));
        state->store = (int**)malloc((state->rows) * sizeof(int*));
        for(int i=0;i<rowCount;i++)
        {
            state->board[i] = (char**)malloc((state->cols) * sizeof(char*));
            state->store[i] = (int*)malloc((state->cols) * sizeof(int));
            for(int j=0;j<colCount;j++)
            {
                state->board[i][j] = (char*)calloc(5, sizeof(char));
            }
        }
        fseek(fp, 0, SEEK_SET);
        // for(int i=0;i<rowCount;i++)
        // {
        //     for(int j=0;j<2;j++)
        //     {
        //         fscanf(fp,"%c",&c);
        //         state->board[i][j][0]=c;
        //         printf("%d %d %c\n",i,j,c);
        //         // printf("%c ",state->board[i][j][0]);
        //         if(c=='.')
        //         {
        //             state->store[i][j]=0;
        //         }
        //         else
        //         {
        //             state->store[i][j]=1;
        //         }
        //     }
        // }
        int currentRow = 0, currentCol = 0;

        while ((c = fgetc(fp)) != EOF && currentRow < rowCount) {
            if (c == '\n') 
            {
                currentRow++;
                currentCol = 0; 
            } 
            else 
            {
                state->board[currentRow][currentCol][0] = c;
                if (c == '.') 
                {
                    state->store[currentRow][currentCol] = 0;
                }
                else 
                {
                    state->store[currentRow][currentCol] = 1;
                }

                currentCol++; 
            }
        }
        fclose(fp);
        return state;
    }
    
}

GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    (void)game;
    (void)row;
    (void)col;
    (void)direction;
    (void)tiles;
    (void)num_tiles_placed;
    return game;
}

GameState* undo_place_tiles(GameState *game) {
    (void)game;
    return game;
}

void free_game_state(GameState *game) 
{
    for(int i = 0; i < game->rows; i++)
    {
        for(int j = 0; j < game->cols; j++)
        {
            free(game->board[i][j]);
        }

        free(game->board[i]);
        free(game->store[i]);
    }
    free(game->board);
    free(game->store);

    free(game);
}

void save_game_state(GameState *game, const char *filename) 
{
    FILE* f = fopen(filename,"w");
    for(int i=0; i<(game->rows); i++)
    {
        for(int j=0; j<(game->cols); j++)
        {
            if((game->store[i][j])==0)
            {
                fprintf(f,"%c", game->board[i][j][0]);
            }
            else
            {
                fprintf(f,"%c", game->board[i][j][(game->store[i][j])-1]);
            }
        }
        fprintf(f,"\n");
    }
    for(int i=0; i<(game->rows); i++)
    {
        for(int j=0; j<(game->cols); j++)
        {
            fprintf(f,"%d",game->store[i][j]);
        }
        fprintf(f,"\n");
    }
    fclose(f);
}
