using System;
using UnityEngine;

public enum ReplicationAction
{
    RA_Create,
    RA_Update,
    RA_Delete,
    RA_Max
};

public class ReplicationManager : MonoBehaviour
{
    public static ReplicationManager Instance
    {
        get
        {
            if (_instance == null)
            {
                Debug.Log("ReplicationManager GameObject가 아직 생성되지 않았습니다. instance는 null을 return합니다.");
            }
            return _instance;
        }
    }
    private static ReplicationManager _instance;

    public void ProcessReplicationAction(InputMemoryBitStream inStream)
    {
        // 모든 패킷이 [NetworkId, ReplicationAction] 으로 시작
        // 생성: ClassId, dirty State가 포함된 데이터
        // 수정: dirty State가 포함된 데이터
        // 삭제: 없음.

        // inStream이 빌 때까지 반복 처리 필요
        while (inStream.GetRemainingBitCount() > (sizeof(uint) << 3))
        {
            ReplicationHeader rh = new ReplicationHeader();
            rh.Read(inStream);

            Debug.Log($"rh.Ra: {rh.Ra}, rh.Nid: {rh.Nid}");

            switch (rh.Ra)
            {
                case ReplicationAction.RA_Create:
                    ReplicationCreate(rh, inStream);
                    break;

                case ReplicationAction.RA_Update:
                    ReplicationUpdate(rh, inStream);
                    break;

                case ReplicationAction.RA_Delete:
                    ReplicationDelete();
                    break;
            }
        }
    }

    public GameObject ReplicationCreate(ReplicationHeader rh, InputMemoryBitStream inStream)
    {
        Debug.Log("RA_Create 도달 완료!");
        
        uint networkId = rh.Nid;
        uint classId = (uint)inStream.ReadInt();

        // if 구문으로 만들지만, GameObject 생성을 위한 Factory로 변경하면 좋을 것.

        GameObject go = null;
        Action<InputMemoryBitStream> readAction = null;

        if (classId == NetworkUtils.Parse4byteStringToUint("STCK"))
        {
            Shuttlecock shuttlecock = _badmintonControllerComponent.CreateShuttlecock();

            go = shuttlecock.gameObject;
            readAction = shuttlecock.Read;
        }
        else if (classId == NetworkUtils.Parse4byteStringToUint("PLYR"))
        {
            Player player = _badmintonControllerComponent.CreatePlayer();

            go = player.gameObject;
            readAction = player.Read;
        }

        if (go == null)
        {
            Debug.LogError($"[ReplicationCreate] Unknown classId {classId} in ReplicationCreate");
            return null;
        }

        var networkComponent = go.AddComponent<NetworkComponent>();
        networkComponent.NetworkId = networkId;
        networkComponent._readAction = readAction;
        networkComponent.Read(inStream);

        _linkingContext.AddGameObject(networkId, go);

        return go;
    }

    public void ReplicationUpdate(ReplicationHeader rh, InputMemoryBitStream inStream)
    {
        uint networkId = rh.Nid;

        GameObject gameObject = _linkingContext.GetGameObject(networkId);

        if (null == gameObject)
        {
            // 정상적이라면 Object가 생성된 이후에만 ReplicationUpdate 발생.
            // game 비정상 종료 시 linkingContext에 gameObject가 없는 상태로 ReplicationUpdate 넘어올 수 있다.
            // null 발생 가능. 비정상 종료 시에는 이후의 패킷이 모두 불필요하다고 보고
            // 아무 동작도 하지 않도록 처리.

            return;
        }
        
        // gameObject의 Read 호출
        NetworkComponent netAction = gameObject.GetComponent<NetworkComponent>();
        netAction.Read(inStream);
    }
    public void ReplicationDelete()
    {
        Debug.Log("RA_Delete 도달 완료!");
    }

    private LinkingContext _linkingContext;

    private ReplicationManager()
    {
        _linkingContext = LinkingContext.Instance;
    }

    private void Awake()
    {
        if (_instance != null && _instance != this)
        {
            Destroy(gameObject);
            return;
        }

        _instance = this;
        DontDestroyOnLoad(gameObject);

        _badmintonControllerComponent = FindFirstObjectByType<BadmintonControllerComponent>().GetComponent<BadmintonControllerComponent>();
    }

    private BadmintonControllerComponent _badmintonControllerComponent;
}
