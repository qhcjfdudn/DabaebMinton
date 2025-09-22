#pragma once
class SockAddress
{
public:
	SockAddress(const char* ip, const uint16_t port);
	SockAddress(const sockaddr& sockaddr);
	SockAddress(const SockAddress& rhs);

	const sockaddr& GetSockAddr() const { return m_sockaddr; }

	bool operator==(const SockAddress& other) const
	{
		return (m_sockaddr.sa_family == other.m_sockaddr.sa_family &&
			GetIP() == other.GetIP() &&
			GetSockaddrIn()->sin_port == other.GetSockaddrIn()->sin_port);
	}

	size_t GetHash() const
	{
		// IP + Port + Family를 조합
		size_t h1 = std::hash<int>()(GetSockaddrIn()->sin_family);
		size_t h2 = std::hash<uint16_t>()(GetSockaddrIn()->sin_port);
		size_t h3 = std::hash<uint32_t>()(GetSockaddrIn()->sin_addr.S_un.S_addr);

		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}

	uint32_t GetIP() const { return *reinterpret_cast<const uint32_t*>(&GetSockaddrIn()->sin_addr.S_un.S_addr); }

private:
	const sockaddr_in* GetSockaddrIn() const { return reinterpret_cast<const sockaddr_in*>(&m_sockaddr); }
	sockaddr_in* GetSockaddrIn() { return reinterpret_cast<sockaddr_in*>(&m_sockaddr); }

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
