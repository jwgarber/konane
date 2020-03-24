#pragma once

#include <stdint.h>

#include "main.h"

int32_t negamax(const State board[SIZE][SIZE], const State color, int32_t a, int32_t b, const uintmax_t depth);
int32_t computer_move(Move* move, const State board[SIZE][SIZE], const State player, const uintmax_t depth);
int32_t second_turn_search(const State board[SIZE][SIZE], const size_t row, const size_t col, int32_t a, int32_t b, const uintmax_t depth);
