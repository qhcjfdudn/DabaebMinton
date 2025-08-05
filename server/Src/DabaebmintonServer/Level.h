#pragma once

#include <PxScene.h>
#include "ReplicationManager.h"

class GameObject;
class StaticGameObject;

class Level
{
public:
	Level();
	~Level();
	
	PxScene* pxScene = nullptr;
	ReplicationManager replicationManager;

	vector<shared_ptr<GameObject>> gameObjects;
	vector<shared_ptr<StaticGameObject>> staticGameObjects;

	void InitLevel();
	void ClearLevel();
	void Release();

	void StepPhysics();

	void RemoveAllGameObjects();
	void RemoveGameObject(size_t idx);
	void Remove(PxActor* actor);

	void RemoveAllStaticGameObjects();

	void Update();
	void FixedUpdate();
	void WriteWorldStateToStream();

	system_clock::time_point lastFixedUpdateTime;
	system_clock::time_point lastPhysxFixedUpdateTime;
	system_clock::time_point lastPacketUpdateTime;

private:
	int WriteByReplication(OutputMemoryBitStream& outStream);

	queue<shared_ptr<GameObject>> _pendingSerializationQueue;
};

