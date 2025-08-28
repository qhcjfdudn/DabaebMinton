#include "ServerPCH.h"
#include "ClientInfo.h"

ClientInfo::ClientInfo(ULONG_PTR completionKey, const sockaddr_in& addr)
{
	_completionKey = completionKey;
	_addr = addr;
}
