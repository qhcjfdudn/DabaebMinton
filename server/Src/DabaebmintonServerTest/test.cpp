#include "pch.h"

#include <WinSock2.h>
#include <mswsock.h>

#include "PhysicsEngine.h"
#include "NetworkManagerServer.h"
#include "GameManager.h"
#include "ClientProxy.h"
#include "InputMemoryBitStream.h"
#include "OutputMemoryBitStream.h"
#include "GameObject.h"
#include "PacketType.h"
#include "GetRequiredBits.h"

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

TEST(ClientProxy, CheckAfterClientProxyCreationAndElimination)
{
	string clientIp = "123.123.123.123";
	const unsigned int clientPort = 12345;

	auto& networkManagerServer = NetworkManagerServer::GetInstance();
	
	EXPECT_EQ(nullptr, networkManagerServer.GetClientProxy(clientIp, clientPort));

	ClientProxy* ci = networkManagerServer.CreateClientProxy(clientIp, clientPort);
	EXPECT_EQ(ci, networkManagerServer.GetClientProxy(clientIp, clientPort));

	networkManagerServer.RemoveClientProxy(ci);
	ci = nullptr;

	EXPECT_EQ(nullptr, networkManagerServer.GetClientProxy(clientIp, clientPort));
}

// Client 매칭이 성사되면 game을 만든다.
TEST_F(PhysicsEngineFixture, CheckGamesSizeWhenGameCreatedAndDeleted)
{
	string clientIps[2] = { "123.123.123.123", "124.124.124.124" };
	const unsigned int clientPorts[2] = { 12345, 23456 };

	auto& networkManagerServer = NetworkManagerServer::GetInstance();
	EXPECT_EQ(networkManagerServer.GetClientProxy(clientIps[0], clientPorts[0]), nullptr);
	EXPECT_EQ(networkManagerServer.GetClientProxy(clientIps[1], clientPorts[1]), nullptr);

	GameManager& gm = GameManager::GetInstance();
	EXPECT_EQ(gm._games.size(), 0);

	gm.CreateGame(clientIps, clientPorts);
	EXPECT_EQ(gm._games.size(), 1);

	ClientProxy* c1 = networkManagerServer.GetClientProxy(clientIps[0], clientPorts[0]);
	ClientProxy* c2 = networkManagerServer.GetClientProxy(clientIps[1], clientPorts[1]);

	gm.RemoveGame(c1);
	EXPECT_EQ(gm._games.size(), 0);

	gm.RemoveGame(c2);
	EXPECT_EQ(gm._games.size(), 0);

	EXPECT_TRUE(networkManagerServer.RemoveClientProxy(c2));
	EXPECT_FALSE(networkManagerServer.RemoveClientProxy(c2));
}

TEST(MemoryBitStream, SerializeTest)
{
	float fBefore = 3.5f;
	int iBefore = 17;
	PacketType beforePacketType = PacketType::PT_Max;
	char cBefore = 'A';

	OutputMemoryBitStream outStream;
	outStream.Write(fBefore);
	outStream.WriteBits(&iBefore, sizeof(iBefore) << 3);
	outStream.WriteBits(&beforePacketType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
	outStream.WriteBits(&cBefore, sizeof(cBefore) << 3);

	float* fp = reinterpret_cast<float*>(const_cast<unsigned char*>(outStream.GetBufferPtr()));
	EXPECT_EQ(fBefore, *fp); // outStream에 Write() 정상적으로 동작했는지 검사

	EXPECT_EQ(outStream.GetBitLength(), 75);

	float fAfter;
	int iAfter;
	PacketType afterPacketType = static_cast<PacketType>(0);
	char cAfter;

	InputMemoryBitStream inputStream{ outStream.GetBufferPtr(), outStream.GetBitLength() };
	EXPECT_EQ(inputStream.GetBitLength(), outStream.GetBitLength());

	inputStream.ReadBits(&fAfter, sizeof(fAfter) << 3);
	inputStream.ReadBits(&iAfter, sizeof(iAfter) << 3);
	inputStream.ReadBits(&afterPacketType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
	inputStream.ReadBits(&cAfter, sizeof(cAfter) << 3);

	EXPECT_EQ(fBefore, fAfter);
	EXPECT_EQ(iBefore, iAfter);
	EXPECT_EQ(beforePacketType, afterPacketType);
	EXPECT_EQ(cBefore, cAfter);

	EXPECT_TRUE(inputStream.isEndOfStream());
}