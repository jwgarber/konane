#include <errno.h>
#include <ctype.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "main.h"
#include "solve.h"
#include "play.h"

#define str(a) #a
#define xstr(a) str(a)

// TODO
// optimize (eg. smaller moves, smaller boards)
// benchmark, and use this to solve 5x5 and 6x6
// set the size in the makefile, and have different sized boards
// this depends on being able to play from anywhere
// test everything and check it over!
// write README

#define BLACK_CIRCLE "○"
#define WHITE_CIRCLE "●"

// Read a line of input from the given stream.
// This returns a dynamically allocated string that contains the line of input.
static char* readline(FILE* stream) {

    char* line = NULL;
    size_t len = 0;

    errno = 0;
    if (getline(&line, &len, stream) == -1) {
        if (errno == 0) {
            fputs("reading input returned EOF\n", stderr);
        } else {
            perror("getline");
        }
        exit(EXIT_FAILURE);
    }

    return line;
}

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
    printf("\n   ");
    for(int i = 'a'; i < 'a' + SIZE; i++){
        printf(" %c", i);
    }
    printf("\n");
    for(int i = 0; i < SIZE; i++){
		if (i < 10) printf(" ");
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

static void print_score(const char* str, const int32_t score) {

    if (score == WIN) {
        printf("%s score = WIN\n", str);
    } else if (score == LOSE) {
        printf("%s score = LOSE\n", str);
    } else {
        printf("%s score = %i\n", str, score);
    }
}

// Parse a uintmax_t integer with extra error handling
//
// The strtoumax function in the standard library is rather loose in the strings
// it will parse into valid integers. This function tightens those corner cases up.
//
// @param str : string containing an integer to parse
// @param errstr : pointer to a string where an error string (if any) is stored
// @return : On success, returns the parsed uintmax_t integer and sets *errstr to NULL.
//           On failure, 0 is returned, and *errstr points to an error string.
static uintmax_t parse_umax(const char* str, const char** errstr) {

    // The string cannot be empty
    if (*str == '\0') {
        *errstr = "empty string";
        return 0;
    }

    // Must consist entirely of digits
    for (size_t i = 0; str[i] != '\0'; ++i) {
        if (!isdigit(str[i])) {
            *errstr = "non-digit character(s)";
            return 0;
        }
    }

    errno = 0;
    const uintmax_t result = strtoumax(str, NULL, 10);
    if (errno != 0) {
        // strtoumax will fail if the value is outside the range of a uintmax_t
        *errstr = "out of range";
        return 0;
    }

    // Success!
    *errstr = NULL;
    return result;
}

static uintmax_t get_depth(void) {

    while (true) {
        printf("How deep should the computer search on its turn? (integer, default = " xstr(DEPTH) "): ");

        char* line = readline(stdin);
        size_t len = strlen(line);

        if (len == 0 || line[len - 1] != '\n') {
            puts("Invalid depth, please try again.");
            free(line);
            continue;
        }

        if (len == 1) {
            // empty string, so just return the default
            free(line);
            return DEPTH;
        }

        // Chop off the newline
        line[len - 1] = 0;

        const char* errstr = NULL;
        const uintmax_t depth = parse_umax(line, &errstr);
        if (errstr == NULL) {
            free(line);
            // The depth needs to be at least 1
            if (depth == 0) {
                puts("Depth cannot be 0.");
                continue;
            }
            return depth;
        }

        /*fprintf(stdout, "error: unable to parse %s as unsigned integer: %s\n", line, errstr);*/
        puts("Invalid depth, please try again.");
        free(line);
    }
}

static State getUser(void){

    while (true) {
	printf("Do you want to play black or white? (b/w): ");

    char* line = readline(stdin);

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

    char* line = readline(stdin);

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

    int32_t corn_score = negamax(corn_board, BLACK, LOSE, WIN, DEPTH);
    int32_t mid_score = negamax(mid_board, BLACK, LOSE, WIN, DEPTH);

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
// 3 for quit
// 4 for making a move
static int user_move(Move* move, uintmax_t *hint_depth) {

	size_t start_row, end_row;
	char char_start_col, char_end_col;

	while (true) {
		printf("Enter a command for hint or hint depth, solve, quit (h / h # / s / q) or make a move: ");

		char* line = readline(stdin);

        uintmax_t tmp_depth;

		if (sscanf(line, "hint %ju\n", &tmp_depth) == 1 || sscanf(line,"h %ju\n", &tmp_depth) == 1){
            free(line);
            if (tmp_depth == 0) {
                puts("Depth cannot be 0.");
                continue;
            }
            *hint_depth = tmp_depth;
            return 1;
        } else if (strcmp(line, "hint\n") == 0 || strcmp(line, "h\n") == 0) {
            free(line);
            return 1;
		} else if (strcmp(line, "solve\n") == 0 || strcmp(line, "s\n") == 0) {
			free(line);
			return 2;
		} else if (strcmp(line, "quit\n") == 0 || strcmp(line, "q\n") == 0){
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

static bool use_hint(State board[SIZE][SIZE], const Move* move, const State user){

    char* line = readline(stdin);

    if(strcmp(line, "y\n") == 0 || strcmp(line, "yes\n") == 0){
        make_move(board, move, user);
        printBoard(board);
        free(line);
        return true;
    }
    else if(strcmp(line, "n\n") == 0 || strcmp(line, "no\n") == 0){
        free(line);
        return false;
    }
    else{
        free(line);
        puts("Invalid option, please try again.\n");
        return false;
    }
}

int main(int argc, char *argv[]){

	Move move = {};
	State board[SIZE][SIZE];

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

	initBoard(board);
	printBoard(board);

    switch(argc){
        case 1: break;
        case 2:
            if(strcmp(argv[1], "s") == 0 || strcmp(argv[1], "solve") == 0){
                // solveMe ??
            }
            break;
        default:
            fprintf(stderr, "See README for acceptable commands");
            exit(1);
    }

    putchar('\n');
    const State user = getUser();
    uintmax_t depth = get_depth();
    printf("depth = %ju", depth);

    if(user == BLACK){
        user_black(board);
        printBoard(board);
    }

    if (user == WHITE) {
        computer_black(board);
        printBoard(board);

	// The computer then makes a second move
	int32_t score = computer_move(&move, board, !user, depth);

	make_move(board, &move, !user);

	printBoard(board);

    printf("\n Computer move = ");
    print_move(&move);

    print_score(" Computer", score);
    }

    while (true) {

	    // User move
        if (game_over(board, user)){
            puts("Computer won!");
            break;
        }

	putchar('\n');

        while (true) {

            uintmax_t hint_depth = depth;
            const int choice = user_move(&move, &hint_depth);

            if (choice == 1) {
                // hint

                const pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    int32_t score = computer_move(&move, board, user, hint_depth);
                    print_move(&move);
                    print_score("User", score);
                    /*printf("Use this hint? y/n: ");*/

                    /*if (use_hint(board, &move, user)){*/
                        /*return EXIT_SUCCESS;*/
                    /*}*/
                    exit(EXIT_SUCCESS);
                } else {

                    // Ignore SIGINT for now
                    action.sa_handler = SIG_IGN;
                    if (sigaction(SIGINT, &action, NULL) == -1) {
                        perror("sigaction");
                        exit(EXIT_FAILURE);
                    }

                    // Wait for the child to terminate
                    int status;
                    if (wait(&status) == -1) {
                        perror("wait");
                        exit(EXIT_FAILURE);
                    }

                    if (!WIFEXITED(status)) {
                        puts(" cancelled");
                    }

                    // Now restore it again
                    action.sa_handler = SIG_DFL;
                    if (sigaction(SIGINT, &action, NULL) == -1) {
                        perror("sigaction");
                        exit(EXIT_FAILURE);
                    }
                }

            } else if (choice == 2) {
                // solve

                const pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    solve(board, user);
                    exit(EXIT_SUCCESS);
                } else {
                    // Ignore SIGINT for now
                    action.sa_handler = SIG_IGN;
                    if (sigaction(SIGINT, &action, NULL) == -1) {
                        perror("sigaction");
                        exit(EXIT_FAILURE);
                    }

                    // Wait for the child to terminate
                    int status;
                    if (wait(&status) == -1) {
                        perror("wait");
                        exit(EXIT_FAILURE);
                    }

                    if (!WIFEXITED(status)) {
                        puts(" cancelled");
                    }

                    // Now restore it again
                    action.sa_handler = SIG_DFL;
                    if (sigaction(SIGINT, &action, NULL) == -1) {
                        perror("sigaction");
                        exit(EXIT_FAILURE);
                    }
                }

            } else if (choice == 3) {
                // quit
                exit(EXIT_SUCCESS);
            } else {
                // move

                int val = make_move(board, &move, user);

                if (val == 1) break;

                puts("Invalid move, please try again.");
            }
        }

        printBoard(board);

	system("sleep 1");

	if (game_over(board, !user)) {
	    puts("You won!");
	    break;
	}

        // Computer move
        int32_t score = computer_move(&move, board, !user, depth);

	make_move(board, &move, !user);

	printBoard(board);

    // Add extra space for computer
    printf("\n Computer move = ");
    print_move(&move);

    print_score(" Computer", score);
    }
    return EXIT_SUCCESS;
}
