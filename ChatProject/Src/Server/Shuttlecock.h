#pragma once

#include "GameObject.h"

class Shuttlecock : public GameObject
{
public:
	Shuttlecock(Vector2 position);
	~Shuttlecock() override = default;
	
	void setRadius(int radius);
	int getRadius();

	void FixedUpdate() override;

private:
	int _radius;

};

