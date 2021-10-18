#pragma once

#include "chess/ai/evaluation.h"
#include "util/chrono.h"


void printDatas(
	int const depth,
	eval_t const eval,
	int const nbMoves,
	int const nbLeafs,
	Chrono const& chrono,
	std::vector<Move> const& moves
);
