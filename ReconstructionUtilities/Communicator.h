#pragma once

#include "DLLExportDefiniton.h"
#include "ICommunicatorObserver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 4096

namespace util
{
	class RECONUTIL_API Communicator
	{
	protected:			
		SOCKET _innerSocket;
		bool _isClosed;
		bool _isConnected;
		std::vector<ICommunicatorObserver*>* _observers;

	public:
		Communicator();
		virtual ~Communicator();

	public:
		virtual bool initialize();
		virtual bool connect(const char* ip, const char* port);		
		virtual void finalize();	
		virtual void send(void* data, size_t dataLength);

	protected:
		virtual bool onConnect(struct addrinfo *result) = 0;
		virtual void notifyDataRecieved(void * data, size_t dataLength);

	public:
		bool isConnected() const;

	public:
		void addObserver(ICommunicatorObserver* observer);
		void removeObserver(ICommunicatorObserver* observer);	
	};
}