#include <stdio.h>
#include <stdint.h>

// Things to do

typedef enum {
    BLACK = 0,
    WHITE = 1,
    EMPTY = 2,
} State;

#define SIZE	4

static void evaluate_board(const State board[SIZE][SIZE], uint32_t* black, uint32_t* white) {

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

#define WHITE_CIRCLE "○"
#define BLACK_CIRCLE "●"

int main(void) {

    State board[SIZE][SIZE];
    puts("Konane!");
}
