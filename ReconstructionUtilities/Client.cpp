#include "Client.h"

util::Client::Client()
{
}

util::Client::~Client()
{
	
}

bool util::Client::initialize()
{
	if (!Communicator::initialize())
		return false;

	return true;
}

bool util::Client::connect(const char * ip, const char * port)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(ip, port, &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		return false;
	}

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	_innerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	// Check for errors
	if (_innerSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Connect to server.
	iResult = ::connect(_innerSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(_innerSocket);
		_innerSocket = INVALID_SOCKET;
		return false;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message
	freeaddrinfo(result);

	if (_innerSocket == INVALID_SOCKET) {
		WSACleanup();
		return false;
	}

	// start listen thread
	_listenThread = CreateThread(NULL, 0, listenThread, this, CREATE_ALWAYS, 0);
	_isConnected = true;

	return true;
}

void util::Client::finalize()
{
	_isClosed = true;
	WaitForSingleObject(_listenThread, INFINITE);

	Communicator::finalize();
}

void util::Client::listenForServer()
{
	// Receive data until the server closes the connection
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	do {
		iResult = recv(_innerSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			notifyAllObservers(recvbuf, iResult);
	} while (!_isClosed);
}

DWORD util::Client::listenThread(LPVOID parameter)
{
	util::Client* client = (util::Client*)parameter;
	client->listenForServer();

	return 0;
}

void util::Client::notifyAllObservers(void * data, size_t dataLength)
{
	for each (auto& observer in *_observers)
		observer->onDataRecieved(data, dataLength);
}
