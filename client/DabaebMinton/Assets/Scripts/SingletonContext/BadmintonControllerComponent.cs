using Unity.MLAgents;
using UnityEngine;

public class BadmintonControllerComponent : MonoBehaviour
{
    [field: SerializeField]
    public EPlayMode PlayMode { get; private set; }
    public BadmintonController Controller { get; private set; }

    private InputManager _inputManager;

    private int _playerCount;
    private int _positionOf1P;

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

        Player player1 = null, player2 = null;

        // PlayMode에 따른 Player or Agent 생성
        if (PlayMode == EPlayMode.Local)
        {
            if (_playerCount == 1)
            {
                if (_positionOf1P == 0) // Left
                {
                    PlayerAgent agent = CreatePlayerAgent("Player2", new Vector2(3f, 1.25f));

                    player1 = CreatePlayer("Player1", new Vector2(-3f, 1.25f));
                    player2 = agent.GetComponent<Player>();

                    agent.SetOpponent(player1);
                }
                else
                {
                    PlayerAgent agent = CreatePlayerAgent("Player1", new Vector2(-3f, 1.25f));

                    player1 = agent.GetComponent<Player>();
                    player2 = CreatePlayer("Player2", new Vector2(3f, 1.25f));

                    agent.SetOpponent(player2);
                }
            }
            else if (_playerCount == 2)
            {
                player1 = CreatePlayer("Player1", new Vector2(-3f, 1.25f));
                player2 = CreatePlayer("Player2", new Vector2(3f, 1.25f));
            }
        }
        else if (PlayMode == EPlayMode.Training)
        {
            PlayerAgent p1Agent = CreatePlayerAgent("Player1", new Vector2(-3f, 1.25f));
            PlayerAgent p2Agent = CreatePlayerAgent("Player2", new Vector2(3f, 1.25f));
            
            player1 = p1Agent.GetComponent<Player>();
            player2 = p2Agent.GetComponent<Player>();

            p1Agent.SetOpponent(player2);
            p2Agent.SetOpponent(player1);
        }
        else if (PlayMode == EPlayMode.Online)
        {
            // 온라인 플레이어는 네트워크 매니저가 ProcessPacket을 통해 생성.
            // 여기서는 아무 것도 하지 않는다.
        }

        Controller.SetPlayer(player1, player2);

        int difficulty = PlayerPrefs.GetInt("difficulty");
        Controller.SetShuttlecockMovementStrategy((EShuttlecockSpeed)difficulty);

        NetworkManager.Instance.onHelloFromServer += SetGamePlayStateReady;

        Controller.Initialize();
    }

    private BadmintonNet CreateBadmintonNet()
    {
        return transform.parent.GetComponentInChildren<BadmintonNet>();
    }

    public Shuttlecock CreateShuttlecock()
    {
        Shuttlecock shuttlecock = transform.parent.GetComponentInChildren<Shuttlecock>();

        if (shuttlecock == null)
        {
            Debug.LogError("[BadmintonControllerComponent] Shuttlecock not found in the scene.");
            return null;
        }

        return shuttlecock;
    }

    public Player CreatePlayer(string name, Vector2 initPos)
    {
        Player player = InstantiatePlayer($"Prefabs/{name}", name, initPos);
        return player;
    }

    public Player CreatePlayer()
    {
        Player player = InstantiatePlayer("Prefabs/Player", "Player", new Vector2(0, 0));

        if (Controller._player1 == null)
        {
            Controller._player1 = player;
        }
        else if (Controller._player2 == null)
        {
            Controller._player2 = player;
        }
        else
        {
            Debug.LogError("[BadmintonControllerComponent] Both players are already assigned.");
        }

        return player;
    }

    private PlayerAgent CreatePlayerAgent(string name, Vector2 initPos)
    {
        Player player = InstantiatePlayer($"Prefabs/ML/{name}Agent", name, initPos);
        return player.GetComponent<PlayerAgent>();
    }

    private Player InstantiatePlayer(string path, string name, Vector2 position)
    {
        GameObject playerPrefab = Resources.Load<GameObject>(path);
        GameObject player = Instantiate(playerPrefab, transform.parent);
        player.name = name;
        player.transform.localPosition = position;

        return player.GetComponent<Player>();
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

        if (PlayMode == EPlayMode.Local)
        {
            _playerCount = PlayerPrefs.GetInt("PlayerCount", 1);
            PlayerPrefs.DeleteKey("PlayerCount");

            _positionOf1P = PlayerPrefs.GetInt("PositionOf1P", 0);
            PlayerPrefs.DeleteKey("PositionOf1P");
        }
        else if (PlayMode == EPlayMode.Online)
        {
        }
    }

    private BadmintonController GetController()
    {
        if (PlayMode == EPlayMode.Local)
        {
            BadmintonPlayUIController uiController =
                FindFirstObjectByType<BadmintonPlayUIController>()
                .GetComponent<BadmintonPlayUIController>();

            if (uiController == null)
            {
                Debug.LogError("[BadmintonControllerComponent] BadmintonPlayUIController not found!");
            }

            var controller = BadmintonControllerFactory.GetPlayableBadmintonController(uiController);

            Debug.Log("[BadmintonControllerComponent] Initialized PlayableBadmintonController with BadmintonPlayUIController.");

            return controller;
        }

        if (PlayMode == EPlayMode.Online)
        {
            BadmintonPlayUIController uiController =
                FindFirstObjectByType<BadmintonPlayUIController>()
                .GetComponent<BadmintonPlayUIController>();

            if (uiController == null)
            {
                Debug.LogError("[BadmintonControllerComponent] BadmintonPlayUIController not found!");
            }

            var controller = BadmintonControllerFactory.GetOnlinePlayableBadmintonController(uiController);

            Debug.Log("[BadmintonControllerComponent] Initialized OnlinePlayableBadmintonController with BadmintonPlayUIController.");

            return controller;
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

    public void SetGamePlayStateReady()
    {
        Controller.SetGamePlayStateReady();
        _inputManager.SetActionMapBy(PlayMode); // 검증 이후 GamePlayState가 Plying에 들어갈 때 변경되도록 수정 필요
        
        if (PlayMode == EPlayMode.Online)
        {
            FindFirstObjectByType<OnlinePlayModeInputManager>().SetOnlinePlayableBadmintonController((OnlinePlayableBadmintonController)Controller);
        }
    }

    private void Awake()
    {
        InitSetting();
        Controller = GetController();
        Controller.badmintonControllerComponent = this;
    }

    private void Start()
    {
        Initialize();
    }

    private void FixedUpdate()
    {
        Controller.FixedUpdate();
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

    public static BadmintonController GetOnlinePlayableBadmintonController(BadmintonPlayUIController badmintonPlayUIController)
    {
        return new OnlinePlayableBadmintonController(badmintonPlayUIController);
    }

    public static BadmintonController GetTrainingController()
    {
        return new TrainingController();
    }
}