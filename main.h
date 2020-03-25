#pragma once

#include <string.h>

_Static_assert(1 <= X && X <= 26, "X must be between 1 and 26 (inclusive)");
_Static_assert(1 <= Y && Y <= 26, "Y must be between 1 and 26 (inclusive)");

typedef uint8_t State;

#define BLACK 0
#define WHITE 1
#define EMPTY 2

// The number of possible moves a player can make is bounded above by the
// number of opponent stones on the board, since each jump captures an opponent
// stone. There are at most X*Y stones on the board. However, if a player is
// to move, they must have at least one of their stones on the board, so the
// maximum number of possible moves is strictly less than X*Y.
#define WIN (X * Y)
#define LOSE (-WIN)

#define DEPTH 6

typedef struct {
    int32_t start_row;
    int32_t start_col;
    int32_t end_row;
    int32_t end_col;
} Move;

static inline void board_copy(State dst[Y][X], const State src[Y][X]) {
    memcpy(dst, src, X * Y * sizeof(State));
}
