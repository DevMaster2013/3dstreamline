#include "Server.h"

util::Server::Server()
	: _currentClientFreeSlot(0)
{	
}

util::Server::~Server()
{
	
}

bool util::Server::initialize()
{
	if (!Communicator::initialize())
		return false;

	for (int slot = 0; slot < MAX_CLIENTS; slot++)
		_clientSockets[slot] = INVALID_SOCKET;

	return true;
}

bool util::Server::connect(const char * ip, const char * port)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		return false;
	}

	// Create a SOCKET for the server to listen for client connections
	_innerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (_innerSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind(_innerSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(_innerSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	// start listen thread
	_listenThread = CreateThread(NULL, 0, listenThread, this, CREATE_ALWAYS, 0);
	_isConnected = true;

	return true;
}

void util::Server::finalize()
{
	for (int slot = 0; slot < _currentClientFreeSlot; slot++)
	{
		int iResult = shutdown(_clientSockets[slot], SD_SEND);
		closesocket(_clientSockets[slot]);
	}

	_isClosed = true;
	WaitForSingleObject(_listenThread, 1000);

	Communicator::finalize();
}

void util::Server::notifyAllObservers(SOCKET clientSocke)
{
	int namelen = 0;
	getsockname(clientSocke, nullptr, &namelen);
	sockaddr* address = (sockaddr*)(new char[namelen]);
	getsockname(clientSocke, address, &namelen);

	for each (auto& observer in *_observers)
		observer->onClinetConnected(address->sa_data);
	delete address;
}

void util::Server::send(void * data, size_t dataLength)
{
	for (int slot = 0; slot < _currentClientFreeSlot; slot++)
		::send(_clientSockets[slot], (char*)data, (int)dataLength, 0);
}

void util::Server::listenToClients()
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

		// Add the clinet socket to the sockets list
		if (_currentClientFreeSlot < MAX_CLIENTS)
		{
			_clientSockets[_currentClientFreeSlot++] = clientSocket;

		}

		notifyAllObservers(clientSocket);

	} while (!_isClosed);	
}

DWORD WINAPI util::Server::listenThread(LPVOID parameter)
{
	util::Server* server = (util::Server*)parameter;
	server->listenToClients();

	return 0;
}
