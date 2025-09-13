#pragma once

consteval unsigned int GetRequiredBits(int bits) {
	unsigned int ret = 0;

	while (bits) {
		++ret;
		bits >>= 1;
	}

	return ret;
}