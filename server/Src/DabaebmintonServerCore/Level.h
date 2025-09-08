#pragma once

#include <PxScene.h>

class GameObject;
class StaticGameObject;

class Level
{
public:
	Level();
	~Level();

	vector<shared_ptr<GameObject>> gameObjects;
	vector<shared_ptr<StaticGameObject>> staticGameObjects;

	void InitLevel();
	void ClearLevel();
	void Release();

	void RemoveAllGameObjects();
	void RemoveGameObject(size_t idx);
	void Remove(PxActor* actor);

	void RemoveAllStaticGameObjects();

	bool HasElapsedFixedUpdateInterval();
	void SetLastFixedUpdateTimeToNow();
	void FixedUpdate();
	
	PxScene* pxScene = nullptr;

	system_clock::time_point lastFixedUpdateTime;
};

