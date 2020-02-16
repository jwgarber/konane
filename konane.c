#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "konane.h"
#include "solve.h"
#include "play.h"

#define WHITE_CIRCLE "○"
#define BLACK_CIRCLE "●"

static void initBoard(State board[SIZE][SIZE]){
	for(int i = 0; i < SIZE; i++){
		for(int j = 0; j < SIZE; j = j+2){
			if(i%2 == 0){
				board[i][j] = BLACK;
				board[i][j+1] = WHITE;
			}else{
				board[i][j] = WHITE;
				board[i][j+1] = BLACK;
			}
		}
	}
}

static void printBoard(const State board[SIZE][SIZE]){
    system("clear");
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
    printf("\nDo you want to play black or white? (b/w): ");
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
        board[-1 + SIZE/2][-1 + SIZE/2] = EMPTY;
        board[-1 + SIZE/2][SIZE/2] = EMPTY;
        printBoard(board);
    }
    if(start_move == 'c'){
        board[0][0] = EMPTY;
        board[0][1] = EMPTY;
        printBoard(board);
    }
}

static bool game_over(const State board[SIZE][SIZE], const State player){
    for(size_t i = 0; i < SIZE; ++i){
        for(size_t j = 0; j < SIZE; ++j){
            const State color = board[i][j];
            if(color != player) continue;
            if(i >=2 && board[i-1][j] == !color && board[i-2][j] == EMPTY) return true;
            if(j < SIZE - 2 && board[i][j+1] == !color && board[i][j+2] == EMPTY) return true;
            if(j >= 2 && board[i][j-1] == !color && board[i][j-2] == EMPTY) return true;
            if(i < SIZE - 2 && board[i+1][j] == !color && board[i+2][j] == EMPTY) return true;
        }
    }
    return false;
}

static int user_move(Move* move) {
    while (true) {
        printf("\nMake a move (from to): ");
        size_t start_row, end_row;
        char char_start_col, char_end_col;
        scanf(" %c%zu %c%zu", &char_start_col, &start_row, &char_end_col, &end_row);
        size_t start_col = (size_t) (char_start_col - 'a'), end_col = (size_t) (char_end_col - 'a');
        move->start_row = start_row;
        move->end_row = end_row;
        move->start_col = start_col;
        move->end_col = end_col;
    }
}

static int make_move(State board[SIZE][SIZE], const Move* move, const State color) {
    // diagonal move
    if(move->start_row != move->end_row){
            if(move->start_col != move->end_col) return -1;
    }

    // coordinate bounds check
    if (0 > (int) move->start_row || SIZE < (int) move->start_row ||
            0 > (int) move->end_row || SIZE < (int) move->end_row ||
            0 > (int) move->start_col || SIZE < (int) move->start_col ||
            0 > (int) move->end_col || SIZE < (int) move->end_col){
        return -1;
    }

    if (board[move->start_row][move->start_col] == color){
        State tmpboard[SIZE][SIZE];
        board_copy(tmpboard, board);
        // Verticle move
        if(move->end_col == move->start_col){
            int jump = (int) move->end_row - (int) move->start_row;

            if (abs(jump) < 2 || abs(jump) > SIZE) return -1;
            else{
                if(jump < 0){
                    // up move
                    for(size_t i = move->start_row; i > move->end_row; i-=2){
                        if(tmpboard[i-1][move->start_col] != !color) return -1;
                        if(tmpboard[i-2][move->start_col] != EMPTY) return -1;
                        else{
                            tmpboard[i][move->start_col] = EMPTY;
                            tmpboard[i-1][move->start_col] = EMPTY;
                            tmpboard[i-2][move->start_col] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return 1;
                }
                if(jump > 0){
                    // down move
                    for(size_t i = move->start_col; i < move->end_col; i += 2){
                        if(tmpboard[i+1][move->start_col] != !color) return -1;
                        if(tmpboard[i+2][move->start_col] != EMPTY) return -1;
                        else{
                            tmpboard[i][move->start_col] = EMPTY;
                            tmpboard[i+1][move->start_col] = EMPTY;
                            tmpboard[i+2][move->start_col] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return 1;
                }
            return -1;
            }
        }
       // Horizontal move
        if(move->end_row == move->start_row){
            int jump = (int) move->end_col - (int) move->start_col;

            if (abs(jump) < 2 || abs(jump) > SIZE) return -1;
            else{
                if(jump < 0){
                    // left move
                    for(size_t i = move->start_col; i > move->end_col; i-=2){
                        if(tmpboard[move->start_row][i-1] != !color) return -1;
                        if(tmpboard[move->start_row][i-2] != EMPTY) return -1;
                        else{
                            tmpboard[move->start_row][i] = EMPTY;
                            tmpboard[move->start_row][i-1] = EMPTY;
                            tmpboard[move->start_row][i-2] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return 1;
                }
                if(jump > 0){
                    // right move
                    for(size_t i = move->start_col; i < move->end_col; i += 2){
                        if(tmpboard[move->start_row][i+1] != !color) return -1;
                        if(tmpboard[move->start_row][i+2] != EMPTY) return -1;
                        else{
                            tmpboard[move->start_row][i] = EMPTY;
                            tmpboard[move->start_row][i+1] = EMPTY;
                            tmpboard[move->start_row][i+2] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return 1;
                }
                return -1;
            }
        }
        else return -1;
    }
    else return -1;
}

int main(void){
	State board[SIZE][SIZE];
	initBoard(board);
	printBoard(board);
    State user = getUser();
    if(user == BLACK){
        user_black(board);
    }

    Move move = {};

    while (true) {
        if (game_over(board, BLACK)){
            puts("Computer won!");
            break;
        }

        while (true) {
            user_move(&move);
            int val = make_move(board, &move, user);

            if (val == -1) {
                printf("Invlid move\n");
                continue;
            } else break;
        }

        // Computer move
        // In the following function call, i changed WHITE to !user  - Komal
        int64_t score = computer_move(&move, board, !user);

        if (score == LOOSE) {
            puts("You won!");
            break;
        }

        printBoard(board);
        system("sleep 1");
    }
    return 1;
}
