#pragma once

#include <PxScene.h>

#include "GameConfig.h"

class GameObject;
class StaticGameObject;

class Level
{
public:
	Level();
	~Level();

	void InitLevel(PlayerId_t (&playerIds)[GameConfig::MAX_PLAYERS]);
	void ClearLevel();
	void Release();

	vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

	void RemoveAllGameObjects();
	void RemoveGameObject(size_t idx);
	void Remove(PxActor* actor);

	void RemoveAllStaticGameObjects();

	bool HasElapsedFixedUpdateInterval();
	void SetLastFixedUpdateTimeToNow();
	void FixedUpdate();

private:
	PxScene* _pxScene = nullptr;
	
	vector<shared_ptr<GameObject>> _gameObjects;
	vector<shared_ptr<StaticGameObject>> _staticGameObjects;
	
	system_clock::time_point _lastFixedUpdateTime;
};
