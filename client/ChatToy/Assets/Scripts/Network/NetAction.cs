using System;
using UnityEngine;

public class NetAction : MonoBehaviour
{
    public Action<InputMemoryBitStream> _readAction;

    public void Read(InputMemoryBitStream inStream)
    {
        _readAction.Invoke(inStream);
    }
}
