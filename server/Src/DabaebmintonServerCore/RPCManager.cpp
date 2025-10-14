#include "ServerPCH.h"
#include "RPCManager.h"

#include "GameManager.h"
#include "Game.h"
#include "GameController.h"
#include "NetworkManagerServer.h"
#include "ClientProxy.h"
#include "InputMemoryBitStream.h"

RPCManager::RPCManager()
{
	// Unwrap rpc 나열
	RegisterUnwrapRpc('CLRD', UnwrapSetClientReady);
	RegisterUnwrapRpc('MVPL', UnwrapMovePlayer);
}

void RPCManager::RegisterUnwrapRpc(RPCNameType name, RPCFuncType func)
{
	if (_nameToRpcMap.find(name) != _nameToRpcMap.end())
	{
		spdlog::warn("{} rpc already exists.", name);
		return;
	}

	_nameToRpcMap.emplace(name, func);
}

void RPCManager::ProcessRpc(const ClientProxy& clientProxy, InputMemoryBitStream& inStream)
{
	RPCNameType name = 0;
	inStream.Read(name);

	if (_nameToRpcMap.find(name) == _nameToRpcMap.end())
	{
		spdlog::warn("[RPCManager::ProcessRpc] {} RPCName is not found.", name);
		return;
	}

	_nameToRpcMap[name](clientProxy, inStream);
}

void UnwrapSetClientReady(const ClientProxy& clientProxy, InputMemoryBitStream& inStream)
{
	auto& gameManager = GameManager::GetInstance();
	auto& gameController = gameManager.FindGame(&clientProxy)->GetGameController();

	gameController.SetClientReady(clientProxy.GetSession().GetPlayerId());
}

void UnwrapMovePlayer(const ClientProxy& clientProxy, InputMemoryBitStream& inStream)
{
	auto& gameManager = GameManager::GetInstance();
	auto& gameController = gameManager.FindGame(&clientProxy)->GetGameController();
	
	NetworkId_t networkId = 0;
	inStream.Read(networkId);

	auto& networkManager = NetworkManagerServer::GetInstance();
	GameObject* playerCharacter = networkManager.GetGameObject(networkId);

	// 이동 데이터를 정의한다.
	// 이동 데이터를 읽는다.

	gameController.MovePlayer(playerCharacter /* , 이동 데이터 전달 */);
}
