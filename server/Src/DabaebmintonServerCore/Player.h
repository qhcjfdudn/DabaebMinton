#pragma once

#include "GameObject.h"

enum class ECharacterID
{
	None, Daramgee, Baebsae, Max
};

class Player : public GameObject
{
public:
	Player(ECharacterID characterId, PxVec2 position);

	uint32_t GetClassId() const override { return 'PLYR'; }

	size_t CountWriteBitSize(const uint8_t inState) const override;

	uint8_t Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const override;

	enum class ReplicationState : uint8_t
	{
		RS_Location = 1 << 0,
		RS_Velocity = 1 << 1,
		RS_CharacterId = 1 << 2,

		RS_All = RS_Location | RS_Velocity | RS_CharacterId
	};

	uint8_t GetAllStateMask() const override { return static_cast<uint8_t>(ReplicationState::RS_All); }

private:
	ECharacterID _characterId;
};
