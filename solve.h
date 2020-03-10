#pragma once

#include <stdint.h>

#include "konane.h"

int32_t solve_negamax(const State board[SIZE][SIZE], const State color, int32_t a, int32_t b);
