#include "ServerPCH.h"
#include "Game.h"

#include "NetworkManagerServer.h"
#include "Constant.h"
#include "ClientInfo.h"
#include "RUDPPacketizer.h"
#include "Packet.h"

Game::Game(ClientInfo* player1, ClientInfo* player2) :
	p_player1{ player1 }, p_player2{ player2 },
	_gamePlayState{ GamePlayState::Initializing },
	_level{},
	gameController{},
	_lastReplicationUpdatedTime{ system_clock::now() }
{
	_level.InitLevel();

	OutputMemoryBitStream stream;
	for (auto gameObject : _level.gameObjects)
		ReplicationManager::GetInstance().ReplicateCreate(stream, _linkingContext, gameObject.get());
}

bool Game::HasElapsedReplicationInterval()
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _lastReplicationUpdatedTime;

	return elapsedTime.count() >= Constant::PACKET_PERIOD;
}

void Game::SetLastReplicationTimeToNow()
{
	_lastReplicationUpdatedTime = system_clock::now();
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
		uint8_t& seqNum = client->m_sequenceNoInChannels[channelId];
		OutputMemoryBitStream& inStream = client->m_pendingStreamToSendingInChannels[channelId];
		auto packets = rudpPacketizer.PacketizeReliable(channelId, seqNum, packetTypes[channelId], inStream);
		inStream.Clear();

		networkManagerServer.SendTo(client, packets);
	}
}
