#pragma once

struct OverlappedDto
{
	static const int MAX_BUFFER_LENGTH = 8'192;

	char _Buffer[MAX_BUFFER_LENGTH + 1];
	DWORD _numberOfBytesTransfered{};
	DWORD _overlappedFlags{};
	OVERLAPPED _overlapped;
};
