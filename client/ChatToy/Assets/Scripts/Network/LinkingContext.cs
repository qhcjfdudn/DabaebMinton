using System.Collections.Generic;
using UnityEngine;

public class LinkingContext
{
    public static LinkingContext Instance
    {
        get
        {
            if (_instance == null)
            {
                _instance = new LinkingContext();
            }
            return _instance;
        }
    }
    private static LinkingContext _instance;

    public void AddGameObject(uint networkId, GameObject gameObject)
    {
        _networkIdToGameObjectMap.Add(networkId, gameObject);
        _gameObjectToNetworkIdMap.Add(gameObject, networkId);
    }

    public uint GetNetworkId(GameObject gameObject)
    {


        return 0;
    }
    public GameObject GetGameObject(uint networkId)
    {
        if (_networkIdToGameObjectMap.TryGetValue(networkId, out GameObject gameObject) == false)
        {
            return null;
            
        }

        return gameObject;
    }

    private Dictionary<uint, GameObject> _networkIdToGameObjectMap;
    private Dictionary<GameObject, uint> _gameObjectToNetworkIdMap;

    private LinkingContext()
    {
        _networkIdToGameObjectMap = new Dictionary<uint, GameObject>();
        _gameObjectToNetworkIdMap = new Dictionary<GameObject, uint>();
    }
}
