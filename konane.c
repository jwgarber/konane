#include <stdio.h>
#include <stdint.h>

#include "konane.h"
#include "solve.h"

#define WHITE_CIRCLE "○"
#define BLACK_CIRCLE "●"

State board41[SIZE][SIZE] = {{EMPTY, EMPTY, BLACK, WHITE},
                            {WHITE, BLACK, WHITE, BLACK},
                            {BLACK, WHITE, BLACK, WHITE},
                            {WHITE, BLACK, WHITE, BLACK}};

State board42[SIZE][SIZE] = {{BLACK, WHITE, BLACK, WHITE},
                            {WHITE, EMPTY, EMPTY, BLACK},
                            {BLACK, WHITE, BLACK, WHITE},
                            {WHITE, BLACK, WHITE, BLACK}};

State board61[SIZE][SIZE] = {{EMPTY, EMPTY, BLACK, WHITE, BLACK, WHITE},
                             {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK},
                             {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
                             {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK},
                             {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
                             {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK}};

State board62[SIZE][SIZE] = {{BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
                             {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK},
                             {BLACK, WHITE, EMPTY, EMPTY, BLACK, WHITE},
                             {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK},
                             {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
                             {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK}};


static void initBoard(State board[SIZE][SIZE]){
	for(int i = 0; i < SIZE; i++){
		for(int j = 0; j < SIZE; j = j+2){
			if(i%2 == 0){
				board[i][j] = BLACK;
				board[i][j+1] = WHITE;
			}
			else{
				board[i][j] = WHITE;
				board[i][j+1] = BLACK;
			}
		}
	}
}

static void printBoard(const State board[SIZE][SIZE]){
    printf("  ");
    for(int i = 'a'; i < 'a' + SIZE; i++){
        printf(" %c", i);
    }
    printf("\n");
    for(int i = 0; i < SIZE; i++){
		printf("%d", i+1);
        if(i < 9){
            printf(" ");
        }
        for(int j = 0; j < SIZE; j++){
            switch(board[i][j]){
                case BLACK:
                    printf(" %s", BLACK_CIRCLE); break;
                case WHITE:
                    printf(" %s", WHITE_CIRCLE); break;
                case EMPTY:
                    printf(" "); break;
            }
        }
        printf("\n");

    }
}

int main(void){
	State board[SIZE][SIZE];
	initBoard(board);
	printBoard(board);

    printf("\nDo you want to play black or white?\n");
    char user;
    scanf("%c", &user);

    return 1;
}

