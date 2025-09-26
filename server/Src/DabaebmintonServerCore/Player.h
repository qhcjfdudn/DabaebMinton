#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(PxVec2 location);

	uint32_t GetClassId() const override { return 'PLYR'; }

	void Write(OutputMemoryBitStream& inStream) const override;
	size_t CountWriteBitSize(const uint8_t inState) const override;
};
