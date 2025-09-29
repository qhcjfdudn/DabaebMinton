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
	unordered_map <uint32_t, RPCFuncType> _nameToRpcMap;
};

void UnwrapMovePlayer(const ClientProxy& clientProxy, InputMemoryBitStream& inStream);
