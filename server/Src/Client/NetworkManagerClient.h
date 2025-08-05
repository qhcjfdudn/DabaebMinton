#pragma once
class NetworkManagerClient
{
public:

	static NetworkManagerClient& GetInstance();

	void Init();

private:
	NetworkManagerClient();
	~NetworkManagerClient();

	//SOCKET mServerSocket;
	WSADATA wsa;
};