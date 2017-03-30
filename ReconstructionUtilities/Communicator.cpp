#include "Communicator.h"

util::Communicator::Communicator()
	: _innerSocket(INVALID_SOCKET)
	, _isClosed(false)
	, _isConnected(false)
{		
}

util::Communicator::~Communicator()
{
}

bool util::Communicator::initialize()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
		return false;

	_observers = new std::vector<ICommunicatorObserver*>();

	return true;
}

bool util::Communicator::connect(const char * ip, const char * port)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;

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

	if (!onConnect(result))
		return false;

	if (_innerSocket == INVALID_SOCKET) {
		WSACleanup();
		return false;
	}
	freeaddrinfo(result);

	_isConnected = true;
	return true;
}

void util::Communicator::finalize()
{
	// cleanup
	if (_innerSocket != INVALID_SOCKET)
	{
		shutdown(_innerSocket, SD_BOTH);
		closesocket(_innerSocket);
	}
	WSACleanup();	

	_observers->clear();
	_isConnected = false;

	delete _observers;
}

void util::Communicator::send(void * data, size_t dataLength)
{
	int iResult = ::send(_innerSocket, (char*)data, (int)dataLength, 0);
	if (iResult == SOCKET_ERROR)
		return;
}

bool util::Communicator::isConnected() const
{
	return _isConnected;
}

void util::Communicator::addObserver(ICommunicatorObserver * observer)
{
	auto found = std::find(_observers->begin(), _observers->end(), observer);
	if (found == _observers->end())
		_observers->push_back(observer);
}

void util::Communicator::removeObserver(ICommunicatorObserver * observer)
{
	auto found = std::find(_observers->begin(), _observers->end(), observer);
	if (found == _observers->end())
		_observers->erase(found);
}

void util::Communicator::notifyDataRecieved(void * data, size_t dataLength)
{
	for each (auto& observer in *_observers)
		observer->onDataRecieved(data, dataLength);
}
