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
	bool ProcessAcceptedClientSocketIOCP();
	HANDLE AddSocketIOCP(std::shared_ptr<Socket> clientSocket, const ULONG_PTR completionKey);
	void ReceivePacketsIOCP(std::shared_ptr<Socket> p_clientSocket);
	int Recv(shared_ptr<Socket> clientSocket);
	void SendPacketsIOCP();
	int Send(shared_ptr<Socket> clientSocket, size_t len);

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

	IOCPEvent m_iocpEvent{};
	DWORD m_timeoutMs{ 100 };

	Socket m_listenSocket{};
	Socket m_clientCandidateSocket{};
};