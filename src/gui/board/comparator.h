#pragma once

struct SquareComp {
	bool operator()(Square const& lhs, Square const& rhs) const {
		if (lhs.x < rhs.x) {
			return true;
		} else {
			return lhs.y < rhs.y;
		}
	}
};

struct MoveComp {
	SquareComp comp;
	
	bool operator()(Move const& lhs, Move const& rhs) const {
		if (comp(lhs.from, rhs.from)) {
			return true;
		} else {
			return comp(lhs.to, rhs.to);
		}
	}
};
