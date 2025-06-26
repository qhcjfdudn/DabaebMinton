using UnityEngine;

public class NetworkIdentity : MonoBehaviour
{
    public bool IsOwner { get; set; }

    private static ushort _networkObjectId;
    public static ushort NetworkObjectId {
        get
        {
            return ++_networkObjectId;
        }
    }
}


// CreateNetworkObjectRPC