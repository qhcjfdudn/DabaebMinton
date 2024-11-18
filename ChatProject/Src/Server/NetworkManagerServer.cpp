#include "ServerPCH.h"

#include "NetworkManagerServer.h"

NetworkManagerServer& NetworkManagerServer::GetInstance() {
	static NetworkManagerServer sInstance;
	return sInstance;
}

NetworkManagerServer::NetworkManagerServer() {
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "WSAStartup failed" << endl;
	}
	cout << "WSAStartup" << endl;
}

NetworkManagerServer::~NetworkManagerServer() {
	WSACleanup();
	cout << "WSACleanup" << endl;
}

void NetworkManagerServer::Init() {
	
}