#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "play.h"

// Winning is the best possible outcome, and losing is the worst possible.
// It is impossible to be able to move to every single spot on a single turn,
// so X * Y will be greater than every possible score. Likewise,
// - X * Y will be less than any possible score, so we will use those
// as the values for winning and loosing.
static void count_moves(const State board[Y][X], int32_t* black, int32_t* white) {

    int8_t moveboard[2][Y][X] = {};

    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {
            const State color = board[i][j];

            if (color == EMPTY) continue;

            // Search up
            for (int32_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {
                    moveboard[color][k - 2][j] = 1;
                } else
                    break;
            }

            // Search right
            for (int32_t k = j; k < X - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {
                    moveboard[color][i][k + 2] = 1;
                } else
                    break;
            }

            // Search left
            for (int32_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {
                    moveboard[color][i][k - 2] = 1;
                } else
                    break;
            }

            // Search down
            for (int32_t k = i; k < Y - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {
                    moveboard[color][k + 2][j] = 1;
                } else
                    break;
            }
        }
    }

    int32_t sum[2] = {0, 0};
    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {
            sum[BLACK] += moveboard[BLACK][i][j];
            sum[WHITE] += moveboard[WHITE][i][j];
        }
    }

    *black = sum[BLACK];
    *white = sum[WHITE];
}

static int32_t evaluate_board(const State board[Y][X], const State color) {

    int32_t black;
    int32_t white;
    count_moves(board, &black, &white);

    if (color == BLACK && black == 0) {
        // Black to play, but black has no moves, so black looses
        return LOSE;
    }

    if (color == WHITE && white == 0) {
        // Ditto for white
        return LOSE;
    }

    switch (color) {
    case BLACK:
        return black - white;
    case WHITE:
        return white - black;
    default:
        printf("Invalid color %i in %s\n", color, __func__);
        exit(EXIT_FAILURE);
    }
}

// color is the player who is currently making a move
int32_t negamax(const State board[Y][X], const State color, int32_t a, int32_t b, const uintmax_t depth) {

    if (depth == 0) {
        return evaluate_board(board, color);
    }

    // If the depth is not zero, we then check all possible moves
    // This will also catch terminal nodes, because there will be no moves to make
    State tmpboard[Y][X];

    int32_t score = LOSE;

    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {

            if (board[i][j] != color) continue;

            // Search up
            board_copy(tmpboard, board);
            for (int32_t k = i; k >= 2; k -= 2) {
                if (board[k - 1][j] == !color && board[k - 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k - 1][j] = EMPTY;
                    tmpboard[k - 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }

            // Search right
            board_copy(tmpboard, board);
            for (int32_t k = j; k < X - 2; k += 2) {
                if (board[i][k + 1] == !color && board[i][k + 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k + 1] = EMPTY;
                    tmpboard[i][k + 2] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }

            // Search left
            board_copy(tmpboard, board);
            for (int32_t k = j; k >= 2; k -= 2) {
                if (board[i][k - 1] == !color && board[i][k - 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k - 1] = EMPTY;
                    tmpboard[i][k - 2] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }

            // Search down
            board_copy(tmpboard, board);
            for (int32_t k = i; k < Y - 2; k += 2) {
                if (board[k + 1][j] == !color && board[k + 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k + 1][j] = EMPTY;
                    tmpboard[k + 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    if (val > score)
                        score = val;
                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }
        }
    }

    return score;
}

int32_t computer_move(Move* move, const State board[Y][X], const State color, const uintmax_t depth) {

    State tmpboard[Y][X];

    int32_t score = LOSE;

    int32_t a = LOSE;
    int32_t b = WIN;

    for (int32_t i = 0; i < Y; ++i) {
        for (int32_t j = 0; j < X; ++j) {

            if (board[i][j] != color) continue;

            // Search up
            board_copy(tmpboard, board);
            for (int32_t k = i; k >= 2; k -= 2) {
                if (tmpboard[k - 1][j] == !color && tmpboard[k - 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k - 1][j] = EMPTY;
                    tmpboard[k - 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    // If this move is better than the current best, then record that move
                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = k - 2;
                        move->end_col = j;
                    }

                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }

            // Search right
            board_copy(tmpboard, board);
            for (int32_t k = j; k < X - 2; k += 2) {
                if (tmpboard[i][k + 1] == !color && tmpboard[i][k + 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k + 1] = EMPTY;
                    tmpboard[i][k + 2] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = i;
                        move->end_col = k + 2;
                    }

                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }

            // Search left
            board_copy(tmpboard, board);
            for (int32_t k = j; k >= 2; k -= 2) {
                if (tmpboard[i][k - 1] == !color && tmpboard[i][k - 2] == EMPTY) {

                    tmpboard[i][k] = EMPTY;
                    tmpboard[i][k - 1] = EMPTY;
                    tmpboard[i][k - 2] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = i;
                        move->end_col = k - 2;
                    }

                    if (score > a)
                        a = score;
                    if (a >= b)
                        return score;

                } else
                    break;
            }

            // Search down
            board_copy(tmpboard, board);
            for (int32_t k = i; k < Y - 2; k += 2) {
                if (tmpboard[k + 1][j] == !color && tmpboard[k + 2][j] == EMPTY) {

                    tmpboard[k][j] = EMPTY;
                    tmpboard[k + 1][j] = EMPTY;
                    tmpboard[k + 2][j] = color;

                    int32_t val = -negamax(tmpboard, !color, -b, -a, depth - 1);

                    if (val >= score) {
                        score = val;

                        move->start_row = i;
                        move->start_col = j;
                        move->end_row = k + 2;
                        move->end_col = j;
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

    return score;
}

// Here we are making turns for white
int32_t second_turn_search(const State board[Y][X], const int32_t row, const int32_t col, int32_t a, int32_t b, const uintmax_t depth) {

    State tmpboard[Y][X];

    int32_t score = LOSE;

    // Move up
    if (row != 0) {
        board_copy(tmpboard, board);
        tmpboard[row - 1][col] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val > score)
            score = val;
        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    // Move right
    if (col < X - 1) {
        board_copy(tmpboard, board);
        tmpboard[row][col + 1] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val > score)
            score = val;
        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    // Move left
    if (col != 0) {
        board_copy(tmpboard, board);
        tmpboard[row][col - 1] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val > score)
            score = val;
        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    // Move down
    if (row < Y - 1) {
        board_copy(tmpboard, board);
        tmpboard[row + 1][col] = EMPTY;
        int32_t val = -negamax(tmpboard, BLACK, -b, -a, depth - 1);

        if (val > score)
            score = val;
        if (score > a)
            a = score;
        if (a >= b)
            return score;
    }

    return score;
}

