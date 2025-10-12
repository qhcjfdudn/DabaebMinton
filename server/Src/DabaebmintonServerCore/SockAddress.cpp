#include "ServerPCH.h"
#include "SockAddress.h"

SockAddress::SockAddress(const char* ip, const uint16_t port)
{
	ZeroMemory(&m_sockaddr, sizeof(m_sockaddr));

	sockaddr_in* s_in = reinterpret_cast<sockaddr_in*>(&m_sockaddr);
	s_in->sin_family = AF_INET;
	s_in->sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &s_in->sin_addr.S_un.S_addr) != 1)
	{
		spdlog::error("[Socket::Bind] inet_pton failed: {}", WSAGetLastError());
	}
}

SockAddress::SockAddress(const sockaddr& sockaddr)
{
	memcpy(&m_sockaddr, &sockaddr, sizeof(m_sockaddr));
}

SockAddress::SockAddress(const SockAddress& rhs) :
	SockAddress(rhs.m_sockaddr)
{
}

const char* SockAddress::GetIP() const
{ 
	return inet_ntoa(reinterpret_cast<const sockaddr_in*>(&m_sockaddr)->sin_addr);
}

const uint16_t SockAddress::GetPort() const
{
	return ntohs(reinterpret_cast<const sockaddr_in*>(&m_sockaddr)->sin_port);
}