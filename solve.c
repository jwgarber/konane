#include <stddef.h>

#include "solve.h"

static void board_copy(State dst[SIZE][SIZE], const State src[SIZE][SIZE]) {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            dst[i][j] = src[i][j];
        }
    }
}

// Count the number of moves for this player.
static int32_t count_moves(const State board[SIZE][SIZE], const State color) {

	int32_t count = 0;

    for (size_t i = 0; i < SIZE; ++i) {
	for (size_t j = 0; j < SIZE; ++j) {

	    if (board[i][j] != color) continue;

	    // Search up
	    for (size_t k = i; k >= 2; k -= 2) {
		if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {
			++count;
		} else break;
	    }

	    // Search right
	    for (size_t k = j; k < SIZE - 2; k += 2) {
		if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {
			++count;
		} else break;
	    }

	    // Search left
	    for (size_t k = j; k >= 2; k -= 2) {
		if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {
			++count;
		} else break;
	    }

	    // Search down
	    for (size_t k = i; k < SIZE - 2; k += 2) {
		if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {
			++count;
		} else break;
	    }
	}
    }

    return count;
}

int32_t solve_negamax(const State board[SIZE][SIZE], const State color) {

    State newboard[SIZE][SIZE];

    int32_t score = LOOSE;

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {

            if (board[i][j] != color) continue;

            // Search up
            board_copy(newboard, board);
            for (size_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k - 1][j] = EMPTY;
                    newboard[k - 2][j] = color;

                    int32_t val = -solve_negamax(newboard, !color);

                    // If we have found a winning move, we don't need to do any more searching
                    if (val == WIN)
                        return WIN;

                } else break;
            }

            // Search right
            board_copy(newboard, board);
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k + 1] = EMPTY;
                    newboard[i][k + 2] = color;

                    int32_t val = -solve_negamax(newboard, !color);

                    if (val == WIN)
                        return WIN;

                } else break;
            }

            // Search left
            board_copy(newboard, board);
            for (size_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k - 1] = EMPTY;
                    newboard[i][k - 2] = color;

                    int32_t val = -solve_negamax(newboard, !color);

                    if (val == WIN)
                        return WIN;

                } else break;
            }

            // Search down
            board_copy(newboard, board);
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k + 1][j] = EMPTY;
                    newboard[k + 2][j] = color;

                    int32_t val = -solve_negamax(newboard, !color);

                    if (val == WIN)
                        return WIN;

                } else break;
            }
        }
    }

    if (score == LOOSE) {
	    // We cannot make any moves, so the other player has won.
	    // Count how many moves they have left, and that is the final score.
	    score = -count_moves(board, !color);
    }

    return score;
}
