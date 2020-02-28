#pragma once

#include <string.h>

typedef enum {
    BLACK = 0,
    WHITE = 1,
    EMPTY = 2,
} State;

//#define SIZE  4
#define SIZE  8

// The number of possible moves a player can make is bounded above by the
// number of opponent stones on the board, since each jump captures an opponent
// stone. There are at most SIZE^2 stones on the board. However, if a player is
// to move, they must have at least one of their stones on the board, so the
// maximum number of possible moves is strictly less than SIZE^2.
#define WIN      (SIZE * SIZE)
#define LOOSE    (-SIZE * SIZE)

#define DEPTH   6

typedef struct {
    size_t start_row;
    size_t start_col;
    size_t end_row;
    size_t end_col;
} Move;

static inline void board_copy(State dst[SIZE][SIZE], const State src[SIZE][SIZE]) {
	memcpy(dst, src, SIZE * SIZE * sizeof(State));
}

