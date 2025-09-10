#pragma once

#include "Constant.h"

struct OverlappedDto
{
	char _Buffer[Constant::MAX_PACKET_SIZE];
	DWORD _numberOfBytesTransfered{};
	DWORD _overlappedFlags{};
	OVERLAPPED _overlapped;
};
