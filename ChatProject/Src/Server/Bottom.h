#pragma once
#include "StaticGameObject.h"
class Bottom : public StaticGameObject
{
public:
	Bottom(PxVec2 location);
	~Bottom() override = default;
	unsigned int GetClassId() override;
	void Write(OutputMemoryBitStream& inStream) override;
};
