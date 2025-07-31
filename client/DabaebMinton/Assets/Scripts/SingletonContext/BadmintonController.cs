using UnityEngine;
using static ShuttlecockMovementStrategyFactory;

public class BadmintonController
{
    protected Player _player1, _player2;
    protected Player _lastTouchedPlayer;
    protected Shuttlecock _shuttlecock;
    protected BadmintonNet _badmintonNet;
    protected float _shortServiceLine = 1.98f;

    protected Vector2 _shuttlecockInitialPosition = new Vector2(-5f, 6f);

    private EGamePlayState _gamePlayState;

    public virtual void Initialize()
    {

    }

    public void SetLevel(BadmintonNet badmintonNet, Shuttlecock shuttlecock, float shortServiceLine)
    {
        _badmintonNet = badmintonNet;
        _shuttlecock = shuttlecock;
        _shortServiceLine = shortServiceLine;
    }

    public void SetPlayer(Player player1, Player player2)
    {
        _player1 = player1;
        _player2 = player2;
    }

    public Shuttlecock GetShuttlecock() { return _shuttlecock; }

    public bool IsLastTouchedPlayer(Player player)
    {
        return _lastTouchedPlayer == player;
    }

    public virtual bool SwingShuttlecock(Player player)
    {
        Debug.Log("[SwingShuttlecock] called.");

        if (_lastTouchedPlayer == player)
            return false;

        BadmintonHitBox hitBox = player.GetComponentInChildren<BadmintonHitBox>();

        if (hitBox.IncludesShuttlecock == false)
        {
            Debug.Log("[Fail] Too long for hit the cock.");

            return false;
        }

        ClearShuttlecock(player);
        
        _lastTouchedPlayer = player;

        return true;
    }

    public virtual bool ActionSwingShuttlecock(Player player)
    {
        Debug.Log("[ActionSwingShuttlecock] called.");

        if (_lastTouchedPlayer == player)
            return false;

        // hitPoint -> accuracy box로 개선 필요
        BadmintonHitBox hitBox = player.GetComponentInChildren<BadmintonHitBox>();

        if (hitBox.IncludesShuttlecock == false)
        {
            Debug.Log("[Fail] Too long for hit the cock.");

            return false;
        }

        _lastTouchedPlayer = player;

        // 1. 셔틀콕이 맞는 순간의 네트 기준 높이, x축 위치, 세기를 통해 구종을 결정
        // 정확도를 통해 날아갈 궤적을 결정 // 구종마다 다르므로 구종 안에서 결정
        Vector2 shuttlecockPos = _shuttlecock.transform.position;

        float shuttlecockHeight = shuttlecockPos.y;
        float netHeight = _badmintonNet.transform.position.y + _badmintonNet.transform.lossyScale.y / 2;
        
        SwingCharger charger = player.GetComponentInChildren<SwingCharger>();
        float chargeRatio = (charger.ChargeGauge - SwingCharger.MIN_CHARGE_VALUE) / (SwingCharger.MAX_CHARGE_VALUE - SwingCharger.MIN_CHARGE_VALUE);

        if (_shortServiceLine > Mathf.Abs(player.transform.position.x))
        {
            if (shuttlecockHeight > netHeight)
            {
                Debug.Log("[Push] call!");
                PushShuttlecock(player);
                return true;
            }

            if (shuttlecockHeight <= netHeight)
            {
                Debug.Log("[Hairpin] call!");
                HairpinShuttlecock(player);
                return true;
            }

            return false;
        }

        if (shuttlecockHeight > netHeight && chargeRatio > 0.8)
        {
            Debug.Log("[Smash] call!");
            SmashShuttlecock(player);
            return true;
        }

        if (shuttlecockHeight > netHeight * 2 / 3 && chargeRatio > 0.8)
        {
            Debug.Log("[Drive] call!");
            DriveShuttlecock(player);

            return  true;
        }
        
        if (shuttlecockHeight > netHeight * 2 / 3)
        {
            Debug.Log("[Dropshot] call!");
            DropshotShuttlecock(player);
            return true;
        }

        return false;
    }

    public virtual void TouchGround(EGroundType groundType)
    {
        StartNewGame();
    }

    public virtual void TouchPenaltyArea()
    {
        StartNewGame();
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

    public void MoveShuttlecockInitialPosition()
    {
        _shuttlecock.Move(_shuttlecockInitialPosition);
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

            case EPauseReason.ShuttlecockTouchGroundLeft:
                Debug.Log("[PauseGame] Shuttlecock touched the left ground.");

                _gamePlayState = EGamePlayState.Paused;

                return true;

            case EPauseReason.ShuttlecockTouchGroundRight:
                Debug.Log("[PauseGame] Shuttlecock touched the right ground.");

                _gamePlayState = EGamePlayState.Paused;

                return true;

            case EPauseReason.ShuttlecockTouchPenaltyArea:
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
        _lastTouchedPlayer = null;
        _gamePlayState = EGamePlayState.Playing;
    }

    private Vector2 GetSwingForce(Player player, float baseAngle, float minAngle, float maxAngle) // 좌우 방향을 한 방향으로 계산 가능하도록 forwardDir 통해
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

}

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
    ShuttlecockTouchGroundLeft,
    ShuttlecockTouchGroundRight,
    ShuttlecockTouchPenaltyArea,
    MAX
}