#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
    printf("\n ");
    for(int i = 'a'; i < 'a' + SIZE; i++){
        printf(" %c", i);
    }
    printf("\n");
    for(int i = 0; i < SIZE; i++){
		printf(" %d", i);
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
    putchar('\n');
    while (true) {
	printf("Do you want to play black or white? (b/w): ");

	    char* line = NULL;
	    size_t len = 0;
	getline(&line, &len, stdin);

	if(strcmp(line, "b\n") == 0) {
		free(line);
	    return BLACK;
	}
	if(strcmp(line, "w\n") == 0){
		free(line);
        return WHITE;
	}

	puts("Invalid option, please try again.");
	free(line);
    }
}

static void user_black(State board[SIZE][SIZE]){
    while (true) {
	printf("Do you want to remove from the middle or corner? (m/c): ");

	char* line = NULL;
	size_t len = 0;
	getline(&line, &len, stdin);

	if(strcmp(line, "m\n") == 0){
	    board[-1 + SIZE/2][-1 + SIZE/2] = EMPTY;
	    board[-1 + SIZE/2][SIZE/2] = EMPTY;
	    free(line);
	    return;
	}

	if(strcmp(line, "c\n") == 0){
	    board[0][0] = EMPTY;
	    board[0][1] = EMPTY;
	    free(line);
	    return;
	}
	puts("Invalid option, please try again.");
	free(line);
    }
}

static void computer_black(State board[SIZE][SIZE]) {
    State corn_board[SIZE][SIZE], mid_board[SIZE][SIZE];
    board_copy(corn_board, board);
    board_copy(mid_board, board);

    corn_board[0][0] = EMPTY;
    corn_board[0][1] = EMPTY;

    mid_board[-1 + SIZE/2][-1 + SIZE/2] = EMPTY;
    mid_board[-1 + SIZE/2][SIZE/2] = EMPTY;

    int32_t corn_score = negamax(corn_board, BLACK, LOOSE, WIN, DEPTH);
    int32_t mid_score = negamax(mid_board, BLACK, LOOSE, WIN, DEPTH);

    if(corn_score > mid_score) board_copy(board, corn_board);
    else board_copy(board, mid_board);
}

static bool game_over(const State board[SIZE][SIZE], const State player){
    for(size_t i = 0; i < SIZE; ++i){
        for(size_t j = 0; j < SIZE; ++j){
            const State color = board[i][j];
            if(color != player) continue;
            if(j >= 2 && board[i][j-1] == !color && board[i][j-2] == EMPTY) return false;
            if(i >= 2 && board[i-1][j] == !color && board[i-2][j] == EMPTY) return false;
            if(j < SIZE - 2 && board[i][j+1] == !color && board[i][j+2] == EMPTY) return false;
            if(i < SIZE - 2 && board[i+1][j] == !color && board[i+2][j] == EMPTY) return false;
        }
    }
    return true;
}

// The return value indicates what input the user made
// 1 for hint
// 2 for solve
// 3 for making a move
static int user_move(Move* move) {

	size_t start_row, end_row;
	char char_start_col, char_end_col;

	while (true) {
		printf("Enter a command (hint, solve, quit) or make a move: ");

		char* line = NULL;
		size_t len = 0;
		getline(&line, &len, stdin);

		if (strcmp(line, "hint\n") == 0) {
			free(line);
			return 1;
		} else if (strcmp(line, "solve\n") == 0) {
			free(line);
			return 2;
		} else if (strcmp(line, "quit\n") == 0){
            puts("You quit!");
            free(line);
            return 3;
        } else if (sscanf(line, "%c%zu %c%zu", &char_start_col, &start_row, &char_end_col, &end_row) == 4) {

			if ('a' <= char_start_col && char_start_col < 'a' + SIZE &&
			    'a' <= char_end_col && char_end_col < 'a'+ SIZE &&
			    start_row < SIZE && end_row < SIZE) {

				size_t start_col = (size_t) (char_start_col - 'a');
				size_t end_col = (size_t) (char_end_col - 'a');

				move->start_row = start_row;
				move->end_row = end_row;
				move->start_col = start_col;
				move->end_col = end_col;
				free(line);
				return 4;
			}
		}
		puts("Invalid option, please try again.");
		free(line);
	}
}

static void print_move(const Move* move){
    char start_col, end_col;
    start_col = 'a' + (char) move->start_col;
    end_col = 'a' + (char) move->end_col;
    printf("%c%zu %c%zu\n", start_col, move->start_row, end_col, move->end_row);
}

static int make_move(State board[SIZE][SIZE], const Move* move, const State color) {
    // diagonal move
    if(move->start_row != move->end_row){
            if(move->start_col != move->end_col) return -1;
    }

    // coordinate bounds check
    if (0 > (int) move->start_row || SIZE <= (int) move->start_row ||
            0 > (int) move->end_row || SIZE <= (int) move->end_row ||
            0 > (int) move->start_col || SIZE <= (int) move->start_col ||
            0 > (int) move->end_col || SIZE <= (int) move->end_col){
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
                    for(size_t i = move->start_row; i < move->end_row; i += 2){
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
	Move move = {};
	State board[SIZE][SIZE];
	initBoard(board);
	printBoard(board);

    const State user = getUser();

    if(user == BLACK){
        user_black(board);
        printBoard(board);
    }

    if (user == WHITE) {
        computer_black(board);
        printBoard(board);

	    // The computer then makes a second move
	int32_t score = computer_move(&move, board, !user);

	make_move(board, &move, !user);

	printBoard(board);

	printf("Computer score = %i\n", score);
    }

    while (true) {

	    // User move
        if (game_over(board, user)){
            puts("Computer won!");
            break;
        }

	putchar('\n');

        while (true) {
            const int choice = user_move(&move);
            if (choice == 1) {
                computer_move(&move, board, user);
                print_move(&move);
            } else if (choice == 2) {
                // solve
                // // will implement this later
                const int32_t score = solve_negamax(board, user, LOSE, WIN);
                printf("Solve score = %i\n", score);
                // print the moves made in solve
                // ^ make a list.


            } else if (choice == 3) {
                exit(0);
            } else {

                int val = make_move(board, &move, user);

                if (val == 1) break;

                puts("Invalid move, please try again.");

            }
        }

        printBoard(board);

	/*system("sleep 1");*/

	if (game_over(board, !user)) {
	    puts("You won!");
	    break;
	}

        // Computer move
        int32_t score = computer_move(&move, board, !user);

	make_move(board, &move, !user);

	printBoard(board);

	printf("Computer score = %i\n", score);
    }
    return EXIT_SUCCESS;
}
