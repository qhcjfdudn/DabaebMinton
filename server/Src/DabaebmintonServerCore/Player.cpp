#include "ServerPCH.h"
#include "Player.h"

#include "PhysicsEngine.h"
#include "OutputMemoryBitStream.h"

Player::Player(ECharacterID characterId, PxVec2 position) :
	GameObject(position, PxVec2{ 0.0f, 0.0f }),
	_characterId(characterId)
{
	auto& physicsEngine = PhysicsEngine::GetInstance();

	_rigidbody = physicsEngine.CreateCapsule2D(
		position,
		0.25f /* x size의 절반 */,
		0.5f /* height의 절반 */);

	PxRigidBodyExt::updateMassAndInertia(*_rigidbody, 1.0f);
}

size_t Player::CountWriteBitSize(const uint8_t inState) const
{
	return GameObject::CountWriteBitSize(inState);
}

uint8_t Player::Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const
{
	uint8_t writtenState = 0;

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Location))
	{
		inStream.Write(true);
		inStream.Write(_location);

		writtenState |= static_cast<uint8_t>(ReplicationState::RS_Location);
	}
	else
	{
		inStream.Write(false);
	}

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Velocity))
	{
		inStream.Write(true);
		inStream.Write(_velocity);

		writtenState |= static_cast<uint8_t>(ReplicationState::RS_Velocity);
	}
	else
	{
		inStream.Write(false);
	}

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_CharacterId))
	{
		inStream.Write(true);
		inStream.WriteBits(static_cast<int>(_characterId), static_cast<int>(ECharacterID::Max));

		writtenState |= static_cast<uint8_t>(ReplicationState::RS_CharacterId);
	}
	else
	{
		inStream.Write(false);
	}

	return writtenState;
}
