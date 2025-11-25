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
	auto& networkManagerServer = NetworkManagerServer::GetInstance();

	SessionToken dummySession{ 123123123123, 123 };
	
	EXPECT_EQ(nullptr, networkManagerServer.GetClientProxy(dummySession.GetTokenId()));

	ClientProxy* ci = networkManagerServer.CreateClientProxy(dummySession);
	EXPECT_EQ(ci, networkManagerServer.GetClientProxy(dummySession.GetTokenId()));

	networkManagerServer.RemoveClientProxy(dummySession.GetTokenId());
	ci = nullptr;

	EXPECT_EQ(nullptr, networkManagerServer.GetClientProxy(dummySession.GetTokenId()));
}

// Client 매칭이 성사되면 game을 만든다.
TEST_F(PhysicsEngineFixture, CheckGamesSizeWhenGameCreatedAndDeleted)
{
	auto& networkManagerServer = NetworkManagerServer::GetInstance();

	SessionToken dummySessions[] = { SessionToken{ 123123123123, 123 }, SessionToken{ 234234234234, 234 } };
	EXPECT_EQ(nullptr, networkManagerServer.GetClientProxy(dummySessions[0].GetTokenId()));
	EXPECT_EQ(nullptr, networkManagerServer.GetClientProxy(dummySessions[1].GetTokenId()));

	GameManager& gm = GameManager::GetInstance();
	EXPECT_EQ(gm._games.size(), 0);

	gm.CreateGame(dummySessions);
	EXPECT_EQ(gm._games.size(), 1);

	ClientProxy* c1 = networkManagerServer.GetClientProxy(dummySessions[0].GetTokenId());
	ClientProxy* c2 = networkManagerServer.GetClientProxy(dummySessions[1].GetTokenId());

	gm.RemoveGame(c1);
	EXPECT_EQ(gm._games.size(), 0);

	gm.RemoveGame(c2);
	EXPECT_EQ(gm._games.size(), 0);

	EXPECT_TRUE(networkManagerServer.RemoveClientProxy(dummySessions[1].GetTokenId()));
	EXPECT_FALSE(networkManagerServer.RemoveClientProxy(dummySessions[1].GetTokenId()));
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