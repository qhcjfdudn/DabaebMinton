#include "ServerPCH.h"
#include "GameObject.h"

#include "Constant.h"
#include "OutputMemoryBitStream.h"
#include "Engine.h"

ReplicationFlag operator| (ReplicationFlag lhs, ReplicationFlag rhs) {
	return static_cast<ReplicationFlag>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

ReplicationFlag& operator|= (ReplicationFlag& lhs, ReplicationFlag rhs) {
	lhs = lhs | rhs;
	return lhs;
}

ReplicationFlag operator&(ReplicationFlag lhs, ReplicationFlag rhs)
{
	return static_cast<ReplicationFlag>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

bool hasFlag(ReplicationFlag flags, ReplicationFlag flag)
{
	return static_cast<bool>(flags & flag);
}

GameObject::GameObject(PxVec2 location, PxVec2 velocity) :
	_location(location), _velocity(velocity), _rigidbody(nullptr) {}

GameObject::~GameObject()
{
	Engine::GetInstance().RemoveActor(_rigidbody);
}

void GameObject::SetDirtyFlag(ReplicationFlag flag)
{
	replicationFlag |= flag;
}

void GameObject::SetVelocity(PxVec2 velocity)
{
	_velocity = velocity;
}

bool GameObject::FixedUpdate()
{
	SetCurrentTransform();

	return true;
}

void GameObject::SetCurrentTransform()
{
	// simulate 중에는 getGlobalPose()를 사용할 수 없다는 에러 메시지 발견
	// 물리 엔진에 접근해 값을 알아오고자 할 때는 lockRead()를 걸어야 한다.
	
	auto& engineInstance = Engine::GetInstance();
	
	engineInstance.LockRead();
	PxVec3 curLocation{ _rigidbody->getGlobalPose().p };
	PxVec3 curVelocity{ _rigidbody->getLinearVelocity() };
	engineInstance.UnlockRead();
	
	_location = PxVec2{ curLocation.x, curLocation.y };
	_velocity = PxVec2{ curVelocity.x, curVelocity.y };
}

unsigned int GameObject::GetClassId()
{
	return 'GMOJ';
}

void GameObject::Write(OutputMemoryBitStream& inStream)
{
	inStream.Write(_location);
	inStream.Write(_velocity);
}
