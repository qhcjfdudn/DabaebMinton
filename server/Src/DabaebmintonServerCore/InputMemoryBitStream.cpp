#include "ServerPCH.h"
#include "InputMemoryBitStream.h"

InputMemoryBitStream::InputMemoryBitStream(const unsigned char* src, uint32_t bitCount) :
	_bitHead{ 0 }, _bitCapacity{ bitCount }, _buffer{ nullptr }
{
	if (bitCount <= 0)
	{
		spdlog::warn("[InputMemoryBitStream::InputMemoryBitStream] Can not allocate 0 byte.");
		return;
	}

	size_t byteCapacity = (bitCount + 7) >> 3;
	_buffer = new unsigned char[byteCapacity];
	memcpy(_buffer, src, byteCapacity);
}

InputMemoryBitStream::~InputMemoryBitStream()
{
	if (_buffer == nullptr)
		return;

	delete[] _buffer;
}

bool InputMemoryBitStream::isEndOfStream() const
{
    return _bitHead >= _bitCapacity;
}

size_t InputMemoryBitStream::GetBitLength() const { return _bitCapacity; }

size_t InputMemoryBitStream::GetBitsLeft() const
{
	return _bitCapacity - _bitHead;
}

void InputMemoryBitStream::ReadBits(uint8_t& outData, uint32_t inBitCount)
{
	uint32_t byteOffset = _bitHead >> 3;
	uint32_t bitOffset = _bitHead & 7;

	outData = _buffer[byteOffset] >> bitOffset;
	
	// 읽어야할 bit가 남았는지 확인
	uint32_t bitsRead = 8 - bitOffset;
	if (bitsRead < inBitCount)
	{
		outData |= _buffer[byteOffset + 1] << bitsRead;
	}

	outData &= ~(0xff << inBitCount);

	_bitHead += inBitCount;
}

void InputMemoryBitStream::ReadBits(void* outData, uint32_t inBitCount)
{
	auto src = reinterpret_cast<uint8_t*>(outData);

	while (inBitCount > 8)
	{
		ReadBits(*src, 8);
		
		++src;
		inBitCount -= 8;
	}

	if (inBitCount > 0)
	{
		ReadBits(*src, inBitCount);
	}
}
