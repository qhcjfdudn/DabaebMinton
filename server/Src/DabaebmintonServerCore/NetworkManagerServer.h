#pragma once

#include "IOCPEvent.h"
#include "Socket.h"
#include "ReplicationManager.h"

class GameObject;
class ClientInfo;

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
	HANDLE AddSocketIOCP(std::shared_ptr<Socket> socket, const ULONG_PTR completionKey);
	void SendPacketsIOCP();
	void ReceivePacketsIOCP(std::shared_ptr<Socket> p_clientSocket, unsigned int receivedBytes);
	int Send(shared_ptr<Socket> clientSocket, size_t len);
	int Recv(shared_ptr<Socket> clientSocket);
	int SendTo(shared_ptr<Socket> clientSocket, size_t len);
	int RecvFrom(shared_ptr<Socket> clientSocket);

	bool HasElapsedPacketInterval();
	void SetLastPacketSendTimeToNow();

	int ReplicateAllGameObjects();
	void AddGameObjectForReplication(GameObject* gameObject);
	void RemoveGameObjectForReplication(GameObject* gameObject);
	void RemoveAllGameObjectsForReplication();

	ClientInfo* CreateClientInfo(const string& ip, const unsigned int port);
	bool RemoveClientInfo(ClientInfo* clientInfo);

	LPFN_ACCEPTEX m_AcceptEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS m_GetAcceptExSockAddrs = nullptr;

	static const int MAX_IOCP_EVENT_COUNT = 1000;

	std::unordered_map<ULONG_PTR, shared_ptr<Socket>> m_clientsMap;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

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
	Socket m_clientCandidateSocket{}; // accept target socket

	Socket m_rudpSocket{};

	system_clock::time_point lastPacketSendTime;

	ReplicationManager _replicationManager;
	vector<GameObject*> _gameObjectsForReplication{};
	queue<GameObject*> _pendingCreatedGameObjectsForReplication{};
	queue<GameObject*> _pendingDeletedGameObjectsForReplication{};

	unordered_map<std::string, shared_ptr<ClientInfo> > _ipPortToClientInfoMap;
	std::mutex _ipPortToClientInfoMapMutex;
};

enum class PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_RPC,
	PT_Disconnect,
	PT_Max
};