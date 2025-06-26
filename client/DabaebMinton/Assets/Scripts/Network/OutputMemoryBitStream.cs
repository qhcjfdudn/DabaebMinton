using System;
using System.Text;
using UnityEngine;

public class OutputMemoryBitStream
{
    //private const int INITIAL_BUFFER_SIZE = 1_024;
    private const int INITIAL_BUFFER_SIZE = 4;  // 검증용

    private byte[] _streamBuffer = new byte[INITIAL_BUFFER_SIZE];
    public byte[] StreamBuffer { get { return _streamBuffer; } }

    private int _capacity = INITIAL_BUFFER_SIZE * 8;
    private int _bitHead = 0;
    public int Count { get { return _bitHead; } }

    public void InitBuffer()
    {
        _bitHead = 0;
    }

    private void ReAllocate(int newCapacity)
    {
        // 왠지.. 2의 제곱수 만큼 Capacity를 관리하고 싶다..
        _capacity = GetNextCapacity(newCapacity);

        byte[] newBuffer = new byte[_capacity];
        Buffer.BlockCopy(_streamBuffer, 0, newBuffer, 0, _streamBuffer.Length);
        _streamBuffer = newBuffer;

        Debug.Log($"ReAllocate {_capacity} capacites.");
    }

    private int GetNextCapacity(int capacity)
    {
        while (capacity > 0)
        {
            int lsb = (capacity & ~(capacity - 1));
            int nextBit = capacity - lsb;

            if (nextBit == 0)
                break;

            capacity = nextBit;
        }
        capacity <<= 1;

        return capacity;
    }

    public void WriteBits(byte[] inData, int inCount)
    {
        Debug.Log($"inData: {BitConverter.ToString(inData)}, inCount: {inCount}");
        Debug.Log($"curHead: {_bitHead}, _capacity: {_capacity}");
        
        int nextBitHead = _bitHead + inCount;
        if (nextBitHead >= _capacity)
        {
            ReAllocate(nextBitHead);
        }

        int byteOffset = _bitHead >> 3;
        int bitOffset = _bitHead & 7;
        int currentMask = ~(0xff << bitOffset);

        int inDataIdx = 0;

        while (inCount > 8)
        {
            _streamBuffer[byteOffset] = (byte)((_streamBuffer[byteOffset] & currentMask) | (inData[inDataIdx] << bitOffset));

            int writtenBits = 8 - bitOffset;
            _streamBuffer[byteOffset + 1] = (byte)(inData[inDataIdx] >> writtenBits);

            ++inDataIdx; ++byteOffset;
            inCount -= 8;
        }

        if (bitOffset + inCount > 8)
        {
            _streamBuffer[byteOffset] = (byte)((_streamBuffer[byteOffset] & currentMask) | (inData[inDataIdx] << bitOffset));

            int writtenBits = 8 - bitOffset;
            inCount -= writtenBits;
            byte mask = (byte)(~(0xff << inCount) << writtenBits);
            _streamBuffer[byteOffset + 1] = (byte)((inData[inDataIdx] & mask) >> writtenBits);
        }
        else
        {
            byte inDataMask = (byte)~(0xff << inCount);
            _streamBuffer[byteOffset] = (byte)((_streamBuffer[byteOffset] & currentMask) | ((inData[inDataIdx] & inDataMask) << bitOffset));
        }

        _bitHead = nextBitHead;
    }

    public void Write(int inData)
    {
        WriteBits(BitConverter.GetBytes(inData), 32);
    }

    public void WriteBits(int inData, int inCount)
    {
        WriteBits(BitConverter.GetBytes(inData), inCount);
    }

    public void Write(string inData)
    {
        byte[] encodedInData = Encoding.UTF8.GetBytes(inData);

        WriteBits(encodedInData, encodedInData.Length * 8);
    }
}
