#pragma once

#include "GameObject.h"

class OutputMemoryBitStream;

class Shuttlecock : public GameObject
{
public:
	Shuttlecock(PxVec2 location, PxVec2 velocity);
	~Shuttlecock() override = default;
	
	void setRadius(float radius);
	float getRadius();

	bool FixedUpdate() override;

	unsigned int GetClassId() override;
	void Write(OutputMemoryBitStream& inStream) override;

private:
	float _radius = 1.5;
};
