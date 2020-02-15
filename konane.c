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
    // Clear the screen in between.
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

static void begin_game(State board[SIZE][SIZE]){
    printf("\nMake a move (from to): ");
    int curr_row, next_row, curr_col, next_col;
    char char_curr_col, char_next_col;
    scanf(" %c%d %c%d", &char_curr_col, &curr_row, &char_next_col, &next_row);
    //error handle illegal moves
    curr_col = char_curr_col - 'a';
    next_col = char_next_col - 'a';
    if(next_col == curr_col){ // up down
        int jump = next_row - curr_row;
        for(int i = 1; i < abs(jump); i++){
            if(jump < 0){ // up
                    if(board[curr_row - 1][curr_col] != WHITE || jump > -2){
                        printf("Illegal move: up\n");
                       // begin_game(board);
                    }
                    else{
                        board[curr_row - i][curr_col] = EMPTY;
                    }
                }
                if(jump > 0){ // down
                    if(board[curr_row + 1][curr_col] != WHITE || jump < 2){
                        printf("Illegal move: down\n");
                      //  begin_game(board);
                    }
                    else{
                        board[curr_row + i][curr_col] = EMPTY;
                    }
                }
        }
    }
    if(next_row == curr_row){ // left right
        int jump = next_col - curr_col;
        for(int i = 1; i < abs(jump); i++){
                if(jump < 0){ // left
                    if(board[curr_row][curr_col - 1] != WHITE || jump > -2){
                        printf("Illegal move: left\n");
                      //  begin_game(board);
                    }
                    else{
                        board[curr_row][curr_col - i] = EMPTY;
                    }
                }
                if(jump > 0){ // right
                    if(board[curr_row][curr_col + 1] != WHITE || jump < 2){
                        printf("Illegal move: right\n");
                      //  begin_game(board);
                    }
                    else{
                        board[curr_row][curr_col + i] = EMPTY;
                    }
                }
        }
    }
    board[curr_row][curr_col] = EMPTY;
    board[next_row][next_col] = BLACK;
    printBoard(board);
}

static void user_move(Move* move) {

    while (true) {
        printf("\nMake a move (from to): ");
        char char_start_col, char_end_col;
        scanf(" %c%zu %c%zu", &char_start_col, &move->start_row, &char_end_col, &move->end_row);
        move->start_col = (size_t) char_start_col - 'a';
        move->end_col = (size_t) char_end_col - 'a';
        if (0 <= (int) move->start_row && SIZE > (int) move->start_row && 0 <= (int) move->end_row && SIZE > (int) move->end_row &&
                0 <= (int) move->start_col && SIZE > (int) move->start_col && 0 <= (int) move->end_col && SIZE > (int) move->end_col) return;
        else{
            printf("Re-enter move\n");
        }
    }
}

// Try to make the current move
static int make_move(State board[SIZE][SIZE], const Move* move, const State color) {

    // Here, the player 'color' is trying to make a new move
    // We try to make the new move, and also check if the move is valid.
    // First, check if the color of the player making the move actually matches
    // the color of the stone at the start position (start_row, start_col)

    // If so, then on the temporary board, start iterating to the new position
    // given by (end_row, end_col).

    // If at any point, the jump doesn't work (eg. we are jumping over a stone
    // of the wrong color), then return an error (-1)

    // If making the move succeeds, then copy the temporary board to the actual board

    State tmpboard[SIZE][SIZE];
    board_copy(tmpboard, board);

    // If the move is up

    // If the move is right

    // If the move is left

    // If the move is down

    // If none of those, then an error

    // Otherwise, everything is good, so copy the board back.
    board_copy(board, tmpboard);
    return 1;
}

bool game_over(const State board[SIZE][SIZE], const State player){
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
        if (game_over(board, BLACK)) {
            puts("Computer won!");
            break;
        }

        // This function needs to be split into user_move() and make_move()
        /*begin_game(board);*/

        // Try to get a user move
        while (true) {

            // Read in the move from the user.
            user_move(&move);

            int val = make_move(board, &move, user);

            if (val == -1) {
                // User made an invalid move, so print a message telling them that
                // On the next iteration of the loop they can try again
                printf("");
            } else {
                // Otherwise the move is good, so break out of the loop
                break;
            }
        }

        // Now the computer makes a move

        // Computer move
        int64_t score = computer_move(&move, board, WHITE);

        if (score == LOOSE) {
            puts("You won!");
            break;
        }

        // print the computer move
        printBoard(board);

        system("sleep 1");
    }
    return 1;
}
