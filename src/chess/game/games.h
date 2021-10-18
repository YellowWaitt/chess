#pragma once

#include "chessgame.h"
#include "chessgame960.h"
#include "kingofthehill.h"
#include "threecheck.h"
#include "horde.h"


enum class GameType {
	Classic,
	Chess960,
	Koth,
	ThreeCheck,
	Horde
};
