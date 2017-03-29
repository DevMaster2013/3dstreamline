#pragma once

#include "DLLExportDefiniton.h"

namespace util
{
	class RECONUTIL_API Stopwatch
	{
	private:
		class StopwatchImpl* _implemntor;

	public:
		Stopwatch();
		~Stopwatch();

	public:
		void start();
		double getElapsedTime();
	};
}
