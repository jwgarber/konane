#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

#include "solve.h"

typedef struct {
    Move* moves;  // pointer to malloc'd array
    int32_t size; // current length of the list
} MovesList;

static void alloc_list(MovesList* list) {

    // There can be at most X*Y moves in a game, since there are X*Y
    // pieces in the beginning and each move removes a piece.
    Move* ptr = malloc(X * Y * sizeof(Move));

    if (ptr == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    list->moves = ptr;
    list->size = 0;
}

static void free_list(MovesList* list) {
    free(list->moves);
}

static void add_move(MovesList* list, const Move* move) {
    list->moves[list->size] = *move;
    list->size += 1;
}

static void print_move(const Move* move, const State color) {

    char start_col = 'a' + (char) move->start_col;
    char end_col = 'a' + (char) move->end_col;

    switch (color) {
    case BLACK:
        printf(" B %c%i %c%i\n", start_col, move->start_row, end_col, move->end_row);
        break;
    case WHITE:
        printf(" W %c%i %c%i\n", start_col, move->start_row, end_col, move->end_row);
        break;
    default:
        printf("Invalid color %i in %s\n", color, __func__);
        exit(EXIT_FAILURE);
    }
}

static void print_list(const MovesList* list, State color, int32_t score) {

    for (int32_t i = list->size - 1; i >= 0; --i) {
        print_move(&list->moves[i], color);
        color = !color;
    }

    if (score < 0) {
        score = -score;
    }

    switch (color) {
    case BLACK:
        printf(" W wins by %i\n", score - 1);
        break;
    case WHITE:
        printf(" B wins by %i\n", score - 1);
        break;
    default:
        printf("Invalid color %i in %s\n", color, __func__);
        exit(EXIT_FAILURE);
    }
}

// Count the number of moves for this player.
static int32_t count_moves(const State board[Y][X], const State color) {

    int32_t count = 0;

    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {

            if (board[i][j] != color) continue;

            // Search up
            for (int32_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {
                    ++count;
                } else
                    break;
            }

            // Search right
            for (int32_t k = j; k < X - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {
                    ++count;
                } else
                    break;
            }

            // Search left
            for (int32_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {
                    ++count;
                } else
                    break;
            }

            // Search down
            for (int32_t k = i; k < Y - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {
                    ++count;
                } else
                    break;
            }
        }
    }

    return count;
}

// alpha is the current lower bound on my possible score (eg. -oo)
// beta is the current upper bound on my possible score (eg. +oo)
// the other player is going to minimize my best scores. If one of my
// possible moves is better than a choice the opponent can already pick,
// they won't pick this move, so bail out early.

static int32_t negamax(const State board[Y][X], const State color, int32_t a, int32_t b, MovesList* list) {

    // Set the list to NULL at the beginning
    list->moves = NULL;
    int32_t score = LOSE - 1;

    MovesList newlist;
    State newboard[Y][X];

    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {

            if (board[i][j] != color) continue;

            // Search up
            board_copy(newboard, board);
            for (int32_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k - 1][j] = EMPTY;
                    newboard[k - 2][j] = color;

                    int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

                    if (val > score) {
                        score = val;

                        // Free the old list of moves, because this one is better
                        free_list(list);
                        *list = newlist;

                        Move move = {i, j, k - 2, j};
                        add_move(list, &move);
                    } else {
                        // Otherwise we don't use this list, so just ignore it
                        free_list(&newlist);
                    }

                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }

            // Search right
            board_copy(newboard, board);
            for (int32_t k = j; k < X - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k + 1] = EMPTY;
                    newboard[i][k + 2] = color;

                    int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

                    if (val > score) {
                        score = val;

                        // Free the old list of moves, because this one is better
                        free_list(list);
                        *list = newlist;

                        Move move = {i, j, i, k + 2};
                        add_move(list, &move);
                    } else {
                        // Otherwise we don't use this list, so just ignore it
                        free_list(&newlist);
                    }

                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;
                } else
                    break;
            }

            // Search left
            board_copy(newboard, board);
            for (int32_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k - 1] = EMPTY;
                    newboard[i][k - 2] = color;

                    int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

                    if (val > score) {
                        score = val;

                        // Free the old list of moves, because this one is better
                        free_list(list);
                        *list = newlist;

                        Move move = {i, j, i, k - 2};
                        add_move(list, &move);
                    } else {
                        // Otherwise we don't use this list, so just ignore it
                        free_list(&newlist);
                    }

                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;
                } else
                    break;
            }

            // Search down
            board_copy(newboard, board);
            for (int32_t k = i; k < Y - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k + 1][j] = EMPTY;
                    newboard[k + 2][j] = color;

                    int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

                    if (val > score) {
                        score = val;

                        // Free the old list of moves, because this one is better
                        free_list(list);
                        *list = newlist;

                        Move move = {i, j, k + 2, j};
                        add_move(list, &move);
                    } else {
                        // Otherwise we don't use this list, so just ignore it
                        free_list(&newlist);
                    }

                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;
                } else
                    break;
            }
        }
    }

    if (score == LOSE - 1) {
        // We cannot make any moves, so the other player has won.
        // Count how many moves they have left, and that is the final score.
        score = -count_moves(board, !color) - 1;
        // Also create a new empty list
        alloc_list(list);
    }

    return score;
}

static int32_t second_turn(const State board[Y][X], const Move* move, const State color, int32_t a, int32_t b, MovesList* list) {

    list->moves = NULL;

    MovesList newlist;
    State newboard[Y][X];

    int32_t score = LOSE - 1;

    const int32_t row = move->start_row;
    const int32_t col = move->start_col;

    // Move up
    if (row != 0) {
        board_copy(newboard, board);
        newboard[row - 1][col] = EMPTY;
        int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

        if (val > score) {
            score = val;

            // Free the old list of moves, because this one is better
            free_list(list);
            *list = newlist;

            Move newmove = {row - 1, col, row - 1, col};
            add_move(list, &newmove);
        } else {
            // Otherwise we don't use this list, so just ignore it
            free_list(&newlist);
        }

        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    // Move right
    if (col < X - 1) {
        board_copy(newboard, board);
        newboard[row][col + 1] = EMPTY;
        int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

        if (val > score) {
            score = val;

            // Free the old list of moves, because this one is better
            free_list(list);
            *list = newlist;

            Move newmove = {row, col + 1, row, col + 1};
            add_move(list, &newmove);
        } else {
            // Otherwise we don't use this list, so just ignore it
            free_list(&newlist);
        }

        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    // Move left
    if (col != 0) {
        board_copy(newboard, board);
        newboard[row][col - 1] = EMPTY;
        int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

        if (val > score) {
            score = val;

            // Free the old list of moves, because this one is better
            free_list(list);
            *list = newlist;

            Move newmove = {row, col - 1, row, col - 1};
            add_move(list, &newmove);
        } else {
            // Otherwise we don't use this list, so just ignore it
            free_list(&newlist);
        }

        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    // Move down
    if (row < Y - 1) {
        board_copy(newboard, board);
        newboard[row + 1][col] = EMPTY;
        int32_t val = -negamax(newboard, !color, -b, -a, &newlist);

        if (val > score) {
            score = val;

            // Free the old list of moves, because this one is better
            free_list(list);
            *list = newlist;

            Move newmove = {row + 1, col, row + 1, col};
            add_move(list, &newmove);
        } else {
            // Otherwise we don't use this list, so just ignore it
            free_list(&newlist);
        }

        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    if (score == LOSE - 1) {
        // In this case we have a 1x1 board, and we have lost
        score = -1;
        // Also create a new empty list
        alloc_list(list);
    }

    return score;
}

static void print_status(const int32_t i, const int32_t j, const int32_t val, const State color) {

    printf("Finished position (%i, %i), ", i, j);

    if (color == BLACK && val < 0) {
        printf("W wins by %i\n", -val - 1);
    } else if (color == BLACK && val > 0) {
        printf("B wins by %i\n", val - 1);
    } else if (color == WHITE && val < 0) {
        printf("B wins by %i\n", -val - 1);
    } else if (color == WHITE && val > 0) {
        printf("W wins by %i\n", val - 1);
    } else {
        puts("Unknown case in print_status");
    }

    fflush(stdout);
}

static int32_t first_turn(const State board[Y][X], const State color, MovesList* list) {

    list->moves = NULL;

    // In this case, score and alpha are both initialized to the lowest possible value,
    // and they will remain the same throughout the execution, so we can use score
    // instead of alpha
    int32_t score = LOSE - 1;

    #pragma omp parallel for collapse(2)
    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {

            if (board[i][j] != color) continue;

            const int32_t b = WIN + 1;

            MovesList newlist;
            State newboard[Y][X];

            board_copy(newboard, board);
            newboard[i][j] = EMPTY;

            const Move newmove = {i, j, i, j};

            const int32_t val = -second_turn(newboard, &newmove, !color, -b, -score, &newlist);

            #pragma omp critical
            // In theory, we could save all the lists and scores, and then find the max
            // once everything is done (and then just use atomic for the score)
            {
            if (val > score) {
                score = val;

                // Free the old list of moves, because this one is better
                free_list(list);
                *list = newlist;

                add_move(list, &newmove);
            } else {
                // Otherwise we don't use this list, so just ignore it
                free_list(&newlist);
            }

            // Normally here we return early if a >= b. However, b is never
            // updated, and its initial value is higher than any possible score,
            // so this will never happen.
            print_status(i, j, val, color);
            }
        }
    }

    if (score == LOSE - 1) {
        // In this case we are playing white on a 1x1 board, and we have lost
        score = -1;
        // Also create a new empty list
        alloc_list(list);
    }

    return score;
}

void solve_start(const State board[Y][X], const State color) {

    MovesList list;
    const int32_t score = first_turn(board, color, &list);

    print_list(&list, color, score);

    free_list(&list);
}

void solve(const State board[Y][X], const State color) {

    puts("Solving, type ^C to cancel");

    MovesList list;
    const int32_t score = negamax(board, color, LOSE - 1, WIN + 1, &list);

    print_list(&list, color, score);

    free_list(&list);
}
