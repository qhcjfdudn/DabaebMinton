#include "ServerPCH.h"
#include "BitSizeCounter.h"

size_t BitSizeCounter::Count(PxVec2 inData)
{
	return (sizeof(inData.x) + sizeof(inData.y)) << 3;
}
