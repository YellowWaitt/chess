#pragma once

#include <memory>

class Chrono {
	public:
		Chrono(bool start = false);
		
		bool isRunning() const;
		double time() const;
		void pause();
		void reset();
		void resume();
		void start();
	
	private:
		struct Private;
		std::shared_ptr<Private> d;
};
