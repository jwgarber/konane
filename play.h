#pragma once

#include <stdint.h>

#include "konane.h"

int64_t nextmove(State newboard[SIZE][SIZE], const State board[SIZE][SIZE], const State player);
