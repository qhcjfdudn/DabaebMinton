#pragma once

constexpr int GetRequiredBits(int bits)
{
	int ret = 0;

	while (bits)
	{
		++ret;
		bits >>= 1;
	}

	return ret;
}