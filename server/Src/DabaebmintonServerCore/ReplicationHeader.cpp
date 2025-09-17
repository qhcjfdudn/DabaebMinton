#include "ServerPCH.h"
#include "ReplicationHeader.h"

#include "OutputMemoryBitStream.h"
#include "GetRequiredBits.h"

void ReplicationHeader::Write(OutputMemoryBitStream& inStream)
{
	inStream.WriteBits(&_ra, GetRequiredBits(static_cast<int>(ReplicationAction::RA_Max)));
	inStream.Write(_nid);
	inStream.Write(_cid);
}
