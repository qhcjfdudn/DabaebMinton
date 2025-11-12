#pragma once

#include "GameObject.h"

class Shuttlecock : public GameObject
{
public:
	Shuttlecock(PxVec2 position, PxVec2 velocity);

	uint32_t GetClassId() const override { return 'STCK'; }
	
	float GetRadius() const;

	bool FixedUpdate() override;

private:
	float _radius = 0.3f;
};
