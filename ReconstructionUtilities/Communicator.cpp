#include "Communicator.h"

util::Communicator::Communicator()
	: _innerSocket(INVALID_SOCKET)
	, _isClosed(false)
	, _isConnected(false)
	, _observers(new std::vector<ICommunicatorObserver*>())
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

	return true;
}

void util::Communicator::finalize()
{
	// cleanup
	if (_innerSocket != INVALID_SOCKET)
		closesocket(_innerSocket);
	WSACleanup();	

	_observers->clear();
	_isConnected = false;
	delete _observers;
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
