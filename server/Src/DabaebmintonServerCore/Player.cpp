#include "ServerPCH.h"
#include "Player.h"

#include "PhysicsEngine.h"
#include "OutputMemoryBitStream.h"
#include "CharacterInitialData.h"

Player::Player(ECharacterID characterId, PlayerId_t ownerId, PxVec2 position) :
	_characterId(characterId),
	_ownerId(ownerId),
	GameObject(position, PxVec2{ 0.0f, 0.0f })
{
	_rigidbody = PhysicsEngine::GetInstance().CreateDefaultPlayerCharacter(position);
}

size_t Player::CountWriteBitSize(const uint8_t inState) const
{
	return GameObject::CountWriteBitSize(inState);
}

uint8_t Player::Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const
{
	uint8_t writtenState = 0;

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Position))
	{
		inStream.Write(true);
		inStream.Write(_position);

		writtenState |= static_cast<uint8_t>(ReplicationState::RS_Position);
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

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_OwnerId))
	{
		inStream.Write(true);
		inStream.Write(_ownerId);
		writtenState |= static_cast<uint8_t>(ReplicationState::RS_OwnerId);
	}
	else
	{
		inStream.Write(false);
	}

	return writtenState;
}

bool Player::FixedUpdate()
{
	float characterMoveVelocity = 0.0f;
	float characterJumpVelocity = 0.0f;
	switch (_characterId)
	{
	case ECharacterID::Daramgee:
		characterMoveVelocity = DaramgeeInitialData::moveVelocity;
		characterJumpVelocity = DaramgeeInitialData::jumpVelocity;
		break;
	case ECharacterID::Baebsae:
		characterMoveVelocity = BaebsaeInitialData::moveVelocity;
		characterJumpVelocity = BaebsaeInitialData::jumpVelocity;
		break;
	}

	float nextVelocityX = _moveValue * characterMoveVelocity;
	float nextVelocityY = _rigidbody->getLinearVelocity().y;

	_rigidbody->setLinearVelocity(PxVec3{ nextVelocityX, nextVelocityY, 0 });

	PxVec3 curPos = _rigidbody->getGlobalPose().p;
	PxVec2 curPos2D{ curPos.x, curPos.y };

	// 가로 이동
	if ((curPos2D - _position).magnitude() > 1e-4f)
	{
		MarkDirtyState(_networkId, static_cast<uint8_t>(ReplicationState::RS_Position));

		GameObject::FixedUpdate();
	}

	// 점프 처리
	if (_isJumpPressed /* && _jumpCount == 0 */)
	{
		const float jumpCorrection = 1.07f;
		_rigidbody->setLinearVelocity(PxVec3{ nextVelocityX, 0, 0 });
		_rigidbody->addForce(PxVec3{ 0, characterJumpVelocity * 0.02f * jumpCorrection, 0 }, PxForceMode::eIMPULSE);

		_isJumpPressed = false;
		++_jumpCount;

		//MarkDirtyState(_networkId, static_cast<uint8_t>(ReplicationState::RS_Velocity));
	}

	return true;
}
