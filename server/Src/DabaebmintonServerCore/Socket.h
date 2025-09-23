#pragma once

#include "SockAddress.h"
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
	
	SOCKET m_socket{};

	SockAddress _remoteAddr{ "0.0.0.0", 0 };

	OverlappedDto _sendOverlappedDto{};
	OverlappedDto _recvOverlappedDto{};

private:
	ULONG_PTR _completionKey;

	SocketProtocolType m_protocolType = SocketProtocolType::NONE;
};
