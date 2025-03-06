#pragma once

#include "OutputMemoryBitStream.h"

class ReplicationHeader
{
public:
	enum class ReplicationAction {
		RA_Create, RA_Update, RA_Delete, Size
	};

	ReplicationHeader(ReplicationAction ra, NetworkId_t nid = 0, unsigned int cid = 0) :
		_ra(ra), _nid(nid), _cid(cid) {}

	void Write(OutputMemoryBitStream& inStream);

private:
	ReplicationAction _ra;
	NetworkId_t _nid;
	unsigned int _cid;

};

