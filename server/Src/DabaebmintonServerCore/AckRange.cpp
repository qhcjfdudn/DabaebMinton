#include "ServerPCH.h"
#include "AckRange.h"

#include "InputMemoryBitStream.h"

void AckRange::Read(InputMemoryBitStream& inStream)
{
	inStream.Read(_start);
	bool hasCount{};
	inStream.Read(hasCount);

	if (hasCount)	// hasCount == false: count 1개, 
					// hasCount == true: count 2개 이상
	{
		inStream.Read(_count);
	}
	else
	{
		_count = 1;	// default.
	}
}
