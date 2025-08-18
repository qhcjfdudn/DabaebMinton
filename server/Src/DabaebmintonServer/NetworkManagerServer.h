#pragma once

#include "IOCPEvent.h"
#include "Socket.h"

class NetworkManagerServer
{
public:
	static NetworkManagerServer& GetInstance();

	void InitIOCP();
	void AcceptEx();
	void GetAcceptExSockAddrs(shared_ptr<Socket> client);
	void ProcessIOCPEvent();
	bool GetCompletionStatus();
	bool ProcessAcceptedClientSocketIOCP();
	HANDLE AddSocketIOCP(std::shared_ptr<Socket> clientSocket, const ULONG_PTR completionKey);
	void SendPacketsIOCP();
	void ReceivePacketsIOCP(std::shared_ptr<Socket> p_clientSocket, unsigned int receivedBytes);
	int Send(shared_ptr<Socket> clientSocket, size_t len);
	int Recv(shared_ptr<Socket> clientSocket);
	int SendTo(shared_ptr<Socket> clientSocket, size_t len);
	int RecvFrom(shared_ptr<Socket> clientSocket);

	LPFN_ACCEPTEX m_AcceptEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS m_GetAcceptExSockAddrs = nullptr;

	static const int MAX_IOCP_EVENT_COUNT = 1000;

	std::unordered_map<ULONG_PTR, shared_ptr<Socket>> m_clientsMap;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

	void CreateListenSocket();
	
	void GetLPFN();

	WSADATA m_wsa;
	HANDLE mh_iocp = nullptr;
	int m_threadCount = 1;

	char m_lpOutputBuf[1024] = { 0, };
	DWORD m_dwBytes{};

	IOCPEvent m_iocpEvent{};
	DWORD m_timeoutMs{ 100 };

	Socket m_listenSocket{};
	Socket m_clientCandidateSocket{}; // accept target socket
};