#include "ServerPCH.h"
#include "ClientInfo.h"

ClientInfo::ClientInfo(const string& ip, const unsigned int port) :
	_ipPort{ ip + ":" + std::to_string(port) }
{
}
