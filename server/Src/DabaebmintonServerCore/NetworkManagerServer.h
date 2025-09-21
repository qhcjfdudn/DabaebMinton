#pragma once

#include "IOCPEvent.h"
#include "Socket.h"
#include "ReplicationManager.h"
#include "LinkingContext.h"

class GameObject;
class ClientInfo;
class Packet;

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
	HANDLE AddSocketIOCP(std::shared_ptr<Socket> socket);
	void ReceivePacketsIOCP(std::shared_ptr<Socket> p_clientSocket, unsigned int receivedBytes);
	
	int Send(shared_ptr<Socket> clientSocket, size_t len);
	int Recv(shared_ptr<Socket> clientSocket);
	
	int SendTo(ClientInfo* client, const OutputMemoryBitStream& stream);
	int SendTo(ClientInfo* client);
	int RecvFrom();
	
	void SendOutgoingPackets();
	void SendReplicationStatePacketToClient(ClientInfo* client);
	void SendRpcPacketToClient(ClientInfo* client);

	bool HasElapsedPacketInterval();
	void SetLastPacketSendTimeToNow();

	GameObject* GetGameObject(const NetworkId_t networkId) const;
	NetworkId_t RegisterGameObject(shared_ptr<GameObject> gameObject);
	void UnregisterGameObject(const NetworkId_t networkId);
	void ClearAllGameObjects();

	ClientInfo* CreateClientInfo(const string& ip, const unsigned int port);
	bool RemoveClientInfo(ClientInfo* clientInfo);
	ClientInfo* GetClientInfo(const string& ip, const unsigned int port);

	LPFN_ACCEPTEX m_AcceptEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS m_GetAcceptExSockAddrs = nullptr;

	static const int MAX_IOCP_EVENT_COUNT = 1000;

	std::unordered_map<ULONG_PTR, shared_ptr<Socket>> m_clientsMap;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

	void InitRudpSendOverlappedPool();
	OverlappedDto& GetNextSendOverlapped();
	void NotifySendOverlappedCompletionEvent(LPOVERLAPPED lpOverlapped);

	void CreateListenSocket();
	void CreateRUDPSocket();
	
	void GetLPFN();

	WSADATA m_wsa;
	HANDLE mh_iocp = nullptr;
	int m_threadCount = 1;

	char m_lpOutputBuf[1024] = { 0, };
	DWORD m_dwBytes{};

	IOCPEvent m_iocpEvent{};
	DWORD m_timeoutMs{ 100 };

	Socket m_listenSocket{};
	Socket m_clientCandidateSocket{}; // accept target TCP socket

	Socket m_rudpSocket{};
	OverlappedDto sendOverlappedDtoPool[1 << 3]; // arbitrary size.
	unordered_map<ULONG_PTR, int> lpOverlappedToSendOverlappedDtoPoolIdxMap;
	queue<int> sendOverlappedQueue; // mutex needed

	system_clock::time_point lastPacketSendTime;

	unordered_map<std::string, shared_ptr<ClientInfo> > _ipPortToClientInfoMap;
	std::mutex _ipPortToClientInfoMapMutex;

	LinkingContext _linkingContext{};
};
