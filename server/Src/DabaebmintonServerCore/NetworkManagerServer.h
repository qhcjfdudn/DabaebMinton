#pragma once

#include "IOCPEvent.h"
#include "Socket.h"
#include "ReplicationManager.h"
#include "LinkingContext.h"
#include "SockAddress.h"

class GameObject;
class ClientInfo;
class Packet;

class NetworkManagerServer
{
public:
	static NetworkManagerServer& GetInstance();

	void InitIOCP();
	void AcceptEx();
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

	void ProcessReceivePacket();

	GameObject* GetGameObject(const NetworkId_t networkId) const;
	NetworkId_t RegisterGameObject(shared_ptr<GameObject> gameObject);
	void UnregisterGameObject(const NetworkId_t networkId);
	void ClearAllGameObjects();

	ClientInfo* CreateClientInfo(std::string_view ip, const uint16_t port);
	bool RemoveClientInfo(ClientInfo* clientInfo);
	ClientInfo* GetClientInfo(std::string_view ip, const uint16_t port);

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

	// IOCP
	WSADATA m_wsa;
	HANDLE mh_iocp = nullptr;
	int m_threadCount = 1;

	char m_lpOutputBuf[1024] = { 0, };
	DWORD m_dwBytes{};

	IOCPEvent m_iocpEvent{};
	DWORD m_timeoutMs{ 100 };

	Socket m_listenSocket{};
	Socket m_clientCandidateSocket{};	// accept target for TCP socket
	Socket m_rudpSocket{};				// for RUDP

	OverlappedDto _sendOverlappedDtoPool[1 << 3]; // arbitrary size.
	unordered_map<ULONG_PTR, int> _lpOverlappedToSendOverlappedDtoPoolIdxMap;
	queue<int> _sendOverlappedQueue; // mutex needed

	queue<shared_ptr<InputMemoryBitStream>> _receivedQueue;

	unordered_map<SockAddress, shared_ptr<ClientInfo> > _sockAddressToClientInfoMap;
	std::mutex _sockAddressToClientInfoMapMutex;

	LinkingContext _linkingContext{};
};
