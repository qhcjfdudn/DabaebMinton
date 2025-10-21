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

    public int GetRemainingBitCount()
    {
        return _capacity - _bitHead;
    }

    public byte ReadByte(byte inCount)
    {
        int byteOffset = _bitHead >> 3;
        int bitOffset = _bitHead & 7;

        byte outData = (byte)(_streamBuffer[byteOffset] >> bitOffset);

        // 읽어야할 bit가 남았는지 확인
        byte bitsRead = (byte)(8 - bitOffset);
        if (bitsRead < inCount)
        {
            outData |= (byte)(_streamBuffer[byteOffset + 1] << bitsRead);
        }

        outData &= (byte)~(0xff << inCount);

        _bitHead += inCount;

        return outData;
    }

    public byte[] ReadBits(int inCount)
    {
        Debug.Log($"ReadBits(inCount: {inCount})");
        Debug.Log($"curHead: {_bitHead}, _capacity: {_capacity}");

        int byteCount = (inCount + 7) >> 3;
        byte[] ret = new byte[byteCount];

        for (int i = 0; i < byteCount; ++i, inCount -= 8)
        {
            int bitsToRead = Math.Min(8, inCount);
            ret[i] = ReadByte((byte)bitsToRead);
        }

        return ret;
    }

    public byte ReadByte()
    {
        return ReadByte((byte)(sizeof(byte) << 3));
    }

    public byte ReadByte(int inCount)
    {
        if (inCount > (sizeof(byte) << 3))
            throw new ArgumentOutOfRangeException("inCount", "ReadByte inCount is too large");

        return ReadBits(inCount)[0];
    }

    public int ReadInt()
    {
        return BitConverter.ToInt32(ReadBits(sizeof(int) << 3));
    }

    public int ReadInt(int inCount)
    {
        if (inCount > (sizeof(int) << 3))
            throw new ArgumentOutOfRangeException("inCount", "ReadInt inCount is too large");

        byte[] src = ReadBits(inCount);
        byte[] bytes = new byte[sizeof(int)];
        Buffer.BlockCopy(src, 0, bytes, 0, src.Length);

        return BitConverter.ToInt32(bytes);
    }

    public float ReadFloat()
    {
        return BitConverter.ToSingle(ReadBits(sizeof(float) << 3), 0);
    }

    public bool ReadBool()
    {
        return ReadBits(1)[0] == 1;
    }

    public Vector2 ReadVector2()
    {
        float x = ReadFloat();
        float y = ReadFloat();
        return new Vector2(x, y);
    }
}
