#include "pch.h"

#include <WinSock2.h>
#include <mswsock.h>

#include "PhysicsEngine.h"
#include "GameManager.h"
#include "ClientInfo.h"

class GameCreation : public ::testing::Test {
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

// Client 매칭이 성사되면 game을 만든다.
TEST_F(GameCreation, CheckGamesSizeWhenGameCreatedAndDeleted)
{
	GameManager& gm = GameManager::GetInstance();

	sockaddr_in s1{}, s2{};
	ClientInfo c1{ 0, s1 }, c2{ 1, s2 };
	
	EXPECT_EQ(gm._games.size(), 0);

	ULONG_PTR dummyGameKey = 0x123;
	gm.CreateGame(dummyGameKey, &c1, &c2);
	EXPECT_EQ(gm._games.size(), 1);

	ULONG_PTR nonExistGameKey = 0x12356;
	gm.RemoveGame(nonExistGameKey);
	EXPECT_EQ(gm._games.size(), 1);

	gm.RemoveGame(dummyGameKey);
	EXPECT_EQ(gm._games.size(), 0);
}