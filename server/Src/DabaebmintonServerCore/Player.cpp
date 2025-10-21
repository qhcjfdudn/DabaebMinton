#include "ServerPCH.h"
#include "Player.h"

#include "PhysicsEngine.h"

Player::Player(PxVec2 location) :
	GameObject(location, PxVec2{ 0.0f, 0.0f })
{
	auto& physicsEngine = PhysicsEngine::GetInstance();

	_rigidbody = physicsEngine.CreateCapsule2D(
		location,
		0.25f /* x size의 절반 */,
		0.5f /* height의 절반 */);

	PxRigidBodyExt::updateMassAndInertia(*_rigidbody, 1.0f);
}

size_t Player::CountWriteBitSize(const uint8_t inState) const
{
	return GameObject::CountWriteBitSize(inState);
}
