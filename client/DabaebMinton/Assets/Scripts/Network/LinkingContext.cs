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
    }

    public GameObject GetGameObject(uint networkId)
    {
        if (_networkIdToGameObjectMap.TryGetValue(networkId, out GameObject go) == false)
        {
            return null;
            
        }

        return go;
    }

    public void RemoveGameObject(uint networkId)
    {
        _networkIdToGameObjectMap.Remove(networkId);
    }

    private Dictionary<uint, GameObject> _networkIdToGameObjectMap;

    private LinkingContext()
    {
        _networkIdToGameObjectMap = new Dictionary<uint, GameObject>();
    }
}
