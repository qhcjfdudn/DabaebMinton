#include "ServerPCH.h"
#include "ClientInfo.h"

ClientInfo::ClientInfo(const string& ip, const unsigned int port) :
	_ipPort{ ip + ":" + std::to_string(port) }
{
	ZeroMemory(&_sendOverlappedDto, sizeof(_sendOverlappedDto));
	ZeroMemory(&_sockAddr, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	_sockAddr.sin_port = htons(port);
}
