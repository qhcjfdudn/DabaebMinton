#pragma once

class OutputMemoryBitStream;

class GameObject
{
public:
	GameObject(PxVec2 location, PxVec2 velocity);
	virtual ~GameObject();

	virtual uint32_t GetClassId() const { return 'GMOJ'; }
	
	void SetVelocity(PxVec2 velocity);
	void SetRigidbody(PxRigidDynamic& rigidbody);

	NetworkId_t GetNetworkId() const { return _networkId; }
	void SetNetworkId(NetworkId_t networkId) { _networkId = networkId; }

	virtual bool FixedUpdate();
	void SetCurrentTransform();

	enum class ReplicationState : uint8_t
	{
		RS_Location = 1 << 0,
		RS_Velocity = 1 << 1,

		RS_All = RS_Location | RS_Velocity
	};

	virtual uint8_t GetAllStateMask() const { return static_cast<uint8_t>(ReplicationState::RS_All); }

	virtual void Write(OutputMemoryBitStream& inStream) const;
	
	virtual uint8_t Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const;
	virtual size_t CountWriteBitSize(const uint8_t inDirtyState) const;

	PxActor* GetRigidbody() const;

protected:
	PxVec2 _location;
	PxVec2 _velocity;

	PxRigidDynamic* _rigidbody = nullptr;
	
	NetworkId_t _networkId;
};
