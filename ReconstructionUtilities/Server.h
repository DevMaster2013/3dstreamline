#pragma once

#include "Communicator.h"

namespace util
{

#define MAX_CLIENTS 10

	class RECONUTIL_API Server : public Communicator
	{
	private:
		int _currentClientFreeSlot;
		SOCKET _clientSockets[MAX_CLIENTS];
		HANDLE _listenThread;

	public:
		Server();
		~Server();

	public:
		virtual bool initialize() override;
		virtual bool connect(const char* ip, const char* port) override;
		virtual void finalize() override;

	private:
		void notifyAllObservers(SOCKET clientSocke);

	public:
		void send(void* data, size_t dataLength);

	private:
		void listenToClients();

	private:
		static DWORD WINAPI listenThread(LPVOID parameter);
	};
}
