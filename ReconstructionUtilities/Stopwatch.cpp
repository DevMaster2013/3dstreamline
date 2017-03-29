#include "Stopwatch.h"
#include <Windows.h>

namespace util
{
	class StopwatchImpl
	{
	private:
		LARGE_INTEGER _frequency;
		LARGE_INTEGER _currentTime;

	public:
		StopwatchImpl()
		{
			QueryPerformanceFrequency(&_frequency);
		}

	public:
		void start()
		{
			QueryPerformanceCounter(&_currentTime);
		}

		double getElapsedTime()
		{
			LARGE_INTEGER temp;
			QueryPerformanceCounter(&temp);
			return double(temp.QuadPart - _currentTime.QuadPart) / double(_frequency.QuadPart);
		}
	};
}

util::Stopwatch::Stopwatch()
	: _implemntor(new StopwatchImpl())
{
}

util::Stopwatch::~Stopwatch()
{
	delete _implemntor;
}

void util::Stopwatch::start()
{
	_implemntor->start();
}

double util::Stopwatch::getElapsedTime()
{
	return _implemntor->getElapsedTime();
}
