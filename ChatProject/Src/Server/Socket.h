#pragma once
class Socket
{
public:
	static const int MAX_RECEIVE_LENGTH = 8'192;
	
	static SOCKET CreateWSASocket();
	
	char m_receiveBuffer[MAX_RECEIVE_LENGTH + 1] = { 0, };
	DWORD m_numberOfBytesReceived{};
	DWORD m_readFlags{};
	OVERLAPPED m_readOverlappedStruct{};

	SOCKET m_socket{};

	int BindServer(unsigned int port);
};
