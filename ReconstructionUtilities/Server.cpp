#include "Server.h"

struct RecieveThreadData
{
public:
	util::Server* server;
	SOCKET clientSocket;
};

util::Server::Server()
{	
}

util::Server::~Server()
{
	
}

bool util::Server::initialize()
{
	if (!Communicator::initialize())
		return false;

	_connectedSockets = new std::vector<SOCKET>();
	_recievingHandles = new std::vector<HANDLE>();

		return true;
}

void util::Server::finalize()
{
	WaitForSingleObject(_listenThread, 1000);
	
	if (_recievingHandles->size() > 0)
		WaitForMultipleObjects(DWORD(_recievingHandles->size()), _recievingHandles->data(), TRUE, 1000);

	delete _connectedSockets;
	delete _recievingHandles;

	util::Communicator::finalize();
}

void util::Server::send(void * data, size_t dataLength)
{
	for each (auto& client in *_connectedSockets)
	{
		int iResult = ::send(client, (char*)data, (int)dataLength, 0);
		if (iResult == SOCKET_ERROR)
			return;
	}
}

bool util::Server::onConnect(addrinfo * result)
{
	// Setup the TCP listening socket
	_connectedSockets->clear();
	result->ai_flags = AI_PASSIVE;
	int iResult = bind(_innerSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(_innerSocket);
		WSACleanup();
		return false;
	}

	// start listen thread
	_listenThread = CreateThread(NULL, 0, listenThread, this, CREATE_ALWAYS, 0);

	return true;
}

void util::Server::listenImplementation()
{
	do
	{
		// Listen on the socket
		if (listen(_innerSocket, SOMAXCONN) == SOCKET_ERROR) {
			continue;
		}

		SOCKET clientSocket;

		// Accept a client socket
		clientSocket = accept(_innerSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			continue;
		}

		RecieveThreadData* threadData = new RecieveThreadData();
		threadData->clientSocket = clientSocket;
		threadData->server = this;

		HANDLE thread = CreateThread(NULL, 0, recieveThread, threadData, CREATE_ALWAYS, 0);
		_recievingHandles->push_back(thread);
		_connectedSockets->push_back(clientSocket);
		_isConnected = _connectedSockets->size() > 0;
		notifyClientConnected(clientSocket);

	} while (!_isClosed);
}

void util::Server::recieveImplementation(SOCKET client)
{
	do
	{
		if (!isConnected())
			continue;

		char recvbuf[DEFAULT_BUFLEN];
		int iResult = recv(client, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0)
			notifyDataRecieved(recvbuf, iResult);
		else if (iResult == 0)
			_isClosed = true;


	} while (!_isClosed);
}

DWORD WINAPI util::Server::listenThread(LPVOID parameter)
{
	util::Server* server = (util::Server*)parameter;
	server->listenImplementation();

	return 0;
}

DWORD util::Server::recieveThread(LPVOID parameter)
{
	RecieveThreadData* threadData = (RecieveThreadData*)parameter;
	threadData->server->recieveImplementation(threadData->clientSocket);
	delete threadData;
	return 0;
}

void util::Server::notifyClientConnected(SOCKET clientSocke)
{
	for each (auto& observer in *_observers)
		observer->onClinetConnected();
}