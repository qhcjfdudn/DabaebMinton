#include "ServerPCH.h"

#include "NetworkManagerServer.h"

int main() {
	auto& nmsInstance = NetworkManagerServer::GetInstance();
	nmsInstance.Init();
}