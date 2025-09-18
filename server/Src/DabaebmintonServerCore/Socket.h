#pragma once

#include "Constant.h"
#include "OverlappedDto.h"

enum class SocketProtocolType {
	NONE,
	SPT_TCP,
	SPT_RUDP,
	SIZE
};

class Socket
{
public:
	static SOCKET CreateWSASocketHandle(SocketProtocolType spt);
	
	int Bind(const char* const ip, unsigned int port);
	
	ULONG_PTR GetCompletionKey() const { return _completionKey; }
	void SetCompletionKey(ULONG_PTR completionKey) { _completionKey = completionKey; }

	SocketProtocolType GetProtocolType() const { return m_protocolType; }
	void SetProtocolType(SocketProtocolType protocolType) { m_protocolType = protocolType; }

	void SetRemoteAddress(const sockaddr_in& remoteAddr) { m_remoteAddr = remoteAddr; }

	OverlappedDto _sendOverlappedDto{};
	OverlappedDto _recvOverlappedDto{};
	
	SOCKET m_socket{};

	sockaddr_in m_remoteAddr;

private:
	ULONG_PTR _completionKey;

	SocketProtocolType m_protocolType = SocketProtocolType::NONE;
};