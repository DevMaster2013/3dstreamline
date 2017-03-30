#pragma once

#include "Communicator.h"

namespace util
{
	class RECONUTIL_API Client : public Communicator
	{
	private:
		HANDLE _recieveThread;

	public:
		Client();
		~Client();

	public:
		virtual void finalize();

	protected:
		virtual bool onConnect(struct addrinfo *result) override;		
		virtual void recieveImplementation();

	protected:
		static DWORD WINAPI recieveThread(LPVOID parameter);
	};
}
