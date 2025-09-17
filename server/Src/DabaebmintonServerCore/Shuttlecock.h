#pragma once

#include "GameObject.h"

class Shuttlecock : public GameObject
{
public:
	Shuttlecock(PxVec2 location, PxVec2 velocity);

	uint32_t GetClassId() const override { return 'STCK'; }
	
	float GetRadius() const;

	bool FixedUpdate() override;

	void Write(OutputMemoryBitStream& inStream) const override;
	size_t CountWriteBitSize(const uint8_t inState) const override;

private:
	float _radius = 1.5;
};
