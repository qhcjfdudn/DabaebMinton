#include "ServerPCH.h"
#include "ClientProxy.h"

ClientProxy::ClientProxy(const SessionToken& session) :
	_session{ session }
{
}

void ClientProxy::SetSockAddressIfAddressModified(const SockAddress& sockAddress)
{
	if (_sockAddress == sockAddress)
		return;

	_sockAddress = sockAddress;
}
