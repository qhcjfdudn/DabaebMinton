#pragma once

#include "IOCPEvent.h"

class NetworkManagerServer
{
public:

	static NetworkManagerServer& GetInstance();

	void Init();
	void ReceivePackets();
	void SendPackets();

	void InitIOCP();
	void ReceivePacketsIOCP();
	bool GetCompletionStatus();

	void closeSockets();

	std::thread* m_acceptSocketThread;

	std::list<SOCKET> m_clientSockets;
	std::vector<std::list<SOCKET>::iterator> m_clientSocketItersToErase;

	LPFN_ACCEPTEX m_AcceptEx;

	static const int MAX_IOCP_EVENT_COUNT = 1000;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

	WSADATA m_wsa;
	HANDLE mh_iocp;
	int m_threadCount;
	OVERLAPPED m_readOverlappedStruct;

	IOCPEvent iocpEvent;
	DWORD timeoutMs = 100;
};