using UnityEngine;
using static ShuttlecockMovementStrategyFactory;

public class GameManager : MonoBehaviour
{
    public EPlayMode PlayMode { get; private set; }

    public BadmintonCourtConfig _badmintonCourtConfig;
    BadmintonNet _badmintonNet;
    Shuttlecock _shuttlecock;

    Player _player1, _player2;
    Vector2 _player1InitPos, _player2InitPos;
    
    Player _lastTouchedPlayer;

    private UIScore _uiScore;

    private EGamePlayState _gamePlayState;

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

        InputManager inputManagerController =
            FindFirstObjectByType<InputManager>()
            .GetComponent<InputManager>();

        inputManagerController.SetActionMapBy(PlayMode);
    }

    public void SwingShuttlecock(Player player)
    {
        Debug.Log("[SwingShuttlecock] called.");

        if (_shuttlecock == null)
        {
            Debug.Log("[Error] Shuttlecock is null.");

            return;
        }

        BadmintonHitBox hitBox = player.GetComponentInChildren<BadmintonHitBox>();

        if (hitBox.IncludesShuttlecock == false)
        {
            Debug.Log("[Fail] Too long for hit the cock.");

            return;
        }

        ClearShuttlecock(player);
        
        _lastTouchedPlayer = player;
    }

    public void ActionSwingShuttlecock(Player player)
    {
        Debug.Log("[ActionSwingShuttlecock] called.");

        // hitPoint -> accuracy box로 개선 필요
        BadmintonHitBox hitBox = player.GetComponentInChildren<BadmintonHitBox>();

        if (hitBox.IncludesShuttlecock == false)
        {
            Debug.Log("[Fail] Too long for hit the cock.");

            return;
        }

        // 1. 셔틀콕이 맞는 순간의 네트 기준 높이, x축 위치, 세기를 통해 구종을 결정
        // 정확도를 통해 날아갈 궤적을 결정 // 구종마다 다르므로 구종 안에서 결정
        Vector2 shuttlecockPos = _shuttlecock.transform.position;

        float shuttlecockHeight = shuttlecockPos.y;
        float netHeight = _badmintonNet.transform.position.y + _badmintonNet.transform.lossyScale.y / 2;
        
        SwingCharger charger = player.GetComponentInChildren<SwingCharger>();
        float chargeRatio = (charger.ChargeGauge - SwingCharger.MIN_CHARGE_VALUE) / (SwingCharger.MAX_CHARGE_VALUE - SwingCharger.MIN_CHARGE_VALUE);

        if (_badmintonCourtConfig.shortServiceLine > Mathf.Abs(player.transform.position.x))
        {
            if (shuttlecockHeight > netHeight)
            {
                Debug.Log("[Push] call!");
                PushShuttlecock(player);
                return;
            }

            if (shuttlecockHeight <= netHeight)
            {
                Debug.Log("[Hairpin] call!");
                HairpinShuttlecock(player);
                return;
            }

            return;
        }

        if (shuttlecockHeight > netHeight && chargeRatio > 0.8)
        {
            Debug.Log("[Smash] call!");
            SmashShuttlecock(player);
            return;
        }

        if (shuttlecockHeight > netHeight * 2 / 3 && chargeRatio > 0.8)
        {
            Debug.Log("[Drive] call!");
            DriveShuttlecock(player);

            return;
        }
        
        if (shuttlecockHeight > netHeight * 2 / 3)
        {
            Debug.Log("[Dropshot] call!");
            DropshotShuttlecock(player);
            return;
        }

        _lastTouchedPlayer = player;
    }

    public void ClearShuttlecock(Player player)
    {
        _shuttlecock.Hit(GetSwingForce(player, 45f, 30f, 90f));
    }

    public void SmashShuttlecock(Player player)
    {
        _shuttlecock.Hit(GetSwingForce(player, 0f, -10f, 30f));
    }

    public void DriveShuttlecock(Player player)
    {
        _shuttlecock.Hit(GetSwingForce(player, 15f, 5f, 30f));
    }

    public void DropshotShuttlecock(Player player)
    {
        _shuttlecock.Hit(GetSwingForce(player, 20f, 0f, 50f));
    }

    public void PushShuttlecock(Player player)
    {
        _shuttlecock.Hit(GetSwingForce(player, -20f, -30f, 20f));
    }

    public void HairpinShuttlecock(Player player)
    {
        _shuttlecock.Hit(GetSwingForce(player, 70f, 50f, 90f));
    }

    public Vector2 GetSwingForce(Player player, float baseAngle, float minAngle, float maxAngle) // 좌우 방향을 한 방향으로 계산 가능하도록 forwardDir 통해
                                                                                                // 좌표와 방향 수정한 뒤 비행 각도 계산.
                                                                                                // 이후 방향을 다시 맞춰준다.
    {
        BadmintonHitBox hitBox = player.GetComponentInChildren<BadmintonHitBox>();
        AccuracyPoint accuracyPoint = hitBox.GetComponentInChildren<AccuracyPoint>();

        Vector2 hitBoxPos = hitBox.transform.position;
        Vector2 accuracyPointPos = accuracyPoint.transform.position;
        Vector2 shuttlecockPos = _shuttlecock.transform.position;

        float forwardDir = _badmintonNet.transform.position.x - player.transform.position.x > 0 ? 1 : -1;

        accuracyPointPos.x *= forwardDir;
        shuttlecockPos.x *= forwardDir;
        hitBoxPos.x *= forwardDir;

        float degree = baseAngle;

        if (shuttlecockPos.x < accuracyPointPos.x)
        {
            float pivot = hitBoxPos.x - hitBox.transform.lossyScale.x / 2;
            float ratio = (accuracyPointPos.x - shuttlecockPos.x) / (accuracyPointPos.x - pivot);
            degree += ratio * (maxAngle - degree);
        }
        else
        {
            float pivot = hitBoxPos.x + hitBox.transform.lossyScale.x / 2;
            float ratio = (shuttlecockPos.x - accuracyPointPos.x) / (pivot - accuracyPointPos.x);
            degree -= ratio * (degree - minAngle);
        }

        float radian = degree * Mathf.Deg2Rad; // degree를 radian으로 변환
        Vector2 direction = new Vector2(Mathf.Cos(radian), Mathf.Sin(radian));
        direction.x *= forwardDir;

        SwingCharger charger = player.GetComponentInChildren<SwingCharger>();
        Vector2 force = direction * player.Power * charger.ChargeGauge;

        return force;
    }

    public void MoveShuttlecockInitialPosition()
    {
        _shuttlecock.Move(new Vector2(-5f, 6f));
    }

    public void ChangeShuttlecockMovementStrategy()
    {
        ShuttlecockMovementStrategy sh = _shuttlecock.MovementStrategy;
        EShuttlecockMovementType movementType = EShuttlecockMovementType.Slow;

        if (sh.GetType() == typeof(ShuttlecockMovementSlowStrategy))
        {
            movementType = EShuttlecockMovementType.Normal;
        }
        else if (sh.GetType() == typeof(ShuttlecockMovementNormalStrategy))
        {
            movementType = EShuttlecockMovementType.Fast;
            
        }
        else if (sh.GetType() == typeof(ShuttlecockMovementFastStrategy))
        {
            movementType = EShuttlecockMovementType.Slow;
        }

        _shuttlecock.MovementStrategy = CreateShuttlecockMovementStrategy(_shuttlecock, movementType);
    }

    public void PlaceInitPosition(Player player)
    {
        if (player == null)
        {
            Debug.LogError("Player is null.");
            return;
        }

        if (player == _player1)
        {
            player.transform.position = _player1InitPos;
        }
        else if (player == _player2)
        {
            player.transform.position = _player2InitPos;
        }
        else
        {
            Debug.LogError("Unknown player.");
        }

        player.GetComponent<Rigidbody2D>().linearVelocity = Vector3.zero;
    }

    public bool PauseGame(EPauseReason pauseReason)
    {
        if (_gamePlayState == EGamePlayState.Paused)
        {
            Debug.LogWarning("[PauseGame] Game is already paused.");

            return false;
        }

        switch (pauseReason)
        {
            case EPauseReason.None:
                Debug.LogWarning("[PauseGame] Pause reason is None.");
                break;
            case EPauseReason.ShuttlecockTouchTheGround:
                Debug.Log("[PauseGame] Shuttlecock touched the ground.");

                _gamePlayState = EGamePlayState.Paused;

                CalculateScore();

                return true;

            default:
                Debug.LogWarning($"[PauseGame] Unknown pause reason: {pauseReason}");
                break;
        }

        return true;
    }

    public void StartNewGame()
    {
        MoveShuttlecockInitialPosition();

        _gamePlayState = EGamePlayState.Playing;
    }

    private async void CalculateScore()
    {
        float shuttlecockX = _shuttlecock.transform.position.x;
        float netX = _badmintonNet.transform.position.x;
        float leftBoundary = -_badmintonCourtConfig.courtWidth / 2;
        float rightBoundary = _badmintonCourtConfig.courtWidth / 2;

        if (_lastTouchedPlayer == _player1)
        {
            if (netX < shuttlecockX && shuttlecockX < rightBoundary)
            {
                Debug.Log("[AddScore] Player 1 scores.");
                _uiScore.AddScore(1);
            }
            else
            {
                Debug.Log("[AddScore] Player 2 scores.");
                _uiScore.AddScore(2);
            }
        }
        else
        {
            if (leftBoundary < shuttlecockX && shuttlecockX < netX )
            {
                Debug.Log("[AddScore] Player 2 scores.");
                _uiScore.AddScore(2);
            }
            else
            {
                Debug.Log("[AddScore] Player 1 scores.");
                _uiScore.AddScore(1);
            }
        }

        await Awaitable.WaitForSecondsAsync(1f);

        StartNewGame();
    }

    private void CreateLevel()
    {
        CreateBadmintonCourt();
        CreateShuttlecock();
    }

    private void CreateBadmintonCourt()
    {
        GameObject court = GameObject.Find("BadmintonCourt");
        court.transform.localScale = new Vector3(_badmintonCourtConfig.courtWidth, 1, 1);
        court.transform.position = new Vector3(0, 0, 0);

        _badmintonNet = GameObject.Find("BadmintonNet").GetComponent<BadmintonNet>();
        _badmintonNet.transform.localScale = new Vector3(0.1f, _badmintonCourtConfig.netHeight, 1);
        _badmintonNet.transform.parent = court.transform;
        _badmintonNet.transform.localPosition = new Vector3(0, _badmintonNet.transform.localScale.y / 2 + court.transform.localScale.y / 2, 0);

        GameObject bottomLeft = GameObject.Find("BadmintonBottomLeft");
        bottomLeft.transform.localPosition = new Vector3(-_badmintonCourtConfig.courtWidth / 2 - bottomLeft.transform.localScale.x / 2, 0, 0);

        GameObject bottomRight = GameObject.Find("BadmintonBottomRight");
        bottomRight.transform.localPosition = new Vector3(_badmintonCourtConfig.courtWidth / 2 + bottomLeft.transform.localScale.x / 2, 0, 0);

        GameObject shortServiceLineLeft = GameObject.Find("BadmintonShortServiceLineLeft");
        shortServiceLineLeft.transform.localPosition = new Vector3(-_badmintonCourtConfig.shortServiceLine, 0, 0);

        GameObject shortServiceLineRight = GameObject.Find("BadmintonShortServiceLineRight");
        shortServiceLineRight.transform.localPosition = new Vector3(_badmintonCourtConfig.shortServiceLine, 0, 0);
    }

    private void CreateShuttlecock()
    {
        GameObject go = GameObject.FindGameObjectWithTag("Shuttlecock");

        if (go == null)
        {
            Debug.Log("[Error] Shuttlecock is null.");
            return;
        }

        _shuttlecock = go.GetComponent<Shuttlecock>();
    }

    private void CreatePlayer()
    {
        _player1 = InstantiatePlayer("Prefabs/Player1", "Player1", _player1InitPos);
        _player1.InitializeStat(GetInitialData(_player1.GetComponent<Player>().CharacterID));

        _player2 = InstantiatePlayer("Prefabs/Player2", "Player2", _player2InitPos);
        _player2.InitializeStat(GetInitialData(_player2.GetComponent<Player>().CharacterID));
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
        PlayMode = EPlayMode.isLocal;
    }

    // Start is called before the first frame update
    private void Start()
    {
        CreateLevel();

        _player1InitPos = new Vector2(-_badmintonCourtConfig.courtWidth / 2 + 2f, 5);
        _player2InitPos = new Vector2(_badmintonCourtConfig.courtWidth / 2 - 2f, 5);
        
        CreatePlayer();

        _uiScore = FindFirstObjectByType<UIScore>();

        Debug.Log("End of GameManager Start()");
    }
}

public enum EPlayMode { None, isLocal, isOnline, MAX }

public enum EGamePlayState
{
    None,
    Playing,
    Paused,
    End
}

public enum EPauseReason
{
    None,
    ShuttlecockTouchTheGround
}