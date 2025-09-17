#pragma once

class OutputMemoryBitStream;

class GameObject
{
public:
	GameObject(PxVec2 location, PxVec2 velocity);
	virtual ~GameObject();

	void SetVelocity(PxVec2 velocity);
	void SetRigidbody(PxRigidDynamic& rigidbody);

	virtual bool FixedUpdate();
	void SetCurrentTransform();

	virtual uint32_t GetAllStateMask() const { return 0; }

	virtual uint32_t GetClassId() const;
	virtual size_t CountWriteBitSize(const uint8_t inDirtyState) const;
	virtual uint8_t Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const;
	virtual void Write(OutputMemoryBitStream& inStream) const;

	PxActor* GetRigidbody() const;

protected:
	PxVec2 _location;
	PxVec2 _velocity;

	PxRigidDynamic* _rigidbody = nullptr;
};
