#include "ServerPCH.h"
#include "Game.h"

#include "NetworkManagerServer.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Constant.h"
#include "ClientInfo.h"
#include "RUDPPacketizer.h"
#include "Packet.h"
#include "GameObject.h"

Game::Game(ClientInfo* player1, ClientInfo* player2) :
	p_player1{ player1 }, p_player2{ player2 },
	_gamePlayState{ GamePlayState::Initializing },
	_level{},
	gameController{}
{
	_level.InitLevel();

	auto& networkManager = NetworkManagerServer::GetInstance();
	for (auto gameObject : _level.gameObjects)
	{
		spdlog::debug("[Game::Game] object: {}", gameObject->GetClassId());

		auto networkId = networkManager.RegisterGameObject(gameObject);		
		player1->GetReplicationManager().ReplicateCreate(networkId, gameObject->GetAllStateMask());
		player2->GetReplicationManager().ReplicateCreate(networkId, gameObject->GetAllStateMask());
	}

	SetNextReplicationTimeFromNow();
}

Game::~Game()
{
	auto& networkManager = NetworkManagerServer::GetInstance();
	for (auto gameObject : _level.gameObjects)
	{
		auto networkId = gameObject->GetNetworkId();
		networkManager.UnregisterGameObject(networkId);
		p_player1->GetReplicationManager().ReplicateDestroy(networkId);
		p_player2->GetReplicationManager().ReplicateDestroy(networkId);
	}
}

bool Game::HasElapsedReplicationInterval()
{
	return system_clock::now() >= _nextReplicationUpdatedTime;
}

void Game::SetNextReplicationTimeFromNow()
{
	std::chrono::duration<float> offset(Constant::REPLICATION_PERIOD);
	_nextReplicationUpdatedTime = system_clock::now() + std::chrono::duration_cast<system_clock::duration>(offset);
}

void Game::SendPacket()
{
	SendPacket(p_player1);
	SendPacket(p_player2);
}

void Game::SendPacket(ClientInfo* client)
{
	// Packet Period가 되었을 때 호출. client별 0~2 channel의 데이터를 보낸다. 
	// 0 channel: Unreliable. ReplicateUpdate.
	// 1 channel: Reliable. ReplicateCreate/Delete.
	// 2 channel: Reliable. RPC.

	auto& networkManagerServer = NetworkManagerServer::GetInstance();
	auto& rudpPacketizer = RUDPPacketizer::GetInstance();
	const PacketType packetTypes[3] = { PacketType::PT_ReplicationData, PacketType::PT_ReplicationData, PacketType::PT_RPC };

	// channel 0
	int channelId = 0;
	auto packets = rudpPacketizer.Packetize(channelId, client->m_sequenceNoInChannels[channelId], packetTypes[channelId], _level.gameObjects);
	networkManagerServer.SendTo(client, packets);

	// channel [1, MAX_CHANNEL_COUNT)
	for (channelId = 1; channelId < Constant::RUDP_MAX_CHANNEL_SIZE; ++channelId)
	{
		OutputMemoryBitStream& outStream = client->m_pendingStreamToSendingInChannels[channelId];

		if (outStream.GetBitLength() == 0)
			continue;
		
		uint8_t& seqNum = client->m_sequenceNoInChannels[channelId];
		packets = rudpPacketizer.PacketizeReliable(channelId, seqNum, packetTypes[channelId], outStream);
		outStream.Clear();

		networkManagerServer.SendTo(client, packets);
	}
}
