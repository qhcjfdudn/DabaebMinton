using System.Text;
using System;
using UnityEngine;

public class InputMemoryBitStream
{
    private byte[] _streamBuffer;
    public byte[] StreamBuffer { get { return _streamBuffer; } }

    private int _capacity;
    private int _bitHead;
    public int Count { get { return _bitHead; } }

    public InputMemoryBitStream(byte[] streamBuffer, int bytesRead)
    {
        _streamBuffer = new byte[bytesRead];
        Buffer.BlockCopy(streamBuffer, 0, _streamBuffer, 0, bytesRead);

        _capacity = bytesRead << 3;
        _bitHead = 0;
    }

    public byte[] ReadBits(int inCount)
    {
        Debug.Log($"ReadBits(inCount: {inCount})");
        Debug.Log($"curHead: {_bitHead}, _capacity: {_capacity}");

        if (inCount <= 0)
            return null;

        int nextBitHead = _bitHead + inCount;

        int byteOffset = _bitHead >> 3;
        int bitOffset = _bitHead & 7;

        int byteSize = (inCount - 1) / 8 + 1;
        byte[] ret = new byte[byteSize];
        int destOffset = 0;

        while (inCount > 8)
        {
            ret[destOffset] |= (byte)(_streamBuffer[byteOffset] >> bitOffset);

            int readBits = 8 - bitOffset;
            byte mask = (byte)(0xff << bitOffset);
            ret[destOffset] |= (byte)((_streamBuffer[byteOffset + 1] & ~mask) << readBits);

            ++destOffset; ++byteOffset;
            inCount -= 8;
        }

        if (bitOffset + inCount > 8)
        {
            ret[destOffset] |= (byte)(_streamBuffer[byteOffset] >> bitOffset);

            int readBits = 8 - bitOffset;
            inCount -= readBits;
            byte mask = (byte)~(0xff << inCount);
            ret[destOffset] |= (byte)((_streamBuffer[byteOffset + 1] & mask) << readBits);
        }
        else
        {
            byte mask = (byte)(~(0xff << inCount) << bitOffset);
            ret[destOffset] = (byte)((_streamBuffer[byteOffset] & mask) >> bitOffset);
        }

        _bitHead = nextBitHead;

        return ret;
    }

    public byte ReadByte(int inCount)
    {
        if (inCount > (sizeof(byte) << 3))
            throw new ArgumentOutOfRangeException("inCount", "ReadByte inCount is too large");

        return ReadBits(inCount)[0];
    }

    public int ReadInt()
    {
        int ret = 0;

        byte[] bytes = ReadBits(sizeof(int) << 3);
        for (int i = 0; i < bytes.Length; ++i)
        {
            ret |= bytes[i] << (i * 8);
        }

        return ret;
    }

    public int ReadInt2()
    {
        byte[] bytes = ReadBits(sizeof(int) << 3);

        if (bytes.Length < 4)
        {
            throw new ArgumentOutOfRangeException("bytes", "ReadInt2 bytes is too small");
        }

        return BitConverter.ToInt32(bytes, 0);
    }

    public int ReadInt(int inCount)
    {
        if (inCount > (sizeof(int) << 3))
            throw new ArgumentOutOfRangeException("inCount", "ReadInt inCount is too large");

        int ret = 0;

        byte[] bytes = ReadBits(inCount);
        for (int i = 0; i < bytes.Length; ++i)
        {
            ret |= bytes[i] << (i * 8);
        }

        return ret;
    }

    public float ReadFloat()
    {
        byte[] bytes = ReadBits(sizeof(float) << 3);

        if (bytes.Length < 4)
        {
            throw new ArgumentOutOfRangeException("bytes", "ReadFloat bytes is too small");
        }

        return BitConverter.ToSingle(bytes, 0);
    }
}
