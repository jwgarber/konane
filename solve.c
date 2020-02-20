#include <stddef.h>

#include "solve.h"

static void board_copy(State dst[SIZE][SIZE], const State src[SIZE][SIZE]) {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            dst[i][j] = src[i][j];
        }
    }
}

static int32_t negamax(const State board[SIZE][SIZE], const State player) {

    State newboard[SIZE][SIZE];

    int score = LOOSE;

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {

            if (board[i][j] != player) continue;

            // Search up
            board_copy(newboard, board);
            for (size_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !player && board[k - 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k - 1][j] = EMPTY;
                    newboard[k - 2][j] = player;

                    int32_t val = -negamax(newboard, !player);

                    // If we have found a winning move, we don't need to do any more searching
                    if (val == WIN)
                        return WIN;

                } else break;
            }

            // Search right
            board_copy(newboard, board);
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (board[i][k + 1] == !player && board[i][k + 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k + 1] = EMPTY;
                    newboard[i][k + 2] = player;

                    int32_t val = -negamax(newboard, !player);

                    if (val == WIN)
                        return WIN;

                } else break;
            }

            // Search left
            board_copy(newboard, board);
            for (size_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !player && board[i][k - 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k - 1] = EMPTY;
                    newboard[i][k - 2] = player;

                    int32_t val = -negamax(newboard, !player);

                    if (val == WIN)
                        return WIN;

                } else break;
            }

            // Search down
            board_copy(newboard, board);
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (board[k + 1][j] == !player && board[k + 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k + 1][j] = EMPTY;
                    newboard[k + 2][j] = player;

                    int32_t val = -negamax(newboard, !player);

                    if (val == WIN)
                        return WIN;

                } else break;
            }
        }
    }

    return score;
}
