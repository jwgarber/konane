#pragma once

typedef enum {
    BLACK = 0,
    WHITE = 1,
    EMPTY = 2,
} State;

/*#define SIZE  4*/
#define SIZE    6

#define WIN      (SIZE * SIZE)
#define LOOSE    (-SIZE * SIZE)

typedef struct {
    size_t start_row;
    size_t start_col;
    size_t end_row;
    size_t end_col;
} Move;
