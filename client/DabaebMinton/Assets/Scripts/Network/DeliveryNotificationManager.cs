using System.Collections.Generic;

using PacketSequenceNumber = System.UInt16;

public class DeliveryNotificationManager
{
    private LinkedList<AckRange> _pendingAcks = new LinkedList<AckRange>();
    
    public bool hasAcks()
    {
        return _pendingAcks.Count > 0;
    }

    public void AddPendingAck(PacketSequenceNumber inSequenceNumber)
    {
        if (_pendingAcks.Count == 0 || _pendingAcks.Last.Value.ExtendIfShould(inSequenceNumber) == false)
        {
            _pendingAcks.AddLast(new AckRange(inSequenceNumber));
        }
    }

    public void WriteAckData(OutputMemoryBitStream outStream)
    {
        bool hasAcks = _pendingAcks.Count > 0;

        outStream.Write(hasAcks);
        if (hasAcks)
        {
            _pendingAcks.First.Value.Write(outStream);
            _pendingAcks.RemoveFirst();
        }
    }
}
