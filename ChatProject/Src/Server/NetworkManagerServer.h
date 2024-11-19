#pragma once

class NetworkManagerServer
{
public:

	static NetworkManagerServer& GetInstance();
	void Init();

	void ReceivePackets();
	void SendPackets();

	void closeSockets();

	std::thread* acceptSocketThread;

	std::list<SOCKET> clientSockets;
	std::vector<std::list<SOCKET>::iterator> clientSocketItersToErase;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

	WSADATA wsa;
};