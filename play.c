#include <stddef.h>
#include <stdlib.h>

#include "play.h"

// Winning is the best possible outcome, and losing is the worst possible.
// It is impossible to be able to move to every single spot on a single turn,
// so SIZE * SIZE will be greater than every possible score. Likewise,
// - SIZE * SIZE will be less than any possible score, so we will use those
// as the values for winning and loosing.
static void count_moves(const State board[SIZE][SIZE], int32_t* black, int32_t* white) {

    int8_t moveboard[2][SIZE][SIZE] = {};

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            const State color = board[i][j];

            if (color == EMPTY) continue;

            // Search up
            for (size_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {
                    moveboard[color][k - 2][j] = 1;
                } else break;
            }

            // Search right
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {
                    moveboard[color][i][k + 2] = 1;
                } else break;
            }

            // Search left
            for (size_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {
                    moveboard[color][i][k - 2] = 1;
                } else break;
            }

            // Search down
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {
                    moveboard[color][k + 2][j] = 1;
                } else break;
            }
        }
    }

    int32_t sum[2] = {0, 0};
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            sum[BLACK] += moveboard[BLACK][i][j];
            sum[WHITE] += moveboard[WHITE][i][j];
        }
    }

    *black = sum[BLACK];
    *white = sum[WHITE];
}

static int32_t evaluate_board(const State board[SIZE][SIZE], const State color) {

    int32_t black;
    int32_t white;
    count_moves(board, &black, &white);

    if (color == BLACK && black == 0) {
        // Black to play, but black has no moves, so black looses
        return LOOSE;
    }

    if (color == WHITE && white == 0) {
        // Ditto for white
        return LOOSE;
    }

    switch (color) {
    case BLACK:
        return black - white;
    case WHITE:
        return white - black;
    case EMPTY:
        exit(EXIT_FAILURE);
    }
}

int32_t negamax(const State board[SIZE][SIZE], const State color, const uint32_t depth) {

    if (depth == 0) {
        return evaluate_board(board, color);
    }

    // If the depth is not zero, we then check all possible moves
    // This will also catch terminal nodes, because there will be no moves to make
    State tmpboard[SIZE][SIZE];

    int32_t score = LOOSE;

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {

            if (board[i][j] != color) continue;

            // Search up
            board_copy(tmpboard, board);
            for (size_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k - 1][j] = EMPTY;
                    tmpboard[k - 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }

            // Search right
            board_copy(tmpboard, board);
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k + 1] = EMPTY;
                    tmpboard[i][k + 2] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }

            // Search left
            board_copy(tmpboard, board);
            for (size_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k - 1] = EMPTY;
                    tmpboard[i][k - 2] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }

            // Search down
            board_copy(tmpboard, board);
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k + 1][j] = EMPTY;
                    tmpboard[k + 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }
        }
    }

    return score;
}

int32_t computer_move(Move *move, const State board[SIZE][SIZE], const State color) {

    State tmpboard[SIZE][SIZE];

    int32_t score = LOOSE;

    uint32_t depth = DEPTH;

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {

            if (board[i][j] != color) continue;

            // Search up
            board_copy(tmpboard, board);
            for (size_t k = i; k >= 2; k -= 2) {
                if (tmpboard[k - 1][j] == !color && tmpboard[k - 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k - 1][j] = EMPTY;
                    tmpboard[k - 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    // If this move is better than the current best, then record that move
                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = k - 2;
                        move->end_col = j;
                    }

                } else break;
            }

            // Search right
            board_copy(tmpboard, board);
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (tmpboard[i][k + 1] == !color && tmpboard[i][k + 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k + 1] = EMPTY;
                    tmpboard[i][k + 2] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = i;
                        move->end_col = k + 2;
                    }

                } else break;
            }

            // Search left
            board_copy(tmpboard, board);
            for (size_t k = j; k >= 2; k -= 2) {
                if (tmpboard[i][k - 1] == !color && tmpboard[i][k - 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k - 1] = EMPTY;
                    tmpboard[i][k - 2] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = i;
                        move->end_col = k - 2;
                    }

                } else break;
            }

            // Search down
            board_copy(tmpboard, board);
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (tmpboard[k + 1][j] == !color && tmpboard[k + 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k + 1][j] = EMPTY;
                    tmpboard[k + 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, depth - 1);

                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = k + 2;
                        move->end_col = j;
                    }

                } else break;
            }
        }
    }

    return score;
}
