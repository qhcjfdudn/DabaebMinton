#pragma once

class ClientProxy;
class InputMemoryBitStream;

using RPCFuncType = std::function<void(const ClientProxy&, InputMemoryBitStream&)>;

class RPCManager
{
public:
	RPCManager();

	void RegisterUnwrapRpc(uint32_t name, RPCFuncType func);

	void ProcessRpc(const ClientProxy& clientProxy, InputMemoryBitStream& inStream);

private:
	unordered_map <uint32_t, RPCFuncType> _nameToRpcMap;
};

void UnwrapMovePlayer(const ClientProxy& clientProxy, InputMemoryBitStream& inStream);
