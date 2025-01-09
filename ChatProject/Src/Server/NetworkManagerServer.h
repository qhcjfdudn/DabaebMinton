#pragma once

class NetworkManagerServer
{
public:

	static NetworkManagerServer& GetInstance();
	void Init();
	void InitIOCP();

	void ReceivePackets();
	void SendPackets();

	void closeSockets();

	std::thread* m_acceptSocketThread;

	std::list<SOCKET> m_clientSockets;
	std::vector<std::list<SOCKET>::iterator> m_clientSocketItersToErase;

	LPFN_ACCEPTEX m_AcceptEx;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

	WSADATA m_wsa;
	HANDLE mh_iocp;
	int m_threadCount;
	OVERLAPPED m_readOverlappedStruct;
};