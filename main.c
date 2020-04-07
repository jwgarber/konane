#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "main.h"
#include "play.h"
#include "solve.h"

#define str(a) #a
#define xstr(a) str(a)

// benchmark, and use this to solve 5x5 and 6x6
// test everything and check it over!
// write README
// figure out sleep stuff too

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

static void initBoard(State board[Y][X]) {
    for (int32_t i = 0; i < Y; i++) {
        for (int32_t j = 0; j < X; j++) {
            if ((i + j) % 2 == 0) {
                board[i][j] = BLACK;
            } else {
                board[i][j] = WHITE;
            }
        }
    }
}

static void printBoard(const State board[Y][X]) {
    system("clear");
    printf("\n  ");
    if (Y > 10) {
        putchar(' ');
    }

    for (int32_t i = 'a'; i < 'a' + X; i++) {
        printf(" %c", i);
    }

    printf("\n");
    for (int32_t i = 0; i < Y; i++) {
        if (Y > 10 && i < 10) putchar(' ');
        printf(" %d", i);
        for (int32_t j = 0; j < X; j++) {
            switch (board[i][j]) {
            case BLACK:
                printf(" %s", BLACK_CIRCLE);
                break;
            case WHITE:
                printf(" %s", WHITE_CIRCLE);
                break;
            case EMPTY:
                printf("  ");
                break;
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
        printf("How deep should the computer search on its turn? (default = " xstr(DEPTH) "): ");

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

static State getUser(void) {

    while (true) {
        printf("Do you want to play black or white? (b/w): ");

        char* line = readline(stdin);

        if (strcmp(line, "b\n") == 0) {
            free(line);
            return BLACK;
        }
        if (strcmp(line, "w\n") == 0) {
            free(line);
            return WHITE;
        }

        puts("Invalid option, please try again.");
        free(line);
    }
}

// User goes first
static void user_black(State board[Y][X], const uintmax_t depth) {

    int32_t row;
    int32_t col;

    while (true) {
        printf("Select a stone to remove: ");

        char* line = readline(stdin);

        char char_col;
        if (sscanf(line, "%c%i", &char_col, &row) == 2) {

            if ('a' <= char_col && char_col < 'a' + X && row < Y) {

                col = char_col - 'a';

                if (board[row][col] == BLACK) {
                    board[row][col] = EMPTY;
                    free(line);
                    break;
                }
            }
        }
        puts("Invalid option, please try again.");
        free(line);
    }

    printBoard(board);

    // Now the computer investigates each of the (possibly four) moves to make.
    // If the board is a 1x1, then there are no moves to make, so the user wins.
    if (X == 1 && Y == 1) {
        puts("\n You won!");
        exit(EXIT_SUCCESS);
    }

    system("sleep 1");

    State tmpboard[Y][X];

    int32_t score = LOSE;
    int32_t a = LOSE;
    int32_t b = WIN;

    int32_t comp_row = 0;
    int32_t comp_col = 0;

    // Move up
    if (row != 0) {
        board_copy(tmpboard, board);
        tmpboard[row - 1][col] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val >= score) {
            score = val;
            comp_row = row - 1;
            comp_col = col;
        }

        if (score > a)
            a = score;
        if (a >= b)
            goto done;
    }

    // Move right
    if (col < X - 1) {
        board_copy(tmpboard, board);
        tmpboard[row][col + 1] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val >= score) {
            score = val;
            comp_row = row;
            comp_col = col + 1;
        }

        if (score > a)
            a = score;
        if (a >= b)
            goto done;
    }

    // Move left
    if (col != 0) {
        board_copy(tmpboard, board);
        tmpboard[row][col - 1] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val >= score) {
            score = val;
            comp_row = row;
            comp_col = col - 1;
        }

        if (score > a)
            a = score;
        if (a >= b)
            goto done;
    }

    // Move down
    if (row < Y - 1) {
        board_copy(tmpboard, board);
        tmpboard[row + 1][col] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val >= score) {
            score = val;
            comp_row = row + 1;
            comp_col = col;
        }

        if (score > a)
            a = score;
        if (a >= b)
            goto done;
    }

done:

    board[comp_row][comp_col] = EMPTY;

    printBoard(board);

    printf("\n Computer move = %c%i\n", (char)(comp_col + 'a'), comp_row);
    print_score(" Computer", score);
}

static void computer_black(State board[Y][X], const uintmax_t depth) {

    State tmpboard[Y][X];

    int32_t score = LOSE;
    int32_t a = LOSE;
    int32_t b = WIN;

    int32_t row = 0;
    int32_t col = 0;

    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {

            if (board[i][j] != BLACK) continue;

            board_copy(tmpboard, board);
            tmpboard[i][j] = EMPTY;

            int32_t val = -second_turn_search(tmpboard, row, col, -b, -a, depth - 1);

            if (val >= score) {
                score = val;
                row = i;
                col = j;
            }

            if (score > a)
                a = score;
            if (a >= b)
                goto done;
        }
    }

done:

    board[row][col] = EMPTY;

    printBoard(board);

    printf("\n Computer move = %c%i\n", (char)(col + 'a'), row);
    print_score(" Computer", score);

    putchar('\n');

    if (X == 1 && Y == 1) {
        puts(" Computer won!");
        exit(EXIT_SUCCESS);
    }

    int32_t user_row;
    int32_t user_col;

    while (true) {
        printf("Select a stone to remove: ");

        char* line = readline(stdin);

        char char_col;
        if (sscanf(line, "%c%i", &char_col, &user_row) == 2) {

            if ('a' <= char_col && char_col < 'a' + X && row < Y) {

                user_col = char_col - 'a';

                if (board[user_row][user_col] == WHITE) {
                    board[user_row][user_col] = EMPTY;
                    free(line);
                    break;
                }
            }
        }
        puts("Invalid option, please try again.");
        free(line);
    }

    printBoard(board);
}

static bool game_over(const State board[Y][X], const State player) {
    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {
            const State color = board[i][j];
            if (color != player) continue;
            if (j >= 2 && board[i][j - 1] == !color && board[i][j - 2] == EMPTY) return false;
            if (i >= 2 && board[i - 1][j] == !color && board[i - 2][j] == EMPTY) return false;
            if (j < X - 2 && board[i][j + 1] == !color && board[i][j + 2] == EMPTY) return false;
            if (i < Y - 2 && board[i + 1][j] == !color && board[i + 2][j] == EMPTY) return false;
        }
    }
    return true;
}

// The return value indicates what input the user made
// 1 for hint
// 2 for solve
// 3 for quit
// 4 for making a move
static int32_t user_move(Move* move, uintmax_t* hint_depth) {

    int32_t start_row, end_row;
    char char_start_col, char_end_col;

    while (true) {
        printf("Enter a command for (hint, solve, quit) or make a move: ");

        char* line = readline(stdin);

        uintmax_t tmp_depth;

        if (sscanf(line, "hint %ju\n", &tmp_depth) == 1 || sscanf(line, "h %ju\n", &tmp_depth) == 1) {
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
        } else if (strcmp(line, "quit\n") == 0 || strcmp(line, "q\n") == 0) {
            free(line);
            return 3;
        } else if (sscanf(line, "%c%i %c%i", &char_start_col, &start_row, &char_end_col, &end_row) == 4) {

            if ('a' <= char_start_col && char_start_col < 'a' + X &&
                'a' <= char_end_col && char_end_col < 'a' + X &&
                start_row < Y && end_row < Y) {

                int32_t start_col = char_start_col - 'a';
                int32_t end_col = char_end_col - 'a';

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

static void print_move(const Move* move) {
    char start_col, end_col;
    start_col = 'a' + (char) move->start_col;
    end_col = 'a' + (char) move->end_col;
    printf("%c%i %c%i\n", start_col, move->start_row, end_col, move->end_row);
}

static bool make_move(State board[Y][X], const Move* move, const State color) {
    // diagonal move
    if (move->start_row != move->end_row) {
        if (move->start_col != move->end_col) return false;
    }

    // coordinate bounds check
    if (0 > move->start_row || Y <= move->start_row ||
        0 > move->end_row || Y <= move->end_row ||
        0 > move->start_col || X <= move->start_col ||
        0 > move->end_col || X <= move->end_col) {
        return false;
    }

    if (board[move->start_row][move->start_col] == color) {
        State tmpboard[Y][X];
        board_copy(tmpboard, board);
        // Verticle move
        if (move->end_col == move->start_col) {
            int32_t jump = move->end_row - move->start_row;

            if (abs(jump) < 2 || abs(jump) > Y) return false;
            else {
                if (jump < 0) {
                    // up move
                    for (int32_t i = move->start_row; i > move->end_row; i -= 2) {
                        if (tmpboard[i - 1][move->start_col] != !color) return false;
                        if (tmpboard[i - 2][move->start_col] != EMPTY) return false;
                        else {
                            tmpboard[i][move->start_col] = EMPTY;
                            tmpboard[i - 1][move->start_col] = EMPTY;
                            tmpboard[i - 2][move->start_col] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return true;
                }
                if (jump > 0) {
                    // down move
                    for (int32_t i = move->start_row; i < move->end_row; i += 2) {
                        if (tmpboard[i + 1][move->start_col] != !color) return false;
                        if (tmpboard[i + 2][move->start_col] != EMPTY) return false;
                        else {
                            tmpboard[i][move->start_col] = EMPTY;
                            tmpboard[i + 1][move->start_col] = EMPTY;
                            tmpboard[i + 2][move->start_col] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return true;
                }
                return false;
            }
        }
        // Horizontal move
        if (move->end_row == move->start_row) {
            int32_t jump = move->end_col - move->start_col;

            if (abs(jump) < 2 || abs(jump) > X) return false;
            else {
                if (jump < 0) {
                    // left move
                    for (int32_t i = move->start_col; i > move->end_col; i -= 2) {
                        if (tmpboard[move->start_row][i - 1] != !color) return false;
                        if (tmpboard[move->start_row][i - 2] != EMPTY) return false;
                        else {
                            tmpboard[move->start_row][i] = EMPTY;
                            tmpboard[move->start_row][i - 1] = EMPTY;
                            tmpboard[move->start_row][i - 2] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return true;
                }
                if (jump > 0) {
                    // right move
                    for (int32_t i = move->start_col; i < move->end_col; i += 2) {
                        if (tmpboard[move->start_row][i + 1] != !color) return false;
                        if (tmpboard[move->start_row][i + 2] != EMPTY) return false;
                        else {
                            tmpboard[move->start_row][i] = EMPTY;
                            tmpboard[move->start_row][i + 1] = EMPTY;
                            tmpboard[move->start_row][i + 2] = color;
                        }
                    }
                    board_copy(board, tmpboard);
                    return true;
                }
                return false;
            }
        } else
            return false;
    } else
        return false;
}

#if 0
static bool use_hint(State board[Y][X], const Move* move, const State user) {

    char* line = readline(stdin);

    if (strcmp(line, "y\n") == 0 || strcmp(line, "yes\n") == 0) {
        make_move(board, move, user);
        printBoard(board);
        free(line);
        return true;
    } else if (strcmp(line, "n\n") == 0 || strcmp(line, "no\n") == 0) {
        free(line);
        return false;
    } else {
        free(line);
        puts("Invalid option, please try again.\n");
        return false;
    }
}
#endif

int main(int argc, char* argv[]) {

    Move move = {};
    State board[Y][X];

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    initBoard(board);

    if (argc == 3) {
        if (strcmp(argv[1], "solve") == 0 && strcmp(argv[2], "black") == 0) {
            puts("Solving for black...");
            solve_start(board, BLACK);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(argv[1], "solve") == 0 && strcmp(argv[2], "white") == 0) {
            puts("Solving for white...");
            solve_start(board, WHITE);
            exit(EXIT_SUCCESS);
        }

        puts("Invalid arguments, see README for acceptable commands");
        exit(EXIT_FAILURE);
    } else if (argc != 1) {
        puts("Invalid arguments, see README for acceptable commands");
        exit(EXIT_FAILURE);
    }

    printBoard(board);

    putchar('\n');
    uintmax_t depth = get_depth();
    const State user = getUser();

    if (user == BLACK) {
        user_black(board, depth);
    }

    if (user == WHITE) {
        computer_black(board, depth);

        system("sleep 1");

        if (game_over(board, !user)) {
            puts("\n You won!");
            exit(EXIT_SUCCESS);
        }

        // The computer then makes a second move
        int32_t score = computer_move(&move, board, !user, depth);

        make_move(board, &move, !user);

        printBoard(board);

        printf("\n Computer move = ");
        print_move(&move);
        print_score(" Computer", score);
    }

    // In this loop we assume the user goes first
    while (true) {

        // User move
        if (game_over(board, user)) {
            puts("\n Computer won!");
            break;
        }

        putchar('\n');

        while (true) {

            uintmax_t hint_depth = depth;
            const int32_t choice = user_move(&move, &hint_depth);

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

                bool valid = make_move(board, &move, user);

                if (valid) break;

                puts("Invalid move, please try again.");
            }
        }

        printBoard(board);

        system("sleep 1");

        if (game_over(board, !user)) {
            puts("\n You won!");
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
