#pragma once

#include "Constant.h"

struct OverlappedDto
{
	uint8_t _Buffer[Constant::MAX_PACKET_SIZE];
	DWORD _numberOfBytesTransfered;
	DWORD _overlappedFlags;
	OVERLAPPED _overlapped;
};
