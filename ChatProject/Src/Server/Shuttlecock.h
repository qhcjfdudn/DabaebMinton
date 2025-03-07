#pragma once

#include "GameObject.h"

class OutputMemoryBitStream;

class Shuttlecock : public GameObject
{
public:
	Shuttlecock(Vector2 position, Vector2 velocity);
	~Shuttlecock() override = default;
	
	void setRadius(int radius);
	int getRadius();

	bool FixedUpdate() override;

	unsigned int GetClassId() override;
	void Write(OutputMemoryBitStream& inStream) override;

private:
	int _radius = 10;

};
