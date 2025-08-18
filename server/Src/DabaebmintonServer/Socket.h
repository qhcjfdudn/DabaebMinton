#pragma once

enum class SocketProtocolType {
	NONE,
	SPT_TCP,
	SPT_RUDP,
	SIZE
};

class Socket
{
public:
	static const int MAX_RECEIVE_LENGTH = 8'192;
	static const int MAX_SEND_LENGTH = 8'192;
	
	static SOCKET CreateWSASocketHandle(SocketProtocolType spt);
	
	int Bind(const char* const ip, unsigned int port);
	
	void SetSendBuffer(const char* str, size_t len);

	SocketProtocolType GetProtocolType() const { return m_protocolType; }
	
	char m_receiveBuffer[MAX_RECEIVE_LENGTH + 1] = { 0, };
	DWORD m_numberOfBytesReceived{};
	DWORD m_readFlags{};
	OVERLAPPED m_readOverlappedStruct{};
	
	char m_sendBuffer[MAX_SEND_LENGTH + 1] = { 0, };
	DWORD m_numberOfBytesSent{};
	DWORD m_sendFlags{};
	OVERLAPPED m_sendOverlappedStruct{};
	
	SOCKET m_socket{};

	sockaddr_in m_remoteAddr;

	void SetRemoteAddress(const sockaddr_in& remoteAddr) {
		m_remoteAddr = remoteAddr;
	}

private:
	SocketProtocolType m_protocolType = SocketProtocolType::NONE;
};