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
	void AcceptEx();
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

	SOCKET m_listenSocket;
	SOCKET m_clientCandidateSocket;
	char m_lpOutputBuf[1024];
	DWORD m_dwBytes;
	OVERLAPPED m_readOverlappedStruct;

	IOCPEvent m_iocpEvent;
	DWORD m_timeoutMs = 100;
};