#include "ServerPCH.h"

#include "NetworkManagerServer.h"

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "WSAStartup failed" << endl;
	}

	WSACleanup();

	auto it = NetworkManagerServer::GetInstance();
	it->Init();
}