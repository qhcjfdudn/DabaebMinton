using System;
using UnityEngine;

using static NetworkUtils;

public class ReplicationHeader
{
    public ReplicationAction Ra { get; private set; }
    public uint Nid { get; private set; }

    public void Read(InputMemoryBitStream inStream)
    {
        Nid = (uint)inStream.ReadInt();
        Ra = (ReplicationAction)inStream.ReadInt(GetRequiredBits(ReplicationAction.RA_Max));
    }
};
