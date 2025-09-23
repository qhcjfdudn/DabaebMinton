#include "ServerPCH.h"
#include "ClientProxy.h"

ClientProxy::ClientProxy(std::string_view ip, const uint16_t port) :
	_sockAddress{ ip.data(), port }
{
}

ClientProxy::ClientProxy(const SockAddress& sockAddress) :
	_sockAddress{ sockAddress }
{
}
