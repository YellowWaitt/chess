#include "chrono.h"

#include <chrono>

using namespace std::chrono;


struct Chrono::Private {
	steady_clock::time_point start;
	duration<double> time;
	bool running;
};


Chrono::Chrono(bool start)
: d(std::make_shared<Private>())
{
	d->time = duration<double>(0);
	d->start = start ? steady_clock::now() : steady_clock::time_point();
	d->running = start;
}

bool Chrono::isRunning() const {
	return d->running;
}

double Chrono::time() const {
	duration<double> time(d->time);
	if (d->running) {
		steady_clock::time_point now = steady_clock::now();
		time += duration_cast<duration<double>>(now - d->start);
	}
	return time.count();
}

void Chrono::pause() {
	if (d->running) {
		steady_clock::time_point now = steady_clock::now();
		d->time += duration_cast<duration<double>>(now - d->start);
		d->running = false;
	}
}

void Chrono::reset() {
	d->time = duration<double>(0);
	d->running = false;
}

void Chrono::resume() {
	if (not d->running) {
		d->start = steady_clock::now();
		d->running = true;
	}
}

void Chrono::start() {
	d->time = duration<double>(0);
	d->start = steady_clock::now();
	d->running = true;
}
