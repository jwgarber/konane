#pragma once

#include <stdint.h>

#include "konane.h"

int64_t computer_move(Move* move, const State board[SIZE][SIZE], const State player);

static inline void board_copy(State dst[SIZE][SIZE], const State src[SIZE][SIZE]) {
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            dst[i][j] = src[i][j];
        }
    }
}

