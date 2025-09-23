#pragma once

template<typename T>
concept Primitive = std::integral<T>
|| std::floating_point<T>;

class InputMemoryBitStream
{
public:
	InputMemoryBitStream(const unsigned char* src, uint32_t bitCount);
	~InputMemoryBitStream();

	bool isEndOfStream() const;
	size_t GetBitLength() const;
	size_t GetBitsLeft() const;

	void ReadBits(uint8_t& outData, uint32_t inBitCount);
	void ReadBits(void* outData, uint32_t inBitCount);

	template<typename T>
	T ReadBits(uint32_t inBitCount)
	{
		T data;
		ReadBits(&data, inBitCount);
		return data;
	}

	template<Primitive T>
	void Read(T& outData)
	{
		ReadBits(&outData, sizeof(outData) << 3);
	}

	template<>
	void Read(bool& outData) { ReadBits(&outData, 1); }

private:
	unsigned char* _buffer;
	uint32_t _bitHead;
	uint32_t _bitCapacity;
};
