#pragma once

#include "StaticGameObject.h"

class BadmintonNet : public StaticGameObject
{
public:
	BadmintonNet(PxVec2 location);
	unsigned int GetClassId() override;
	void Write(OutputMemoryBitStream& inStream) override;

private:
	float _halfX = 0.04f;
	float _halfY = 0.77f;
};
