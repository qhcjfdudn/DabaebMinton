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

        if (classId == NetworkUtils.ParseClassIdToUint("STCK"))
        {
            Shuttlecock shuttlecock = _badmintonControllerComponent.CreateShuttlecock();

            go = shuttlecock.gameObject;
            readAction = shuttlecock.Read;
        }
        else if (classId == NetworkUtils.ParseClassIdToUint("PLYR"))
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
        Debug.Log("RA_Update 도달 완료!");
        
        uint networkId = rh.Nid;

        // LinkingContext 필요. networkId에 해당하는 데이터 찾기

        GameObject gameObject = _linkingContext.GetGameObject(networkId);
        if (null == gameObject)
        {
            // 새로운 gameObject를 생성해서 전달해야 하는데, 
            // classID를 기준으로 만드는 Factory 함수? 가 있어야 한다.
            gameObject = Instantiate(Resources.Load<GameObject>("Prefabs/Shuttlecock"));
            _linkingContext.AddGameObject(networkId, gameObject);
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
