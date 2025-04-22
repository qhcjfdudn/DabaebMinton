#pragma once

#include <PxScene.h>
#include "ReplicationManagerNew.h"

class GameObjectNew;

class Level
{
public:
	Level();
	~Level();
	
	PxScene* pxScene = nullptr;
	ReplicationManagerNew replicationManager;

	vector<shared_ptr<GameObjectNew>> gameObjects;

	void InitLevel();
	void ClearLevel();
	void Release();

	void StepPhysics();

	void RemoveAllGameObjects();
	void RemoveGameObject(size_t idx);
	void Remove(PxActor* actor);

	void Update();
	void FixedUpdate();
	void WriteWorldStateToStream();

	system_clock::time_point lastFixedUpdateTime;
	system_clock::time_point lastPhysxFixedUpdateTime;
	system_clock::time_point lastPacketUpdateTime;

private:
	int WriteByReplication(OutputMemoryBitStream& outStream);

	queue<shared_ptr<GameObjectNew>> _pendingSerializationQueue;
};

