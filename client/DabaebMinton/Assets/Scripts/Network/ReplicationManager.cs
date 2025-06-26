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
        ReplicationHeader rh = new ReplicationHeader();
        rh.Read(inStream);

        Debug.Log($"rh.Ra: {rh.Ra}, rh.Nid: {rh.Nid}, rh.Cid: {rh.Cid}");

        switch (rh.Ra)
        {
            case ReplicationAction.RA_Create:
                ReplicationCreate();
                break;

            case ReplicationAction.RA_Update:
                ReplicationUpdate(rh, inStream);
                break;

            case ReplicationAction.RA_Delete:
                ReplicationDelete();
                break;
        }
    }

    public void ReplicationCreate()
    {
        Debug.Log("RA_Create 도달 완료!");
    }
    public void ReplicationUpdate(ReplicationHeader rh, InputMemoryBitStream inStream)
    {
        uint networkId = rh.Nid, classId = rh.Cid;

        Debug.Log("RA_Update 도달 완료!");
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
        NetAction netAction = gameObject.GetComponent<NetAction>();
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
    }
}
