#pragma once

#include "ReplicationAction.h"
#include "GetRequiredBits.h"

class BitSizeCounter
{
public:
	static constexpr size_t Count(unsigned int) { return sizeof(unsigned int) << 3; }
	static constexpr size_t Count(ReplicationAction) { 
		return GetRequiredBits(static_cast<int>(ReplicationAction::RA_Max));
	}
	
	static size_t Count(PxVec2 inData);
};
