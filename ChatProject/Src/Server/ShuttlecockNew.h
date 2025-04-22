#pragma once

#include "GameObjectNew.h"

class ShuttlecockNew : public GameObjectNew
{
public:
	ShuttlecockNew(PxVec2 location, PxVec2 velocity);

	float GetRadius();

	bool FixedUpdate() override;

	unsigned int GetClassId() override;
	void Write(OutputMemoryBitStream& inStream) override;

private:
	float _radius = 1.5;
};

