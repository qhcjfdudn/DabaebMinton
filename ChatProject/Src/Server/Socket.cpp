#include "ServerPCH.h"
#include "Socket.h"

SOCKET Socket::CreateWSASocket()
{
	return WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		nullptr,
		0,
		WSA_FLAG_OVERLAPPED);
}

int Socket::BindServer(unsigned int port) {
	sockaddr_in s_in = {};
	s_in.sin_family = AF_INET;
	s_in.sin_addr.S_un.S_addr = INADDR_ANY;
	s_in.sin_port = htons(port);

	return bind(m_socket, reinterpret_cast<sockaddr*>(&s_in), sizeof(s_in));
}

int Socket::Recv()
{
	WSABUF b;
	b.buf = m_receiveBuffer;
	b.len = MAX_RECEIVE_LENGTH;

	// overlapped I/O가 진행되는 동안 여기 값이 채워집니다.
	m_readFlags = 0;

	int retCode = WSARecv(
		m_socket,
		&b,
		1,
		NULL,
		&m_readFlags,
		&m_readOverlappedStruct,
		NULL);

	return retCode;
}
