using System;
using Unity.VisualScripting;
using UnityEngine;
using PacketSequenceNumber = System.UInt16;

public class AckRange
{
    public AckRange(PacketSequenceNumber inStart)
    {
        _start = inStart;
        _count = 1;
    }

    public bool ExtendIfShould(PacketSequenceNumber inSequenceNumber)
    {
        if (inSequenceNumber == _start + _count)
        {
            ++_count;
            return true;
        }

        return false;
    }

    public void Write(OutputMemoryBitStream outStream)
    {
        outStream.Write(_start);
        bool hasCount = _count > 1;
        outStream.Write(hasCount);
        if (hasCount)
        {
            outStream.Write(_count);
        }
    }

    private PacketSequenceNumber _start, _count;
}
