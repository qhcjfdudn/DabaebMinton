#include "pch.h"

#include <WinSock2.h>
#include <mswsock.h>

#include "PhysicsEngine.h"
#include "NetworkManagerServer.h"
#include "GameManager.h"
#include "ClientInfo.h"

class PhysicsEngineFixture : public ::testing::Test {
protected:
	virtual void SetUp() override
	{
		physicsEngine.InitPhysics();
	}
	virtual void TearDown() override
	{
		physicsEngine.CleanupPhysics();
	}

	PhysicsEngine& physicsEngine = PhysicsEngine::GetInstance();
};

TEST(ClientInfo, CheckAfterClientInfoCreationAndElimination)
{
	string clientIp = "123.123.123.123";
	const unsigned int clientPort = 12345;

	auto& networkManagerServer = NetworkManagerServer::GetInstance();
	
	EXPECT_EQ(nullptr, networkManagerServer.GetClientInfo(clientIp, clientPort));

	ClientInfo* ci = networkManagerServer.CreateClientInfo(clientIp, clientPort);
	EXPECT_EQ(ci, networkManagerServer.GetClientInfo(clientIp, clientPort));

	networkManagerServer.RemoveClientInfo(ci);
	ci = nullptr;

	EXPECT_EQ(nullptr, networkManagerServer.GetClientInfo(clientIp, clientPort));
}

// Client 매칭이 성사되면 game을 만든다.
TEST_F(PhysicsEngineFixture, CheckGamesSizeWhenGameCreatedAndDeleted)
{
	string clientIps[2] = { "123.123.123.123", "124.124.124.124" };
	const unsigned int clientPorts[2] = { 12345, 23456 };

	auto& networkManagerServer = NetworkManagerServer::GetInstance();
	EXPECT_EQ(networkManagerServer.GetClientInfo(clientIps[0], clientPorts[0]), nullptr);
	EXPECT_EQ(networkManagerServer.GetClientInfo(clientIps[1], clientPorts[1]), nullptr);

	GameManager& gm = GameManager::GetInstance();
	EXPECT_EQ(gm._games.size(), 0);

	gm.CreateGame(clientIps, clientPorts);
	EXPECT_EQ(gm._games.size(), 1);

	ClientInfo* c1 = networkManagerServer.GetClientInfo(clientIps[0], clientPorts[0]);
	ClientInfo* c2 = networkManagerServer.GetClientInfo(clientIps[1], clientPorts[1]);

	gm.RemoveGame(c1);
	EXPECT_EQ(gm._games.size(), 0);

	gm.RemoveGame(c2);
	EXPECT_EQ(gm._games.size(), 0);
}