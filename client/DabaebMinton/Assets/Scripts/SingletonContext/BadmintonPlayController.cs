using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;
using static ShuttlecockMovementStrategyFactory;

public class BadmintonPlayController : MonoBehaviour
{
    public EPlayMode PlayMode { get; private set; }

    BadmintonNet _badmintonNet;
    float shortServiceLine = 1.98f;
    Shuttlecock _shuttlecock;

    Player _player1, _player2;
    Vector2 _player1InitPos, _player2InitPos;
    
    Player _lastTouchedPlayer;

    private EGamePlayState _gamePlayState;

    private int _player1Score, _player2Score;
    private int _endScore;

    private BadmintonPlayUIController _badmintonPlayUIController;

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

        _lastTouchedPlayer = player;

        // 1. 셔틀콕이 맞는 순간의 네트 기준 높이, x축 위치, 세기를 통해 구종을 결정
        // 정확도를 통해 날아갈 궤적을 결정 // 구종마다 다르므로 구종 안에서 결정
        Vector2 shuttlecockPos = _shuttlecock.transform.position;

        float shuttlecockHeight = shuttlecockPos.y;
        float netHeight = _badmintonNet.transform.position.y + _badmintonNet.transform.lossyScale.y / 2;
        
        SwingCharger charger = player.GetComponentInChildren<SwingCharger>();
        float chargeRatio = (charger.ChargeGauge - SwingCharger.MIN_CHARGE_VALUE) / (SwingCharger.MAX_CHARGE_VALUE - SwingCharger.MIN_CHARGE_VALUE);

        if (shortServiceLine > Mathf.Abs(player.transform.position.x))
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

    public void SetShuttlecockMovementStrategy(EShuttlecockSpeed difficultyType)
    {
        switch (difficultyType)
        {
            case EShuttlecockSpeed.Slow:
                _shuttlecock.MovementStrategy = CreateShuttlecockMovementStrategy(_shuttlecock, EShuttlecockMovementType.Slow);
                break;
            case EShuttlecockSpeed.Normal:
                _shuttlecock.MovementStrategy = CreateShuttlecockMovementStrategy(_shuttlecock, EShuttlecockMovementType.Normal);
                break;
            case EShuttlecockSpeed.Fast:
                _shuttlecock.MovementStrategy = CreateShuttlecockMovementStrategy(_shuttlecock, EShuttlecockMovementType.Fast);
                break;
            default:
                Debug.LogError("Unknown difficulty type.");
                break;
        }
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

            case EPauseReason.ShuttlecockTouchTheGroundLeft:
                Debug.Log("[PauseGame] Shuttlecock touched the left ground.");

                _gamePlayState = EGamePlayState.Paused;

                return true;

            case EPauseReason.ShuttlecockTouchTheGroundRight:
                Debug.Log("[PauseGame] Shuttlecock touched the right ground.");

                _gamePlayState = EGamePlayState.Paused;

                return true;

            case EPauseReason.ShuttlecockTouchThePenaltyArea:
                Debug.Log("[PauseGame] Shuttlecock touched the penalty area.");
                
                _gamePlayState = EGamePlayState.Paused;
                
                return true;

            default:
                Debug.LogWarning($"[PauseGame] Unknown pause reason: {pauseReason}");
                break;
        }

        return true;
    }

    public bool IsGamePaused()
    {
        return _gamePlayState == EGamePlayState.Paused;
    }

    public void StartNewGame()
    {
        MoveShuttlecockInitialPosition();

        _gamePlayState = EGamePlayState.Playing;
    }

    public void TouchTheGround(EGroundType groundType)
    {
        if (IsGamePaused())
        {
            return;
        }

        if (groundType == EGroundType.Left)
        {
            PauseGame(EPauseReason.ShuttlecockTouchTheGroundLeft);
            AddScoreTo(_player2);
            CheckGameEnd();
        }
        else if (groundType == EGroundType.Right)
        {
            PauseGame(EPauseReason.ShuttlecockTouchTheGroundRight);
            AddScoreTo(_player1);
            CheckGameEnd();
        }
        else
        {
            Debug.LogWarning("[TouchTheGround] Unknown ground type.");
        }
    }

    public void TouchThePenaltyArea()
    {
        if (IsGamePaused())
        {
            return;
        }

        Player winPlayer = null;

        if (_lastTouchedPlayer == _player1)
            winPlayer = _player2;
        else if (_lastTouchedPlayer == _player2)
            winPlayer = _player1;
        else
        {
            Debug.LogWarning("[TouchThePenaltyArea] Last touched player is null.");
            return;
        }

        PauseGame(EPauseReason.ShuttlecockTouchThePenaltyArea);
        AddScoreTo(winPlayer);
        CheckGameEnd();
    }

    private void AddScoreTo(Player player)
    {
        if (player == _player1)
        {
            _player1Score++;
            _badmintonPlayUIController.SetScore(1, _player1Score);
        }
        else if (player == _player2)
        {
            _player2Score++;
            _badmintonPlayUIController.SetScore(2, _player2Score);
        }
    }

    private async void CheckGameEnd()
    {
        // 누군가 endScore 점수 달성시 게임 종료 및 메인 페이지로 이동
        if (_player1Score >= _endScore || _player2Score >= _endScore)
        {
            string winner = _player1Score >= _endScore ? _player1.name : _player2.name;

            _badmintonPlayUIController.ShowWinnerText(winner);
            QuitGame();
            return;
        }

        await Awaitable.WaitForSecondsAsync(1f);

        StartNewGame();
    }

    private async void QuitGame()
    {
        Debug.Log($"[GameEnd] A player reached {_endScore} points. Ending game...");
        Debug.Log("[QuitGame] Game is ending...");

        await Awaitable.WaitForSecondsAsync(3f);

        SceneManager.LoadScene("MainMenuScene");
    }

    private void CreateLevel()
    {
        CreateBadmintonCourt();
        CreateShuttlecock();
    }

    private void CreateBadmintonCourt()
    {
        _badmintonNet = GameObject.Find("BadmintonNet").GetComponent<BadmintonNet>();
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

        _player1InitPos = new Vector2(-3f, 3);
        _player2InitPos = new Vector2(3f, 3);
        
        CreatePlayer();

        _endScore = PlayerPrefs.GetInt("score");
        int difficulty = PlayerPrefs.GetInt("difficulty");
        SetShuttlecockMovementStrategy((EShuttlecockSpeed)difficulty);

        Debug.Log($"Score: {_endScore}, Difficulty: {difficulty}");

        _badmintonPlayUIController = FindFirstObjectByType<BadmintonPlayUIController>();

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
    ShuttlecockTouchTheGroundLeft,
    ShuttlecockTouchTheGroundRight,
    ShuttlecockTouchThePenaltyArea,
    MAX
}