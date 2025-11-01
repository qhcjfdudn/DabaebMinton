#include "ServerPCH.h"
#include "RPCManager.h"

#include "GameManager.h"
#include "Game.h"
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
	auto game = gameManager.FindGame(&clientProxy);

	game->SetClientReady(clientProxy.GetSession().GetPlayerId());
}

void UnwrapMovePlayer(const ClientProxy& clientProxy, InputMemoryBitStream& inStream)
{
	NetworkId_t networkId = 0;
	PxVec2 direction = {};
	inStream.Read(networkId);
	inStream.Read(direction.x);
	inStream.Read(direction.y);
	
	spdlog::debug("[UnwrapMovePlayer] x: {}, y: {}", direction.x, direction.y);

	GameObject* playerCharacter = NetworkManagerServer::GetInstance().GetGameObject(networkId);

	if (playerCharacter == nullptr)
	{
		spdlog::warn("[UnwrapMovePlayer] playerCharacter is nullptr. networkId: {}", networkId);
		return;
	}

	if (playerCharacter->GetClassId() != 'PLYR')
	{
		spdlog::warn("[UnwrapMovePlayer] playerCharacter is not Player class. networkId: {}", networkId);
		return;
	}

	auto game = GameManager::GetInstance().FindGame(&clientProxy);
	game->MovePlayer(static_cast<Player*>(playerCharacter), direction);
}
