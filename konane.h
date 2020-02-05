#pragma once

typedef enum {
    BLACK = 0,
    WHITE = 1,
    EMPTY = 2,
} State;

/*#define SIZE	4*/
#define SIZE	6

#define WIN      (SIZE * SIZE)
#define LOOSE    (-SIZE * SIZE)
