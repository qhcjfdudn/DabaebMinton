#include "ServerPCH.h"
#include "ClientInfo.h"

ClientInfo::ClientInfo(std::string_view ip, const uint16_t port) :
	_sockAddress{ ip.data(), port }
{
}

ClientInfo::ClientInfo(const SockAddress& sockAddress) :
	_sockAddress{ sockAddress }
{
}
