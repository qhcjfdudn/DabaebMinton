#pragma once

#include "GameObject.h"

enum class ECharacterID
{
	None, Daramgee, Baebsae, Max
};

class Player : public GameObject
{
public:
	Player(ECharacterID characterId, PlayerId_t ownerId, PxVec2 position);

	uint32_t GetClassId() const override { return 'PLYR'; }

	PlayerId_t GetOwnerId() const { return _ownerId; }

	size_t CountWriteBitSize(const uint8_t inState) const override;

	uint8_t Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const override;

	enum class ReplicationState : uint8_t
	{
		RS_Position = 1 << 0,
		RS_Velocity = 1 << 1,
		RS_CharacterId = 1 << 2,
		RS_OwnerId = 1 << 3,

		RS_All = RS_Position | RS_Velocity | RS_CharacterId | RS_OwnerId
	};

	uint8_t GetAllStateMask() const override { return static_cast<uint8_t>(ReplicationState::RS_All); }

	bool FixedUpdate() override;

	float _moveValue;
	bool _isJumpPressed{ false };
	int _jumpCount{ 0 };

private:
	ECharacterID _characterId;
	PlayerId_t _ownerId;
};
