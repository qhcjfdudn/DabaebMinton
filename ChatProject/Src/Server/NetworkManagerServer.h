#pragma once

#include "IOCPEvent.h"
#include "Socket.h"

class NetworkManagerServer
{
public:

	static NetworkManagerServer& GetInstance();

	void Init();
	void ReceivePackets();
	void SendPackets();

	void InitIOCP();
	void AcceptEx();
	void ProcessIOCPEvent();
	bool GetCompletionStatus();

	void closeSockets();

	std::thread* m_acceptSocketThread = nullptr;

	std::list<SOCKET> m_clientSockets;
	std::vector<std::list<SOCKET>::iterator> m_clientSocketItersToErase;

	LPFN_ACCEPTEX m_AcceptEx = nullptr;

	static const int MAX_IOCP_EVENT_COUNT = 1000;

	std::unordered_map<ULONG_PTR, shared_ptr<Socket>> m_clientsMap;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

	WSADATA m_wsa;
	HANDLE mh_iocp = nullptr;
	int m_threadCount = 1;

	char m_lpOutputBuf[1024] = { 0, };
	DWORD m_dwBytes{};
	OVERLAPPED m_readOverlappedStruct{};

	IOCPEvent m_iocpEvent{};
	DWORD m_timeoutMs{ 100 };

	Socket m_listenSocket{};
	Socket m_clientCandidateSocket{};
};