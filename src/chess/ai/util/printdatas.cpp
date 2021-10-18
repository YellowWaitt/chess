#include "printdatas.h"

#include <iomanip>
#include <iostream>
#include <sstream>


std::string str(Square const& square) {
	std::stringstream ss;
	ss << char(square.x + 'a') << square.y + 1;
	return ss.str();
}

std::string str(Move const& move) {
	std::stringstream ss;
	ss << "(" << str(move.from) << "," << str(move.to) << ")";
	return ss.str();
}

std::string str(std::vector<Move> const& moves) {
	std::stringstream ss;
	ss << "[" << str(moves[0]);
	for (auto& move : moves) {
		ss << ", " << str(move);
	}
	ss << "]";
	return ss.str();
}

void printDatas(
	int const depth,
	eval_t const eval,
	int const nbMoves,
	int const nbLeafs,
	Chrono const& chrono,
	std::vector<Move> const& moves
) {
	using namespace std;
	cout << "Depth " << setw(2) << depth
		 << " / eval " << setw(8) << eval
		 << " / nbMoves " << setw(8) << nbMoves
		 << " / nbLeafs " << setw(8) << nbLeafs
		 << " / time " << left << setw(8) << chrono.time() << right << " seconds / "
		 << setw(7) << nbMoves / chrono.time() << " mps / "
		 << setw(7) << nbLeafs / chrono.time() << " lps / "
		 << str(moves) << endl;
}
