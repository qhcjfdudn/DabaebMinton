#pragma once

#include "IOCPEvent.h"
#include "Socket.h"
#include "ReplicationManager.h"
#include "LinkingContext.h"
#include "SockAddress.h"
#include "ReceivedPacket.h"
#include "RPCManager.h"
#include "SessionToken.h"

class GameObject;
class ClientProxy;
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
	
	int SendTo(ClientProxy* client, const OutputMemoryBitStream& stream);
	int SendTo(ClientProxy* client);
	int RecvFrom();
	
	void SendOutgoingPackets();
	void SendReplicationStatePacketToClient(ClientProxy* client);
	void SendRpcPacketToClient(ClientProxy* client);

	void ProcessQueuedPackets();
	void ProcessPacket(InputMemoryBitStream& inStream, const SockAddress& clientSockAddress);

	GameObject* GetGameObject(const NetworkId_t networkId) const;
	NetworkId_t RegisterGameObject(shared_ptr<GameObject> gameObject);
	void UnregisterGameObject(const NetworkId_t networkId);
	void ClearAllGameObjects();

	ClientProxy* GetClientProxy(const SessionId_t sessionId);
	ClientProxy* CreateClientProxy(const SessionToken session);
	bool RemoveClientProxy(const SessionId_t sessionId);

	SessionToken GetSessionToken(const SessionId_t sessionId) const;

	RPCManager& GetRpcManager() { return _rpcManager; }

	void SendWelcomePacket(ClientProxy* clientProxy);
	void ProcessRPCs(InputMemoryBitStream& inStream, ClientProxy* client);

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

	queue<ReceivedPacket> _receivedQueue;

	unordered_map<SessionId_t, shared_ptr<ClientProxy> > _sessionIdToClientProxyMap;
	shared_mutex _sessionIdToClientProxyMapMutex;

	LinkingContext _linkingContext{};

	RPCManager _rpcManager{};
};
