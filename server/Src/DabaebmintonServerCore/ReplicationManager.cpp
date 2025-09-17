#include "ServerPCH.h"
#include "ReplicationManager.h"

#include "NetworkManagerServer.h"
#include "GameObject.h"
#include "PacketGenerator.h"
#include "ReplicationManagerTransmissionData.h"

#include "GetRequiredBits.h"
#include "BitSizeCounter.h"

void ReplicationManager::ReplicateCreate(int inNetworkId, uint8_t inInitialDirtyState)
{
	mNetworkIdToReplicationCommand[inNetworkId] = ReplicationCommand(inInitialDirtyState);
}

void ReplicationManager::ReplicateDestroy(int inNetworkId)
{
	mNetworkIdToReplicationCommand[inNetworkId].SetDestroy();
}

void ReplicationManager::RemoveFromReplication(int inNetworkId)
{
	mNetworkIdToReplicationCommand.erase(inNetworkId);
}

void ReplicationManager::Write(PacketGenerator& packetGenerator)
{
	for (auto& [networkId, replicationCommand] : mNetworkIdToReplicationCommand)
	{
		if (replicationCommand.HasDirtyState())
		{
			const size_t addBits = CountWriteBitSize(networkId);
			
			auto& outStream = packetGenerator.GenerateNewPacketIfExceedExtraPayloadSize(addBits);

			outStream.Write(networkId);

			ReplicationAction action = replicationCommand.GetAction();
			outStream.WriteBits(&action, GetRequiredBits(static_cast<int>(ReplicationAction::RA_Max)));

			uint8_t writtenState = 0;
			uint8_t dirtyState = replicationCommand.GetDirtyState();

			switch (action)
			{
			case ReplicationAction::RA_Create:
				writtenState = WriteCreateAction(outStream, networkId, dirtyState);
				break;
			case ReplicationAction::RA_Update:
				writtenState = WriteUpdateAction(outStream, networkId, dirtyState);
				break;
			case ReplicationAction::RA_Destroy:
				writtenState = WriteDestroyAction(outStream, networkId, dirtyState);
				break;
			}

			auto inFlightPacket = packetGenerator.GetInFlightPacket();
			
			int key = static_cast<int>(packetGenerator.GetPacketType());
			auto transmissinData = std::static_pointer_cast<ReplicationManagerTransmissionData>(inFlightPacket->GetTransmissionData(key));
			transmissinData->AddTransmission(networkId, action, writtenState);

			replicationCommand.ClearDirtyState(writtenState);
		}
	}
}

void ReplicationManager::SetStateDirty(int inNetworkId, uint8_t inDirtyState)
{
	mNetworkIdToReplicationCommand[inNetworkId].AddDirtyState(inDirtyState);
}

void ReplicationManager::HandleCreateAckd(int inNetworkId)
{
	mNetworkIdToReplicationCommand[inNetworkId].HandleCreateAckd();
}

size_t ReplicationManager::CountWriteBitSize(NetworkId_t networkId) const
{
	size_t size = 0;
	size += BitSizeCounter::Count(networkId);
	size += BitSizeCounter::Count(ReplicationAction::RA_Max);

	auto command = mNetworkIdToReplicationCommand.at(networkId);
	auto action = command.GetAction();
	auto dirtyState = command.GetDirtyState();

	auto gameObject = NetworkManagerServer::GetInstance().GetGameObject(networkId);

	switch (action)
	{
	case ReplicationAction::RA_Create:
		size += BitSizeCounter::Count(gameObject->GetClassId());
		size += gameObject->CountWriteBitSize(dirtyState);
		break;

	case ReplicationAction::RA_Update:
		size += gameObject->CountWriteBitSize(dirtyState);
		break;

	case ReplicationAction::RA_Destroy:
		break;
	}

	return size;
}

uint8_t ReplicationManager::WriteCreateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint8_t inDirtyState)
{
	GameObject* gameObject = NetworkManagerServer::GetInstance().GetGameObject(inNetworkId);
	inOutputStream.Write(gameObject->GetClassId());
	return gameObject->Write(inOutputStream, inDirtyState);
}

uint8_t ReplicationManager::WriteUpdateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint8_t inDirtyState)
{
	GameObject* gameObject = NetworkManagerServer::GetInstance().GetGameObject(inNetworkId);
	return gameObject->Write(inOutputStream, inDirtyState);
}

uint8_t ReplicationManager::WriteDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint8_t inDirtyState)
{
	// networkId와 action을 먼저 작성한 것으로 여기서는 아무 것도 하지 않는다.
	return inDirtyState;
}