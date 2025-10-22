using System;
using UnityEngine;

public class NetworkComponent : MonoBehaviour
{
    public UInt32 NetworkId { get; set; } = 0;
    
    public Action<InputMemoryBitStream> _readAction;

    public void Read(InputMemoryBitStream inStream)
    {
        _readAction.Invoke(inStream);
    }
}
