#pragma once
#include <iostream>
#include <string>

class OutputMemoryBitStream
{
public:
	OutputMemoryBitStream() :
		_buffer(nullptr), _bitHead(0), _bitCapacity(0)
	{
		ReallocBuffer(32);		// 32bytes
	}
	~OutputMemoryBitStream()
	{
		if (_buffer == nullptr)
			return;

		free(_buffer);
	}

	void Clear();

	void WriteBits(uint8_t inData, size_t inBitCount);
	void WriteBits(const void* inData, size_t inBitCount);

	const unsigned char* GetBufferPtr() const { return _buffer; }
	uint32_t GetBitLength() const { return _bitHead; }
	uint32_t GetByteLength() const { return (_bitHead + 7) >> 3; }

	void WriteBytes(const void* inData, size_t inByteCount) {
		WriteBits(inData, inByteCount << 3);
	}

	void Write(uint32_t inData);
	void Write(uint16_t inData);
	void Write(uint8_t inData);
	void Write(float inData);
	void Write(std::string inData);
	void Write(PxVec2 inData);

private:
	void ReallocBuffer(uint32_t newBitCapacity);

	unsigned char* _buffer;
	uint32_t _bitHead;
	uint32_t _bitCapacity;
};
