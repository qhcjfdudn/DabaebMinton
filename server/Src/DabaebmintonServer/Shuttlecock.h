#pragma once

#include "GameObject.h"

class Shuttlecock : public GameObject
{
public:
	Shuttlecock(PxVec2 location, PxVec2 velocity);

	float GetRadius() const;

	bool FixedUpdate() override;

	unsigned int GetClassId() override;
	void Write(OutputMemoryBitStream& inStream) override;

private:
	float _radius = 1.5;
};

