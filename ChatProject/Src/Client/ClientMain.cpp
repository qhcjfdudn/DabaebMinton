#include "clientPCH.h"

#include "NetworkManagerClient.h"

int main() {
	auto& nmc = NetworkManagerClient::GetInstance();
	nmc.Init();
}