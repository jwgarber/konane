#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "solve.h"

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

// alpha is the current lower bound on my possible score (eg. -oo)
// beta is the current upper bound on my possible score (eg. +oo)
// the other player is going to minimize my best scores. If one of my
// possible moves is better than a choice the opponent can already pick,
// they won't pick this move, so bail out early.

static int32_t negamax(const State board[SIZE][SIZE], const State color, int32_t a, int32_t b) {

    State newboard[SIZE][SIZE];

    int32_t score = LOSE;

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

                    int32_t val = -negamax(newboard, !color, -b, -a);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >=b)
                        return score;

                } else break;
            }

            // Search right
            board_copy(newboard, board);
            for (size_t k = j; k < SIZE - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k + 1] = EMPTY;
                    newboard[i][k + 2] = color;

                    int32_t val = -negamax(newboard, !color, -b, -a);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >=b)
                        return score;
                } else break;
            }

            // Search left
            board_copy(newboard, board);
            for (size_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {

                    newboard[i][k] = EMPTY;
                    newboard[i][k - 1] = EMPTY;
                    newboard[i][k - 2] = color;

                    int32_t val = -negamax(newboard, !color, -b, -a);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >=b)
                        return score;
                } else break;
            }

            // Search down
            board_copy(newboard, board);
            for (size_t k = i; k < SIZE - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {

                    newboard[k][j] = EMPTY;
                    newboard[k + 1][j] = EMPTY;
                    newboard[k + 2][j] = color;

                    int32_t val = -negamax(newboard, !color, -b, -a);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >=b)
                        return score;
                } else break;
            }
        }
    }

    if (score == LOSE) {
	    // We cannot make any moves, so the other player has won.
	    // Count how many moves they have left, and that is the final score.
	    score = -count_moves(board, !color);
    }

    return score;
}

void solve(const State board[SIZE][SIZE], const State color) {

    puts("Solving, type ^C to cancel");
    const int32_t score = negamax(board, color, LOSE, WIN);
    printf("Solve score = %i\n", score);
    // print the moves made in solve
    // ^ make a list.
}
