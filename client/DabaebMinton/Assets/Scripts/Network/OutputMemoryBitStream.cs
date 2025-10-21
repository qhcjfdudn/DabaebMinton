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

    public void Clear()
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

    public void WriteBits(byte inData, int inCount)
    {
        int nextBitHead = _bitHead + inCount;
        if (nextBitHead >= _capacity)
        {
            ReAllocate(nextBitHead);
        }

        int byteOffset = _bitHead >> 3;
        int bitOffset = _bitHead & 7;
        int bitsFreeThisByte = 8 - bitOffset;

        // 현재 처리 중 바이트에 inData 중 쓸 수 있는 만큼 우선 쓰기
        int currentMask = ~(0xff << bitOffset);
        _streamBuffer[byteOffset] = (byte)((_streamBuffer[byteOffset] & currentMask) | (inData << bitOffset));

        // inData의 쓰지 못한 상위 부분이 있다면 쓰기
        if (inCount > bitsFreeThisByte)
        {
            _streamBuffer[byteOffset + 1] = (byte)(inData >> bitsFreeThisByte);
        }

        _bitHead = nextBitHead;
    }

    public void WriteBits(byte[] inData, int inCount)
    {
        for (int idx = 0; inCount > 0; ++idx, inCount -= 8)
        {
            WriteBits(inData[idx], Math.Min(8, inCount));
        }
    }

    public void WriteBits(int inData, int inCount)
    {
        WriteBits(BitConverter.GetBytes(inData), inCount);
    }

    public void Write(int inData)
    {
        WriteBits(BitConverter.GetBytes(inData), 32);
    }

    public void Write(string inData)
    {
        byte[] encodedInData = Encoding.UTF8.GetBytes(inData);

        WriteBits(encodedInData, encodedInData.Length * 8);
    }

    public void Write(bool inData)
    {
        WriteBits(inData ? 1 : 0, 1);
    }

    public void Write(uint inData)
    {
        WriteBits(BitConverter.GetBytes(inData), 32);
    }
}
