#include "ServerPCH.h"
#include "ReplicationHeader.h"

#include "GetRequiredBits.h"

void ReplicationHeader::Write(OutputMemoryBitStream& inStream)
{
	cout << "inStream.GetBitLength(): " << inStream.GetBitLength() << endl;
	inStream.WriteBits(&_ra, GetRequiredBits(static_cast<int>(ReplicationAction::RA_Max)));
	cout << "inStream.GetBitLength(): " << inStream.GetBitLength() << endl;
	cout << "_nid: " << _nid << endl;
	inStream.Write(_nid);
	cout << "inStream.GetBitLength(): " << inStream.GetBitLength() << endl;
	cout << "_cid: " << _cid << endl;
	inStream.Write(_cid);
	cout << "inStream.GetBitLength(): " << inStream.GetBitLength() << endl;
}
