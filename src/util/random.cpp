#include "random.h"

#include <cstdlib>
#include <ctime>
#include <random>


bool Random::needInit = true;
// static std::default_random_engine generator;


Random::Random() {
	if (needInit) init();
}

int Random::uniform(int a, int b) {
	// std::uniform_int_distribution<int> distribution(a, b);
	// return distribution(generator);
	return std::rand() % (b - a + 1) + a;
}

void Random::init() {
	// generator.seed(std::time(nullptr));
	srand(std::time(nullptr));
	needInit = false;
}
