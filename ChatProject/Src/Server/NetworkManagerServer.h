#pragma once

class NetworkManagerServer
{
public:
	static NetworkManagerServer* GetInstance();
	
	void Init();

private:
	NetworkManagerServer() {}

	//SOCKET mServerSocket;
	//WSADATA wsa;
};