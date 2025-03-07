#pragma once

#include "GameObject.h"

class Shuttlecock : public GameObject
{
public:
	Shuttlecock(Vector2 position, Vector2 velocity);
	~Shuttlecock() override = default;
	
	void setRadius(int radius);
	int getRadius();

	bool FixedUpdate() override;

private:
	int _radius = 10;

};
