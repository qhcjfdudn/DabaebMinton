using UnityEngine;

public class RPCManager
{
    private OutputMemoryBitStream _outStream;

    public RPCManager()
    {
        _outStream = new OutputMemoryBitStream();
    }

    public void Clear()
    {
        _outStream.Clear();
    }

    public bool hasRpcToSend()
    {
        return _outStream.Count > 0;
    }

    public OutputMemoryBitStream GetStream()
    {
        return _outStream;
    }

    
}
