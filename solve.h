#pragma once

#include <stdint.h>

#include "konane.h"

int32_t solve_negamax(const State board[SIZE][SIZE], const State color);
