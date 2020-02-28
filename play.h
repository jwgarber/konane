#pragma once

#include <stdint.h>

#include "konane.h"

int32_t negamax(const State board[SIZE][SIZE], const State color, int32_t a, int32_t b, const uint32_t depth);
int32_t computer_move(Move* move, const State board[SIZE][SIZE], const State player);
