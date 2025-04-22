#pragma once

#include "StaticGameObject.h"

class BadmintonBottom : public StaticGameObject
{
public:
	BadmintonBottom(PxVec2 location);
	unsigned int GetClassId() override;
	void Write(OutputMemoryBitStream& inStream) override;
};
