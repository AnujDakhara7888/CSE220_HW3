#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)

void changeGameStateByCols(GameState *game,int colNumber,int oldCol,int numColsToBeAdded);
void changeGameStateByRows(GameState *game,int rowNumber,int oldRow,int numRowsToBeAdded);

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
        state->firstTile=0;
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
                    state->firstTile=1;
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
    if(row>=0 && col>=0 && (row<=((game->rows)-1)) && (col<=((game->cols)-1)))
    {
        if(toupper(direction) == 'V' || toupper(direction) == 'H')
        {
            if(game->firstTile == 0)
            {
                FILE* fp = fopen("./tests/words.txt","r");
                int found =0;
                char s[100];
                while(!feof(fp))
                {
                    fscanf(fp,"%s",s);
                    if(!strcasecmp(s,tiles))
                    {
                        found=1;
                        break;
                    }
                }
                if(found==0)
                {
                    fseek(fp, 0, SEEK_SET);
                    int len = strlen(tiles);
                    if(len>0 && ((tiles[len-1]=='s') || (tiles[len-1]=='S')))
                    {
                        char p[len];
                        strncpy(p, tiles, len - 1);
                        p[len-1] = '\0';
                        while(!feof(fp))
                        {
                            fscanf(fp,"%s",s);
                            if(!strcasecmp(s,p))
                            {
                                found=1;
                                break;
                            }
                        }
                    }
                    else
                    {
                        return game;
                    }

                }
                if(found==1)
                { 
                    if(strlen(tiles)>=2)
                    {
                        int length = strlen(tiles);
                        if(toupper(direction)=='H')
                        {
                            if((col + length)>game->cols)
                            {
                                changeGameStateByCols(game,col,game->cols,length);
                            }
                            for(int i=col;i<(col + length);i++)
                            {
                                game->board[row][i][0] = toupper(*(tiles++));
                                game->store[row][i] = 1;
                            }
                            *num_tiles_placed = length;
                        }
                        else
                        {
                            if((row + length)>game->rows)
                            {
                                changeGameStateByRows(game,row,game->rows,length);
                            }
                            for(int i=row;i<(row + length);i++)
                            {
                                game->board[i][col][0] = toupper(*(tiles++));
                                game->store[i][col] = 1;
                            }  
                            *num_tiles_placed=length;
                        }
                        game->firstTile=1;
                        return game;
                    }
                    else
                    {
                        return game;
                    }
                }
            }
            else
            {
                //checking if something is connecting or not
                int checker=0;
                int first=0;
                int touchHorizontal[2];
                int touchVertical[2];
                if(toupper(direction)=='H')
                {
                    int sizeOfTile = strlen(tiles);
                    if((col+sizeOfTile)>game->cols)
                    {
                        sizeOfTile=game->cols-col;
                    }
                    //checking row
                    if((row)>0)
                    {
                        for(int i=col;i<col+sizeOfTile;i++)
                        {
                            if(game->store[row-1][i]>=1)
                            {
                                touchHorizontal[0]=row-1;
                                touchHorizontal[1]=i;
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                    }
                    //checking row+1
                    if(row+1<game->rows)
                    {
                        for(int i=col;i<col+sizeOfTile;i++)
                        {
                            if(game->store[row+1][i]>=1)
                            {
                                touchHorizontal[0]=row+1;
                                touchHorizontal[1]=i;
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                    }
                    //checking left of each element in row
                    if(col>0)
                    {
                        for(int i=col;i<col+sizeOfTile;i++)
                        {
                            if(game->store[row][i-1]>=1 || game->store[row][i]>=1)
                            {
                                if(game->store[row][i]>=1)
                                {
                                    touchHorizontal[0]=row;
                                    touchHorizontal[1]=i;
                                }
                                else
                                {
                                    touchHorizontal[0]=row;
                                    touchHorizontal[1]=i-1;
                                }
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                    }
                    //checking ahead as well just to be sure
                    if(col+1<game->cols)
                    {
                        for(int i=col;i<col+sizeOfTile;i++)
                        {
                            if(game->store[row][i+1]>=1 || game->store[row][i]>=1)
                            {
                                if(game->store[row][i]>=1)
                                {
                                    touchHorizontal[0]=row;
                                    touchHorizontal[1]=i;
                                }
                                else
                                {
                                    touchHorizontal[0]=row;
                                    touchHorizontal[1]=i+1;
                                }
                                first=i;
                                checker=1;
                                break;
                            }
                            if(!((i+1)+1<game->cols))
                            {
                                break;
                            }
                        }
                    }
                    //checkedcolumn
                    if(col==0)
                    {
                        for(int i=(col+1);i<col+sizeOfTile;i++)
                        {
                            if(game->store[row][i-1]>=1 || game->store[row][i]>=1)
                            {
                                if(game->store[row][i]>=1)
                                {
                                    touchHorizontal[0]=row;
                                    touchHorizontal[1]=i;
                                }
                                else
                                {
                                    touchHorizontal[0]=row;
                                    touchHorizontal[1]=i-1;
                                }
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                        if(game->store[row][0]>=1)
                        {
                            touchHorizontal[0]=row;
                            touchHorizontal[1]=0;
                            first=0;
                            checker=1;
                        }
                    }
                }
                else
                {
                    int sizeOfTile = strlen(tiles);
                    if((row+sizeOfTile)>game->rows)
                    {
                        sizeOfTile=game->rows-row;
                    }
                    //checking col-1
                    if((col)>0)
                    {
                        for(int i=row;i<row+sizeOfTile;i++)
                        {
                            if(game->store[i][col-1]>=1)
                            {
                                touchVertical[0]=i;
                                touchVertical[1]=col-1;
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                    }
                    //checking col+1
                    if(col+1<game->cols)
                    {
                        for(int i=row;i<row+sizeOfTile;i++)
                        {
                            if(game->store[i][col+1]>=1)
                            {
                                touchVertical[0]=i;
                                touchVertical[1]=col+1;
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                    }
                    //checking above of each element in col
                    if(row>0)
                    {
                        for(int i=row;i<row+sizeOfTile;i++)
                        {
                            if(game->store[i-1][col]>=1 || game->store[i][col]>=1)
                            {
                                if(game->store[i][col]>=1)
                                {
                                    touchVertical[0]=i;
                                    touchVertical[1]=col;
                                }
                                else
                                {
                                    touchVertical[0]=i-1;
                                    touchVertical[1]=col;
                                }
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                    }
                    //checking ahead as well just to be sure
                    if(row+1<game->rows)
                    {
                        for(int i=row;i<row+sizeOfTile;i++)
                        {
                            if(game->store[i+1][col]>=1 || game->store[i][col]>=1)
                            {
                                if(game->store[i][col]>=1)
                                {
                                    touchVertical[0]=i;
                                    touchVertical[1]=col;
                                }
                                else
                                {
                                    touchVertical[0]=i+1;
                                    touchVertical[1]=col;
                                }
                                first=i;
                                checker=1;
                                break;
                            }
                            if(!((i+1)+1<game->rows))
                            {
                                break;
                            }

                        }
                    }
                    //checkedcolumn
                    if(row==0)
                    {
                        for(int i=(row+1);i<row+sizeOfTile;i++)
                        {
                            if(game->store[i-1][col]>=1 || game->store[i][col]>=1)
                            {
                                if(game->store[i][col]>=1)
                                {
                                    touchVertical[0]=i;
                                    touchVertical[1]=col;
                                }
                                else
                                {
                                    touchVertical[0]=i-1;
                                    touchVertical[1]=col;
                                }
                                first=i;
                                checker=1;
                                break;
                            }
                        }
                        if(game->store[0][col]>=1)
                        {
                            touchVertical[0]=0;
                            touchVertical[1]=col;
                            first=0;
                            checker=1;
                        }
                    }
                }
                if(checker==0)
                {
                    return game;
                }
                //creating word and checking in file
                char word[100];
                char word1[100];
                int length = strlen(tiles);
                int wordIndex=0;
                int wordIndex1=0;
                const char *start = tiles;
                int count;
                int count1;
                int countStackPile=0;
                int countForAllWords=0;
                int tileCounter=0;
                int tileCheck=0;
                while(*tiles)
                {
                    if(*tiles!=' ')
                    {
                        tileCounter++;
                    }
                    tiles++;
                }
                tiles=start;
                if(toupper(direction)=='H')
                {
                    if(row!=touchHorizontal[0])
                    {
                        count1=touchHorizontal[0];
                        //find till .
                        while(count1>0)
                        {
                            if(game->store[count1-1][touchHorizontal[1]]>=1)
                            {
                                count1--;
                            }
                            else
                            {
                                break;
                            }
                        }
                        int traverse=touchHorizontal[0];
                        int j=row;
                        if(touchHorizontal[0]>row)
                        {
                            traverse=row;
                            j=touchHorizontal[0];
                        }
                        //fidning letter connected to our tile
                        tiles=start;
                        char c;
                        for(int i=col;i<=touchHorizontal[1];i++)
                        {
                            c=*tiles;
                            tiles++;
                        }
                        tiles=start;
                        //now start filling from traverse
                        while(traverse<=j)
                        {
                            if(game->store[traverse][touchHorizontal[1]]>=1)
                            {
                                word1[wordIndex1++]=game->board[traverse][touchHorizontal[1]][(game->store[traverse][touchHorizontal[1]])-1];
                            }
                            else if(game->store[traverse][touchHorizontal[1]]==0 && (isalpha(c)))
                            {
                                word1[wordIndex1++]=c;
                            }
                            else
                            {
                                if(game->store[traverse][touchHorizontal[1]]==5)
                                {
                                    *num_tiles_placed=0;
                                    return game;
                                }
                                //change here
                                if(game->store[traverse][touchHorizontal[1]]>=1)
                                {
                                    if(game->board[traverse][touchHorizontal[1]][(game->store[traverse][touchHorizontal[1]])-1] == c)
                                    {
                                        //change here;
                                        *num_tiles_placed=0;
                                        return game;
                                    }
                                }
                                word1[wordIndex1++]=c;
                            }
                            traverse++;
                        }
                        word1[wordIndex1]='\0';
                        int found1 =0;
                        tiles=start;
                        FILE* fpp = fopen("./tests/words.txt","r");
                        char s1[100];
                        while(!feof(fpp))
                        {
                            fscanf(fpp,"%s",s1);
                            if(!strcasecmp(s1,word1))
                            {
                                found1=1;
                                break;
                            }
                        }
                        if(found1==0)
                        {
                            *num_tiles_placed=0;
                            return game;
                        }
                    }
                    if(row==touchHorizontal[0])
                    {
                            tiles=start;
                            for(int i=col;i<touchHorizontal[1];i++)
                            {
                                tiles++;
                            }
                            if(*tiles!=' ' && col<=touchHorizontal[1])
                            {
                                count1=touchHorizontal[0];
                                //find till .
                                while(count1>0)
                                {
                                    if(game->store[count1-1][touchHorizontal[1]]>=1)
                                    {
                                        count1--;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                int traverse=count1;
                                //fidning letter connected to our tile
                                tiles=start;
                                char c;
                                for(int i=col;i<=touchHorizontal[1];i++)
                                {
                                    c=*tiles;
                                    tiles++;
                                }
                                tiles=start;
                                //now start filling from traverse
                                while(traverse<touchHorizontal[0])
                                {
                                    if(game->store[traverse][touchHorizontal[1]]>=1)
                                    {
                                        word1[wordIndex1++]=game->board[traverse][touchHorizontal[1]][(game->store[traverse][touchHorizontal[1]])-1];
                                    }
                                    traverse++;
                                }
                                word1[wordIndex1++]=c;
                                traverse++;
                                //till a . is found
                                if((traverse<game->rows) && (game->store[traverse][touchHorizontal[1]])>=1)
                                {
                                    while((traverse<game->rows) && (game->store[traverse][touchHorizontal[1]])>=1 && isalpha(game->board[traverse][touchHorizontal[1]][(game->store[traverse][touchHorizontal[1]])-1]))
                                    {
                                        word1[wordIndex1++]=game->board[traverse][touchHorizontal[1]][(game->store[traverse][touchHorizontal[1]])-1];
                                        traverse++;
                                    }
                                }
                                word1[wordIndex1]='\0';
                                int found1 =0;
                                tiles=start;
                                FILE* fpp = fopen("./tests/words.txt","r");
                                char s1[100];
                                while(!feof(fpp))
                                {
                                    fscanf(fpp,"%s",s1);
                                    if(!strcasecmp(s1,word1))
                                    {
                                        found1=1;
                                        break;
                                    }
                                }
                                if(found1==0)
                                {
                                    *num_tiles_placed=0;
                                    return game;
                                }
                            }
                    
                    }
                    tiles=start;
                    // if((col + length)>game->cols)
                    // {
                    //     changeGameStateByCols(game,col,game->cols,length);
                    // }
                    int i=0;
                    count=first;
                    //finding elmenet backwards to make word
                    while(count>0)
                    {
                        if(game->store[row][count-1]>=1)
                        {
                            count--;
                        }
                        else
                        {
                            break;
                        }
                    }
                    //goes to element till . is found
                    while(count<first)
                    {
                        if(game->store[row][count]>=1)
                        {
                            word[wordIndex++]=game->board[row][count][(game->store[row][count])-1];
                            countForAllWords++;
                        }
                        count++;
                        //countForAllWords++;
                    }
                    count=wordIndex;
                    int traverse=first;
                    if(traverse>col)
                    {
                        traverse=col;
                    }
                    //once dot found and stored now putting the remaining without space words picked from tiles and our board
                    while(i<length && traverse<game->cols)
                    {
                        if(game->store[row][traverse]>=1 && (*tiles==' '))
                        {
                            countForAllWords++;
                            word[wordIndex++]=game->board[row][traverse][(game->store[row][traverse])-1];
                        }
                        else if(game->store[row][traverse]==0 && (isalpha(*tiles)))
                        {
                            countForAllWords++;
                            word[wordIndex++]=*tiles;
                        }
                        else
                        {
                            if(game->store[row][traverse]==5)
                            {
                                *num_tiles_placed=0;
                                return game;
                            }
                            //change here
                            if(game->store[row][traverse]>=1)
                            {
                                if(game->board[row][traverse][(game->store[row][traverse])-1] == *tiles)
                                {
                                    //change here;
                                    *num_tiles_placed=0;
                                    return game;
                                }
                            }
                            word[wordIndex++]=*tiles;
                            countStackPile++;
                        }
                        i++;
                        tiles++;
                        traverse++;
                        //countForAllWords++;
                    }
                    //storing if more tile there
                    while(traverse<(col+length))
                    {
                        word[wordIndex++]=*tiles;
                        tiles++;
                        countForAllWords++;
                        traverse++;
                        tileCheck=1;
                    }
                    //storing till a . in our word
                    if((traverse<game->cols) && (game->store[row][traverse])>=1)
                    {
                        while((traverse<game->cols) && (game->store[row][traverse])>=1 && isalpha(game->board[row][traverse][(game->store[row][traverse])-1]))
                        {
                            word[wordIndex++]=game->board[row][traverse][(game->store[row][traverse])-1];
                            traverse++;
                            countForAllWords++;
                        }
                    }
                    if(tileCounter!=1)
                    {
                        if(countStackPile==tileCounter && countForAllWords==0)
                        {
                            *num_tiles_placed=0;
                            return game;
                        }
                    }
                }
                else
                {
                    if(col!=touchVertical[1])
                    {
                        count1=touchVertical[1];
                        //find till .
                        while(count1>0)
                        {
                            if(game->store[row][count1-1]>=1)
                            {
                                count1--;
                            }
                            else
                            {
                                break;
                            }
                        }
                        int traverse=count1;
                        int j=col;
                        if(touchHorizontal[1]>col)
                        {
                            traverse=col;
                            j=touchVertical[1];
                        }
                        //fidning letter connected to our tile
                        tiles=start;
                        char c;
                        for(int i=row;i<=touchVertical[0];i++)
                        {
                            c=*tiles;
                            tiles++;
                        }
                        tiles=start;
                        //now start filling from traverse
                        while(traverse<=j)
                        {
                            if(game->store[touchVertical[0]][traverse]>=1)
                            {
                                word1[wordIndex1++]=game->board[touchVertical[0]][traverse][(game->store[touchVertical[0]][traverse])-1];
                            }
                            else if(game->store[touchVertical[0]][traverse]==0 && (isalpha(c)))
                            {
                                word1[wordIndex1++]=c;
                            }
                            else
                            {
                                if(game->store[touchVertical[0]][traverse]==5)
                                {
                                    *num_tiles_placed=0;
                                    return game;
                                }
                                //change here
                                if(game->store[touchVertical[0]][traverse]>=1)
                                {
                                    if(game->board[touchVertical[0]][traverse][(game->store[touchVertical[0]][traverse])-1] == c)
                                    {
                                        //change here;
                                        *num_tiles_placed=0;
                                        return game;
                                    }
                                }
                                word1[wordIndex1++]=c;
                            }
                            traverse++;
                        }
                        word1[wordIndex1]='\0';
                        int found1 =0;
                        tiles=start;
                        FILE* fpp = fopen("./tests/words.txt","r");
                        char s1[100];
                        while(!feof(fpp))
                        {
                            fscanf(fpp,"%s",s1);
                            if(!strcasecmp(s1,word1))
                            {
                                found1=1;
                                break;
                            }
                        }
                        if(found1==0)
                        {
                            *num_tiles_placed=0;
                            return game;
                        }
                    }
                    if(col==touchVertical[1])
                    {
                            tiles=start;
                            for(int i=row;i<touchVertical[0];i++)
                            {
                                tiles++;
                            }
                            if(*tiles!=' ' && row<touchVertical[0])
                            {
                                count1=touchVertical[1];
                                //find till .
                                while(count1>0)
                                {
                                    if(game->store[touchVertical[0]][count1-1]>=1)
                                    {
                                        count1--;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                int traverse=count1;
                                //fidning letter connected to our tile
                                tiles=start;
                                char c;
                                for(int i=row;i<=touchVertical[0];i++)
                                {
                                    c=*tiles;
                                    tiles++;
                                }
                                tiles=start;
                                //now start filling from traverse
                                while(traverse<touchVertical[1])
                                {
                                    if(game->store[touchVertical[0]][traverse]>=1)
                                    {
                                        word1[wordIndex1++]=game->board[touchVertical[0]][traverse][(game->store[touchVertical[0]][traverse])-1];
                                    }
                                    traverse++;
                                }
                                word1[wordIndex1++]=c;
                                traverse++;
                                //till a . is found
                                if((traverse<game->cols) && (game->store[touchVertical[0]][traverse])>=1)
                                {
                                    while((traverse<game->cols) && (game->store[touchVertical[0]][traverse])>=1 && isalpha(game->board[touchVertical[0]][traverse][(game->store[touchVertical[0]][traverse])-1]))
                                    {
                                        word1[wordIndex1++]=game->board[touchVertical[0]][traverse][(game->store[touchVertical[0]][traverse])-1];
                                        traverse++;
                                    }
                                }
                                word1[wordIndex1]='\0';
                                int found1 =0;
                                tiles=start;
                                FILE* fpp = fopen("./tests/words.txt","r");
                                char s1[100];
                                while(!feof(fpp))
                                {
                                    fscanf(fpp,"%s",s1);
                                    if(!strcasecmp(s1,word1))
                                    {
                                        found1=1;
                                        break;
                                    }
                                }
                                if(found1==0)
                                {
                                    *num_tiles_placed=0;
                                    return game;
                                }
                            }
                    
                    }
                    tiles=start;
                    // if((row + length)>game->rows)
                    // {
                    //     changeGameStateByRows(game,row,game->rows,length);
                    // }
                    int i=0;
                    count=first;
                    //finding elmenet backwards to make word
                    while(count>0)
                    {
                        if(game->store[count-1][col]>=1)
                        {
                            count--;
                        }
                        else
                        {
                            break;
                        }
                    }
                    //goes to element till . is found
                    while(count<first)
                    {
                        if(game->store[count][col]>=1)
                        {
                            word[wordIndex++]=game->board[count][col][(game->store[count][col])-1];
                            countForAllWords++;
                        }
                        count++;
                        //countForAllWords++;
                    }
                    count=wordIndex;
                    int traverse=first;
                    if(traverse>row)
                    {
                        traverse=row;
                    }
                    //once dot found and stored now putting the remaining without space words picked from tiles and our board
                    while(i<length && traverse<game->rows)
                    {
                        if(game->store[traverse][col]>=1 && (*tiles==' '))
                        {
                            countForAllWords++;
                            word[wordIndex++]=game->board[traverse][col][(game->store[traverse][col])-1];
                        }
                        else if(game->store[traverse][col]==0 && (isalpha(*tiles)))
                        {
                            countForAllWords++;
                            word[wordIndex++]=*tiles;
                        }
                        else
                        {
                            if(game->store[traverse][col]==5)
                            {
                                *num_tiles_placed=0;
                                return game;
                            }
                            //change here
                            if(game->store[traverse][col]>=1)
                            {
                                if(game->board[traverse][col][(game->store[traverse][col])-1] == *tiles)
                                {
                                    *num_tiles_placed=0;
                                    return game;
                                }
                            }
                            word[wordIndex++]=*tiles;
                            countStackPile++;
                        }
                        traverse++;
                        i++;
                        tiles++;
                        //countForAllWords++;
                    }
                    //storing if more tile there
                    while(traverse<(row+length))
                    {
                        word[wordIndex++]=*tiles;
                        tiles++;
                        countForAllWords++;
                        traverse++;
                        tileCheck=1;
                    }
                    //storing till a . in our word
                    if((traverse<game->rows) && (game->store[traverse][col])>=1)
                    {
                        while((traverse<game->rows) && (game->store[traverse][col])>=1 && isalpha(game->board[traverse][col][(game->store[traverse][col])-1]))
                        {
                            word[wordIndex++]=game->board[traverse][col][(game->store[traverse][col])-1];
                            traverse++;
                            countForAllWords++;
                        }
                    }
                    if(tileCounter!=1)
                    {
                        if(countStackPile==tileCounter && countForAllWords==0)
                        {
                            *num_tiles_placed=0;
                            return game;
                        }
                    }
                }
                word[wordIndex]='\0';
                int found =0;
                tiles=start;
                FILE* fp = fopen("./tests/words.txt","r");
                char s[100];
                while(!feof(fp))
                {
                    fscanf(fp,"%s",s);
                    if(!strcasecmp(s,word))
                    {
                        found=1;
                        break;
                    }
                }
                if(found==0)
                {
                    fseek(fp, 0, SEEK_SET);
                    int len = strlen(word);
                    if(len>0 && (word[len-1]=='s' || (word[len-1]=='S')))
                    {
                        char p[len];
                        strncpy(p, word, len - 1);
                        p[len] = '\0';
                        while(!feof(fp))
                        {
                            fscanf(fp,"%s",s);
                            if(!strcasecmp(s,p))
                            {
                                found=1;
                                break;
                            }
                        }
                    }
                    else
                    {
                        *num_tiles_placed=0;
                        return game;
                    }
                }
                tiles=start;
                if(tileCheck==1)
                {
                    if(toupper(direction)=='H')
                    {
                        changeGameStateByCols(game,col,game->cols,length);
                    }
                    else
                    {
                        changeGameStateByRows(game,row,game->rows,length);
                    }
                }
                if(found==1)
                {
                    int tempLength=(strlen(word))-count;
                    int howMuch=0;
                    int tileCOunt=0;
                    if(toupper(direction)=='H')
                    {
                        for(int i=col;i<(col + tempLength);i++)
                        {
                            if(game->store[row][i]==0 && (isalpha(*tiles)))
                            {
                                tileCOunt++;
                                howMuch++;
                                game->board[row][i][0]=*tiles;
                                game->store[row][i]=1;
                            }
                            else if(game->store[row][i]>=1 && (!isalpha(*tiles)))
                            {
                                howMuch++;
                            }
                            else
                            {
                                tileCOunt++;
                                howMuch++;
                                game->board[row][i][(game->store[row][i])]=*tiles;
                                game->store[row][i]=(game->store[row][i])+1;
                            }
                            if(*tiles=='\0')
                            {
                                break;
                            }
                            tiles++;
                        }
                        *num_tiles_placed = tileCOunt;
                    }
                    else
                    {
                        for(int i=row;i<(row + tempLength);i++)
                        {
                            if(game->store[i][col]==0 && (isalpha(*tiles)))
                            {
                                tileCOunt++;
                                howMuch++;
                                game->board[i][col][0]=*tiles;
                                game->store[i][col]=1;
                            }
                            else if(game->store[i][col]>=1 && (!isalpha(*tiles)))
                            {
                                howMuch++;
                            }
                            else
                            {
                                tileCOunt++;
                                howMuch++;
                                game->board[i][col][(game->store[i][col])]=*tiles;
                                game->store[i][col]=(game->store[i][col])+1;
                            }
                            if(*tiles=='\0')
                            {
                                break;
                            }
                            tiles++;
                        }
                        *num_tiles_placed = tileCOunt;
                    }
                }
                (void)touchVertical;
                fclose(fp);
                return game;
            }
        }
        else
        {
            *num_tiles_placed=0;
            return game;
        }
    }
   
    return game;
    
}
void changeGameStateByCols(GameState *game,int colNumber,int oldCol,int numColsToBeAdded)
{
    int newCol = (colNumber + numColsToBeAdded) - (game->cols);
    game->cols=(game->cols)+newCol;

    for (int i = 0; i<(game->rows); i++) 
    {
        game->board[i] = (char**)realloc(game->board[i], game->cols * sizeof(char*));
        game->store[i] = (int*)realloc(game->store[i], game->cols * sizeof(int));

        for (int j = oldCol; j<(game->cols); j++) 
        {
            game->board[i][j] = (char*)calloc(5, sizeof(char));
        }
    }
    for (int i = 0; i<(game->rows); i++) 
    {
        for (int j = oldCol; j<(game->cols); j++) 
        {
            game->board[i][j][0] = '.';
            game->store[i][j] = 0;
        }
    }
}
void changeGameStateByRows(GameState *game, int rowNumber, int oldRow, int numRowsToBeAdded) {
    int newRow = (rowNumber + numRowsToBeAdded) - game->rows;
    game->rows = (game->rows) +newRow; 

    game->board = (char***)realloc(game->board, game->rows * sizeof(char**));
    game->store = (int**)realloc(game->store, game->rows * sizeof(int*));

    for (int i = oldRow; i < game->rows; i++) {
        game->board[i] = (char**)calloc(game->cols, sizeof(char*));
        game->store[i] = (int*)calloc(game->cols, sizeof(int));
        for (int j = 0; j < game->cols; j++) 
        {
            game->board[i][j] = (char*)calloc(5, sizeof(char));
            game->board[i][j][0] = '.';
            game->store[i][j] = 0; 
        }
    }
}
GameState* undo_place_tiles(GameState *game) {
    (void)game;
    return game;
}

void free_game_state(GameState *game) 
{
    for(int i = 0;i<(game->rows); i++)
    {
        for(int j=0;j<(game->cols);j++)
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
