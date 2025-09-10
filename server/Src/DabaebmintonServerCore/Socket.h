#pragma once

#include "Constant.h"

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
	
	void SetSendBuffer(const unsigned char* str, size_t len);

	SocketProtocolType GetProtocolType() const { return m_protocolType; }
	void SetProtocolType(SocketProtocolType protocolType) { m_protocolType = protocolType; }

	char m_receiveBuffer[Constant::MAX_PACKET_SIZE + 1] = { 0, };
	DWORD m_numberOfBytesReceived{};
	DWORD m_receiveFlags{};
	OVERLAPPED m_receiveOverlappedStruct{};
	
	char m_sendBuffer[Constant::MAX_PACKET_SIZE + 1] = { 0, };
	DWORD m_numberOfBytesSent{};
	DWORD m_sendFlags{};
	OVERLAPPED m_sendOverlappedStruct{};
	
	SOCKET m_socket{};

	sockaddr_in m_remoteAddr;
	int lpFromLen;

	void SetRemoteAddress(const sockaddr_in& remoteAddr) {
		m_remoteAddr = remoteAddr;
	}

private:
	SocketProtocolType m_protocolType = SocketProtocolType::NONE;
};