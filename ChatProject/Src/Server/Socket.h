#pragma once
class Socket
{
public:
	static const int MAX_RECEIVE_LENGTH = 8'192;
	
	static SOCKET CreateWSASocket();
	
	char m_receiveBuffer[MAX_RECEIVE_LENGTH + 1] = { 0, };
	OVERLAPPED m_readOverlappedStruct{};
	DWORD m_readFlags{};

	SOCKET m_socket{};

	int BindServer(unsigned int port);
	int Recv();
};
