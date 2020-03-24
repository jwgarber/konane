#pragma once

#include <stdint.h>

#include "main.h"

int32_t negamax(const State board[Y][X], const State color, int32_t a, int32_t b, const uintmax_t depth);
int32_t computer_move(Move* move, const State board[Y][X], const State player, const uintmax_t depth);
int32_t second_turn_search(const State board[Y][X], const int32_t row, const int32_t col, int32_t a, int32_t b, const uintmax_t depth);
