#include "ServerPCH.h"
#include "Socket.h"

SOCKET Socket::CreateWSASocketHandle(SocketProtocolType spt)
{
	int type = -1;
	int protocol = 0;

	switch (spt)
	{
	case SocketProtocolType::SPT_TCP:
		type = SOCK_STREAM;
		protocol = IPPROTO_TCP;
		break;
	case SocketProtocolType::SPT_RUDP:
		type = SOCK_DGRAM;
		protocol = IPPROTO_UDP;
		break;
	default:
		cout << "Invalid SocketProtocolType. Set WSASocket Type to default: RUDP" << endl;
		type = SOCK_DGRAM;
		protocol = IPPROTO_UDP;
	}

	return WSASocket(
		AF_INET,
		type,
		protocol,
		nullptr,
		0,
		WSA_FLAG_OVERLAPPED);
}

int Socket::Bind(const char* const ip, unsigned int port) {
	sockaddr_in s_in = {};
	s_in.sin_family = AF_INET;
	s_in.sin_addr.S_un.S_addr = inet_addr(ip);
	s_in.sin_port = htons(port);
	

	return bind(m_socket, reinterpret_cast<sockaddr*>(&s_in), sizeof(s_in));
}
