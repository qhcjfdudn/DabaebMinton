#pragma once

#include "ReplicationAction.h"
#include "ReplicationManagerTransmissionData.h"

class InputMemoryBitStream;
class OutputMemoryBitStream;

class ReplicationCommand
{
public:

	ReplicationCommand() = default;
	ReplicationCommand(uint8_t inInitialDirtyState);

	void HandleCreateAckd();
	void AddDirtyState(uint8_t inState);
	void SetDestroy();

	bool HasDirtyState() const;

	ReplicationAction	GetAction()	const;
	uint8_t GetDirtyState() const;
	void				ClearDirtyState(uint8_t inStateToClear);

	void Write(OutputMemoryBitStream& inStream, int inNetworkId, ReplicationManagerTransmissionData* ioTransactionData);
	void Read(InputMemoryBitStream& inStream, int inNetworkId);

private:

	uint8_t			_dirtyState;
	ReplicationAction	_action;
};

