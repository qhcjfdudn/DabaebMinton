#pragma once

#include <PxScene.h>

class GameObject;
class StaticGameObject;

class Level
{
public:
	Level();
	~Level();
	
	PxScene* pxScene = nullptr;

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

	void FixedUpdate();

	system_clock::time_point lastFixedUpdateTime;
};

