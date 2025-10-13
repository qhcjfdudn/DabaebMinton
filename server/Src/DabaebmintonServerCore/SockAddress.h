#pragma once

class SockAddress
{
public:
	SockAddress() { ZeroMemory(&m_sockaddr, sizeof(m_sockaddr)); }
	SockAddress(const char* ip, const uint16_t port);
	SockAddress(const sockaddr& sockaddr);
	SockAddress(const SockAddress& rhs);

	sockaddr& GetSockAddr() { return m_sockaddr; }
	const sockaddr& GetSockAddr() const { return m_sockaddr; }

	bool operator==(const SockAddress& other) const
	{
		return (m_sockaddr.sa_family == other.m_sockaddr.sa_family &&
			GetSockAddrIn()->sin_addr.S_un.S_addr == other.GetSockAddrIn()->sin_addr.S_un.S_addr &&
			GetSockAddrIn()->sin_port == other.GetSockAddrIn()->sin_port);
	}

	size_t GetHash() const
	{
		// IP + Port + Family를 조합
		size_t h1 = std::hash<int>()(GetSockAddrIn()->sin_family);
		size_t h2 = std::hash<uint16_t>()(GetSockAddrIn()->sin_port);
		size_t h3 = std::hash<uint32_t>()(GetSockAddrIn()->sin_addr.S_un.S_addr);

		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}

	const char* GetIP() const;
	const uint16_t GetPort() const;

	bool IsValid() const { return GetSockAddrIn()->sin_port != 0; }

private:
	const sockaddr_in* GetSockAddrIn() const { return reinterpret_cast<const sockaddr_in*>(&m_sockaddr); }
	sockaddr_in* GetSockAddrIn() { return reinterpret_cast<sockaddr_in*>(&m_sockaddr); }

	sockaddr m_sockaddr;
};

namespace std {
	template<>
	struct hash<SockAddress> {
		size_t operator()(const SockAddress& addr) const noexcept
		{
			return addr.GetHash();
		}
	};
}
