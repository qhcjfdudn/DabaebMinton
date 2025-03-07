#pragma once

template<int bits, int tBits>
struct GetRequiredBitsHelper {
	static const int value = GetRequiredBitsHelper<bits + 1, (tBits >> 1)>::value;
};

template<int bits>
struct GetRequiredBitsHelper<bits, 0> {
	static const int value = bits;
};

template<int tBits>
struct GetRequiredBits {
	static const int value = GetRequiredBitsHelper<0, tBits>::value;
};