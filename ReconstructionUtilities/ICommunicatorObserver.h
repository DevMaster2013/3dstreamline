#pragma once

#include "DLLExportDefiniton.h"

namespace util
{
	class RECONUTIL_API ICommunicatorObserver
	{
	public:
		virtual void onDataRecieved(void* data, size_t dataLength) = 0;
		virtual void onClinetConnected() = 0;
	};
}
