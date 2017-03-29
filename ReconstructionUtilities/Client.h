#pragma once

#include "Communicator.h"

namespace util
{
	class RECONUTIL_API Client : public Communicator
	{
	private:
		HANDLE _listenThread;

	public:
		Client();
		~Client();

	public:
		virtual bool initialize() override;
		virtual bool connect(const char* ip, const char* port) override;
		virtual void finalize() override;

	private:
		void notifyAllObservers(void * data, size_t dataLength);

	private:
		void listenForServer();			

	private:
		static DWORD WINAPI listenThread(LPVOID parameter);

	};
}
