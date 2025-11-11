#pragma once

class ClientProxy;
class InputMemoryBitStream;

using RPCNameType = uint32_t;
using RPCFuncType = std::function<void(const ClientProxy&, InputMemoryBitStream&)>;

class RPCManager
{
public:
	RPCManager();

	void RegisterUnwrapRpc(RPCNameType name, RPCFuncType func);

	void ProcessRpc(const ClientProxy& clientProxy, InputMemoryBitStream& inStream);

private:
	unordered_map <RPCNameType, RPCFuncType> _nameToRpcMap;
};

// Game State 변경 관련 RPC 함수
void UnwrapSetClientReady(const ClientProxy& clientProxy, InputMemoryBitStream& inStream);

// 캐릭터 조작 관련 RPC 함수
void UnwrapMovePlayer(const ClientProxy& clientProxy, InputMemoryBitStream& inStream);
void UnwrapJumpPlayer(const ClientProxy& clientProxy, InputMemoryBitStream& inStream);