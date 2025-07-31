using Unity.MLAgents;
using UnityEngine;

public class BadmintonControllerComponent : MonoBehaviour
{
    [field: SerializeField]
    public EPlayMode PlayMode { get; private set; }
    public BadmintonController Controller { get; private set; }

    private InputManager _inputManager;

    public void TogglePlayMode()
    {
        switch (PlayMode)
        {
            case EPlayMode.None:
                PlayMode = EPlayMode.Local;
                break;
            case EPlayMode.Local:
                PlayMode = EPlayMode.Online;
                break;
            case EPlayMode.Online:
                PlayMode = EPlayMode.Local;
                break;
            default:
                PlayMode = EPlayMode.Local;
                break;
        }

        _inputManager.SetActionMapBy(PlayMode);
    }

    private void Initialize()
    {
        float shortServiceLine = 1.98f;

        Controller.SetLevel(
            CreateBadmintonNet(),
            CreateShuttlecock(),
            shortServiceLine);

        // Local Play에서는 Player를 생성하고, 학습모드에서는 PlayerAgent가 포함된 Player를 생성.
        Player player1 = CreatePlayer("Player1", new Vector2(-3f, 3));
        Player player2 = CreatePlayer("Player2", new Vector2(3f, 3));

        Controller.SetPlayer(player1, player2);

        int difficulty = PlayerPrefs.GetInt("difficulty");
        Controller.SetShuttlecockMovementStrategy((EShuttlecockSpeed)difficulty);

        Controller.Initialize();
    }

    private BadmintonNet CreateBadmintonNet()
    {
        return transform.parent.GetComponentInChildren<BadmintonNet>();
    }

    private Shuttlecock CreateShuttlecock()
    {
        Shuttlecock shuttlecock = transform.parent.GetComponentInChildren<Shuttlecock>();

        if (shuttlecock == null)
        {
            Debug.LogError("[BadmintonControllerComponent] Shuttlecock not found in the scene.");
            return null;
        }

        return shuttlecock;
    }

    private Player CreatePlayer(string name, Vector2 initPos)
    {
        if (PlayMode == EPlayMode.Training)
            return CreatePlayerAgent(name, initPos);

        Player player = InstantiatePlayer($"Prefabs/{name}", name, initPos);
        player.InitializeStat(GetInitialData(player.GetComponent<Player>().CharacterID));

        return player;
    }

    private Player CreatePlayerAgent(string name, Vector2 initPos)
    {
        Player player = InstantiatePlayer($"Prefabs/ML/{name}Agent", name, initPos);
        player.InitializeStat(GetInitialData(player.GetComponent<Player>().CharacterID));
        return player;
    }

    private Player InstantiatePlayer(string path, string name, Vector2 position)
    {
        GameObject playerPrefab = Resources.Load<GameObject>(path);
        GameObject player = Instantiate(playerPrefab, transform.parent);
        player.name = name;
        player.transform.localPosition = position;

        return player.GetComponent<Player>();
    }

    private CharacterInitialData GetInitialData(ECharacterID characterID)
    {
        switch (characterID)
        {
            case ECharacterID.Daramgee:
                return Resources.Load<CharacterInitialData>("ScriptableObjects/CharacterInitialData/DaramgeeInitialData");
            case ECharacterID.Baebsae:
                return Resources.Load<CharacterInitialData>("ScriptableObjects/CharacterInitialData/BaebsaeInitialData");
        }

        return null;
    }

    private void InitSetting()
    {
        if (FindFirstObjectByType<InputManager>() != null)
        {
            _inputManager =
                FindFirstObjectByType<InputManager>()
                .GetComponent<InputManager>();
        }

        if (PlayMode == EPlayMode.None)
        {
            PlayMode = (EPlayMode)PlayerPrefs.GetInt("PlayMode", (int)EPlayMode.None);
            PlayerPrefs.DeleteKey("PlayMode");
        }

        Debug.Log($"[BadmintonControllerComponent] PlayMode: {PlayMode}");
    }

    private BadmintonController GetController()
    {
        if (PlayMode == EPlayMode.Local)
        {
            // 추후 1P, 2P 모드로 확장 예정. 현재는 2P 모드로 고정.

            BadmintonPlayUIController uiController =
                FindFirstObjectByType<BadmintonPlayUIController>()
                .GetComponent<BadmintonPlayUIController>();

            if (uiController == null)
            {
                Debug.LogError("[BadmintonControllerComponent] BadmintonPlayUIController not found!");
            }

            var controller = BadmintonControllerFactory.GetPlayableBadmintonController(uiController);

            Debug.Log("[BadmintonControllerComponent] Initialized with BadmintonPlayUIController.");

            return controller;
        }
        
        if (PlayMode == EPlayMode.Online)
        {
            return BadmintonControllerFactory.GetDefaultBadmintonController();
        }

        // 학습 모드
        if (PlayMode == EPlayMode.Training)
        {
            // UI 없음. Input은 Action으로 자체 발생 시키므로 없음.
            // Training Mode의 Controller가 필요하다면 생성하여 여기서 할당 가능.

            return BadmintonControllerFactory.GetTrainingController();
        }

        return BadmintonControllerFactory.GetDefaultBadmintonController();
    }

    private void Awake()
    {
        InitSetting();
        
        Controller = GetController();
    }

    private void Start()
    {
        Initialize();
    }
}

public enum EPlayMode
{
    None,
    Local,
    Online,
    Training,
    MAX
}

public static class BadmintonControllerFactory
{
    public static BadmintonController GetDefaultBadmintonController()
    {
        return new BadmintonController();
    }
    public static BadmintonController GetPlayableBadmintonController(BadmintonPlayUIController badmintonPlayUIController)
    {
        return new PlayableBadmintonController(badmintonPlayUIController);
    }
    public static BadmintonController GetTrainingController()
    {
        return new TrainingController();
    }
}