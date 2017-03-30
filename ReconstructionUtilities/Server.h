#pragma once

#include "Communicator.h"

namespace util
{

	class RECONUTIL_API Server : public Communicator
	{
	private:
		HANDLE _listenThread;
		std::vector<SOCKET>* _connectedSockets;
		std::vector<HANDLE>* _recievingHandles;

	public:
		Server();
		~Server();

	public:
		virtual bool initialize() override;
		virtual void finalize() override;
		virtual void send(void* data, size_t dataLength) override;

	protected:		
		virtual bool onConnect(struct addrinfo *result) override;
		virtual void listenImplementation();
		virtual void recieveImplementation(SOCKET client);

	private:
		void notifyClientConnected(SOCKET clientSocke);

	private:
		static DWORD WINAPI listenThread(LPVOID parameter);
		static DWORD WINAPI recieveThread(LPVOID parameter);
	};
}
