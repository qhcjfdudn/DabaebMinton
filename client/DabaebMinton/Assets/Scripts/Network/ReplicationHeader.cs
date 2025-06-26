using System;
using UnityEngine;

using static NetworkUtils;

public class ReplicationHeader
{
    public ReplicationAction Ra { get; private set; }
    public uint Nid { get; private set; }
    public uint Cid { get; private set; }

    public void Read(InputMemoryBitStream inStream)
    {
        Ra = (ReplicationAction)inStream.ReadInt(GetRequiredBits(ReplicationAction.RA_Max));
        //Nid = (uint)outStream.ReadInt();
        //Cid = (uint)outStream.ReadInt();
        Nid = (uint)inStream.ReadInt2();
        Cid = (uint)inStream.ReadInt2();
    }
};
