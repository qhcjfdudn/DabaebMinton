#include "clientPCH.h"
#include "NetworkManagerClient.h"

NetworkManagerClient& NetworkManagerClient::GetInstance() {
	static NetworkManagerClient sInstance;
	return sInstance;
}

NetworkManagerClient::NetworkManagerClient() {
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "WSAStartup failed" << endl;
	}
	cout << "WSAStartup" << endl;
}

NetworkManagerClient::~NetworkManagerClient() {
	WSACleanup();
	cout << "WSACleanup" << endl;
}

void NetworkManagerClient::Init() {
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in remote_sockaddr = {};
	remote_sockaddr.sin_family = AF_INET;
	remote_sockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	remote_sockaddr.sin_port = htons(50000);

	if (connect(s, reinterpret_cast<sockaddr*>(&remote_sockaddr), sizeof(remote_sockaddr))
		== SOCKET_ERROR) {
		cout << "connect error: " << WSAGetLastError() << endl;
		return;
	}
	
	while (true) {
		char msg[20] = {};
		if (gets_s(msg, sizeof(msg) - 1) == nullptr) {
			cout << "너무 길다. 다시 입력." << endl;
			continue;
		}

		if (send(s, msg, static_cast<int>(strlen(msg)), 0) == SOCKET_ERROR) {
			cout << "send error: " << WSAGetLastError() << endl;
		}

		if (strcmp(msg, "quit") == 0)
			break;
	}

	//memset(msg, 0, sizeof(msg));
	//if (recv(s, msg, sizeof(msg), 0) == SOCKET_ERROR) {
	//	cout << "recv error: " << WSAGetLastError() << endl;
	//}

	//cout << msg << endl;

	if (closesocket(s) == SOCKET_ERROR) {
		cout << "closesocket error: " << WSAGetLastError() << endl;
		return;
	}
}