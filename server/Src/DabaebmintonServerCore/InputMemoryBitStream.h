#pragma once
class InputMemoryBitStream
{
public:
	InputMemoryBitStream(const unsigned char* src, uint32_t bitCount);
	~InputMemoryBitStream();

	bool isEndOfStream() const;
	size_t BitsLeft() const;

	void ReadBits(uint8_t& outData, uint32_t inBitCount);
	void ReadBits(void* outData, uint32_t inBitCount);

private:
	unsigned char* _buffer;
	uint32_t _bitHead;
	uint32_t _bitCapacity;
};

