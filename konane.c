#include <stdio.h>
#include <stdint.h>

#include "konane.h"
#include "solve.h"

#define WHITE_CIRCLE "○"
#define BLACK_CIRCLE "●"

#if 0
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
#endif

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
    printf(" ");
    for(int i = 'a'; i < 'a' + SIZE; i++){
        printf(" %c", i);
    }
    printf("\n");
    for(int i = 0; i < SIZE; i++){
		printf("%d", i);
        for(int j = 0; j < SIZE; j++){
            switch(board[i][j]){
                case BLACK:
                    printf(" %s", BLACK_CIRCLE); break;
                case WHITE:
                    printf(" %s", WHITE_CIRCLE); break;
                case EMPTY:
                    printf("  "); break;
            }
        }
        printf("\n");
    }
}

static State getUser(void){
    printf("\nDo you want to play black or white? (b/w)\n");
    char user;
    scanf("%s", &user);
    //error handle various entries
    if(user == 'b'){
        return BLACK;
    }
    if(user == 'w'){
        return WHITE;
    }
    else{
        printf("Invalid entry\n");
        return EMPTY;
    }
}

static void user_black(State board[SIZE][SIZE]){
    printf("Do you want you remove from the middle or corner? (m/c): ");
    char start_move;
    scanf(" %c", &start_move);
    if(start_move == 'm'){
        board[2][2] = EMPTY;
        board[2][3] = EMPTY;
        printf("\n");
        printBoard(board);
        printf("\n");
    }
    if(start_move == 'c'){
        board[0][0] = EMPTY;
        board[0][1] = EMPTY;
        printf("\n");
        printBoard(board);
    }

}

static void begin_game(State board[SIZE][SIZE]){
    printf("\nMake a move (from to): ");
    int curr_row, next_row, curr_col, next_col;
    char char_curr_col, char_next_col;
    scanf(" %c%d %c%d", &char_curr_col, &curr_row, &char_next_col, &next_row);
    //error handle illegal moves
    curr_col = char_curr_col - 'a';
    next_col = char_next_col - 'a';
    board[curr_row][curr_col] = EMPTY;
    board[next_row][next_col] = BLACK;

    if(next_col == curr_col){
        int jump = next_row - curr_row;
        if(jump < 0){
            for(int i = 1; i < -jump; i++){
                board[curr_row - i][curr_col] = EMPTY;
            }
        }
        if(jump > 0){
            for(int i = 1; i < jump; i++){
                board[curr_row + i][curr_col] = EMPTY;
            }
        }
    }
    if(next_row == curr_row){
        int jump = next_col - curr_col;
        if(jump < 0){
            for(int i = 1; i < -jump; i++){
                board[curr_row][curr_col - i] = EMPTY;
            }
        }
        if(jump > 0){
            for(int i = 1; i < jump; i++){
                board[curr_row][curr_col + 1] = EMPTY;
            }
        }
    }
    printf("\n");
    printBoard(board);
}


int main(void){
	State board[SIZE][SIZE];
	initBoard(board);
	printBoard(board);
    State user = getUser();
    if(user == BLACK){
        user_black(board);
    }
    begin_game(board);
    return 1;
}


/*
        printf("\nRemove one: ");
        int curr_row;
        char curr_col;
        scanf(" %c%d", &curr_col, &curr_row);
        //error handle incorrect starting move.
        //Possible moves = a0, f5, c2, d3
        board[curr_row][curr_col - 'a'] = EMPTY;
    }
    printf("\n");
    return;
*/
