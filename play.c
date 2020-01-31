#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "konane.h"

#define WIN      (SIZE * SIZE)
#define LOOSE    (-SIZE * SIZE)

static void board_copy(State dst[SIZE][SIZE], const State src[SIZE][SIZE]) {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            dst[i][j] = src[i][j];
        }
    }
}

// Winning is the best possible outcome, and losing is the worst possible
// It is impossible to be able to move to every single spot on a single turn,
// so SIZE * SIZE will be greater than every possible score. Likewise,
// - SIZE * SIZE will be less than any possible score, so we will use those
// as the values for winning and loosing.
static void count_moves(const State board[SIZE][SIZE], uint32_t* black, uint32_t* white) {

    uint8_t newboard[2][SIZE][SIZE] = {};

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            const State player = board[i][j];

            if (player == EMPTY) continue;

            // Search up
            for (size_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !player && board[k - 2][j] == EMPTY) {
                    newboard[player][k - 2][j] = 1;
                } else break;
            }

            // Search right
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (board[i][k + 1] == !player && board[i][k + 2] == EMPTY) {
                    newboard[player][i][k + 2] = 1;
                } else break;
            }

            // Search left
            for (size_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !player && board[i][k - 2] == EMPTY) {
                    newboard[player][i][k - 2] = 1;
                } else break;
            }

            // Search down
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (board[k + 1][j] == !player && board[k + 2][j] == EMPTY) {
                    newboard[player][k + 2][j] = 1;
                } else break;
            }
        }
    }

    uint32_t sum[2] = {0, 0};
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            sum[0] += newboard[0][i][j];
            sum[1] += newboard[1][i][j];
        }
    }

    *black = sum[BLACK];
    *white = sum[WHITE];
}

static int64_t evaluate_board(const State board[SIZE][SIZE], const State player) {

    uint32_t black;
    uint32_t white;
    count_moves(board, &black, &white);

    if (player == BLACK && black == 0) {
        // Black to play, but black has no moves, so black looses
        return LOOSE;
    }

    if (player == WHITE && white == 0) {
        // Ditto for white
        return LOOSE;
    }

    switch (player) {
    case BLACK:
        return black - white;
    case WHITE:
        return white - black;
    case EMPTY:
        exit(EXIT_FAILURE);
    }
}

static int64_t negamax(const State board[SIZE][SIZE], const State player, const uint32_t depth) {

    if (depth == 0) {
        return evaluate_board(board, player);
    }

    // If the depth is not zero, we then check all possible moves
    // This will also catch terminal nodes, because there will be no moves to make
    State newboard[SIZE][SIZE];

    int64_t score = LOOSE;

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {

            if (board[i][j] != player) continue;

            // Search up
            board_copy(newboard, board);
            for (size_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !player && board[k - 2][j] == EMPTY) {

                    newboard[k][i] = EMPTY;
                    newboard[k - 1][j] = EMPTY;
                    newboard[k - 2][j] = player;

                    int64_t val = -negamax(newboard, !player, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }

            // Search right
            board_copy(newboard, board);
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (board[i][k + 1] == !player && board[i][k + 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k + 1] = EMPTY;
                    newboard[i][k + 2] = player;

                    int64_t val = -negamax(newboard, !player, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }

            // Search left
            board_copy(newboard, board);
            for (size_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !player && board[i][k - 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k - 1] = EMPTY;
                    newboard[i][k - 2] = player;

                    int64_t val = -negamax(newboard, !player, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }

            // Search down
            board_copy(newboard, board);
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (board[k + 1][j] == !player && board[k + 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k + 1][j] = EMPTY;
                    newboard[k + 2][j] = player;

                    int64_t val = -negamax(newboard, !player, depth - 1);

                    if (val > score)
                        score = val;

                } else break;
            }
        }
    }

    return score;
}
