#include "ServerPCH.h"

#include "NetworkManagerServer.h"

NetworkManagerServer* NetworkManagerServer::GetInstance() {
	static NetworkManagerServer sInstance;
	return &sInstance;
}

void NetworkManagerServer::Init() {
	
}