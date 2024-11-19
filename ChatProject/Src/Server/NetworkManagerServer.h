#pragma once

class NetworkManagerServer
{
public:

	static NetworkManagerServer& GetInstance();
	void Init();

	void ReceivePackets();
	void SendPackets();

	std::thread* acceptSocketThread;
	std::list<SOCKET> clientSockets;

private:
	NetworkManagerServer();
	~NetworkManagerServer();

	WSADATA wsa;
};