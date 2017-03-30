#include "Client.h"

util::Client::Client()
{
}

util::Client::~Client()
{
	
}

void util::Client::finalize()
{
	WaitForSingleObject(_recieveThread, 1000);
	util::Communicator::finalize();
}

bool util::Client::onConnect(addrinfo * result)
{
	// Connect to server.
	int iResult = ::connect(_innerSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(_innerSocket);
		_innerSocket = INVALID_SOCKET;		
		return false;
	}
	
	// start recieve thread
	_isConnected = true;
	_recieveThread = CreateThread(NULL, 0, recieveThread, this, CREATE_ALWAYS, 0);

	return true;
}

void util::Client::recieveImplementation()
{
	do
	{
		if (!isConnected())
			continue;

		char recvbuf[DEFAULT_BUFLEN];
		int iResult = recv(_innerSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0)
			notifyDataRecieved(recvbuf, iResult);
		else if (iResult == 0)
			_isClosed = true;


	} while (!_isClosed);
}

DWORD util::Client::recieveThread(LPVOID parameter)
{
	util::Client* client = (util::Client*)parameter;
	client->recieveImplementation();

	return 0;
}
