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

void Socket::SetSendBuffer(const char* str, size_t len)
{
	memcpy(m_sendBuffer, str, len);
}
