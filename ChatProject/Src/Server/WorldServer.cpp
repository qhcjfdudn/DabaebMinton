#include "ServerPCH.h"
#include "WorldServer.h"

WorldServer& WorldServer::GetInstance() {
	static WorldServer instance;
	return instance;
}

void WorldServer::Update() {

}