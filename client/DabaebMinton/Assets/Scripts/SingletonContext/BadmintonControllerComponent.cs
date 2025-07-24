using UnityEngine;

public class BadmintonControllerComponent : MonoBehaviour
{
    public EPlayMode PlayMode { get; private set; }
    public BadmintonController BadmintonController { get; private set; }

    private InputManager _inputManager;

    public void TogglePlayMode()
    {
        switch (PlayMode)
        {
            case EPlayMode.None:
                PlayMode = EPlayMode.isLocal;
                break;
            case EPlayMode.isLocal:
                PlayMode = EPlayMode.isOnline;
                break;
            case EPlayMode.isOnline:
                PlayMode = EPlayMode.isLocal;
                break;
            default:
                PlayMode = EPlayMode.isLocal;
                break;
        }

        _inputManager.SetActionMapBy(PlayMode);
    }

    private void Initialize()
    {
        BadmintonNet badmintonNet = CreateBadmintonNet();
        Shuttlecock shuttlecock = CreateShuttlecock();
        float shortServiceLine = 1.98f;

        Player player1 = CreatePlayer("Player1", new Vector2(-3f, 3));
        Player player2 = CreatePlayer("Player2", new Vector2(3f, 3));

        BadmintonController.SetLevel(
            badmintonNet,
            shuttlecock,
            shortServiceLine);
        BadmintonController.SetPlayer(player1, player2);

        int difficulty = PlayerPrefs.GetInt("difficulty");
        BadmintonController.SetShuttlecockMovementStrategy((EShuttlecockSpeed)difficulty);

        BadmintonController.Initialize();
    }

    private BadmintonNet CreateBadmintonNet()
    {
        return GameObject.Find("BadmintonNet").GetComponent<BadmintonNet>();
    }

    private Shuttlecock CreateShuttlecock()
    {
        GameObject go = GameObject.FindGameObjectWithTag("Shuttlecock");

        if (go == null)
        {
            Debug.Log("[Error] Shuttlecock is null.");
            return null;
        }

        return go.GetComponent<Shuttlecock>();
    }

    private Player CreatePlayer(string name, Vector2 initPos)
    {
        Player player = InstantiatePlayer($"Prefabs/{name}", name, initPos);
        player.InitializeStat(GetInitialData(player.GetComponent<Player>().CharacterID));

        return player;
    }

    private Player InstantiatePlayer(string path, string name, Vector2 position)
    {
        GameObject playerPrefab = Resources.Load<GameObject>(path);
        GameObject player = Instantiate(playerPrefab);
        player.name = name;
        player.transform.position = position;

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

    private void Awake()
    {
        BadmintonPlayUIController badmintonPlayUIController =
            FindFirstObjectByType<BadmintonPlayUIController>().GetComponent<BadmintonPlayUIController>();

        if (badmintonPlayUIController == null)
        {
            Debug.LogError("[BadmintonControllerComponent] BadmintonPlayUIController not found!");
        }

        BadmintonController = new PlayableBadmintonController(badmintonPlayUIController);

        _inputManager = FindFirstObjectByType<InputManager>()
            .GetComponent<InputManager>();

        Debug.Log("[BadmintonControllerComponent] Initialized with BadmintonPlayUIController.");
    }

    private void Start()
    {
        Initialize();
    }
}

public enum EPlayMode
{
    None,
    isLocal,
    isOnline,
    MAX
}
