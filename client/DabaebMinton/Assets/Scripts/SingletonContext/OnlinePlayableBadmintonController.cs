using UnityEngine;
using UnityEngine.SceneManagement;

public class OnlinePlayableBadmintonController : BadmintonController
{
    private BadmintonPlayUIController _badmintonPlayUIController;

    private int _player1Score, _player2Score;
    private int _endScore;

    public OnlinePlayableBadmintonController(BadmintonPlayUIController badmintonPlayUIController)
        : base()
    {
        _badmintonPlayUIController = badmintonPlayUIController;
    }

    public override void Initialize()
    {
        base.Initialize();

        _player1Score = 0;
        _player2Score = 0;
        _endScore = PlayerPrefs.GetInt("score");
        Debug.Log($"Score: {_endScore}");

        _badmintonPlayUIController.Initialize();

        _badmintonPlayUIController.SetScore(1, _player1Score);
        _badmintonPlayUIController.SetScore(2, _player2Score);

        Debug.Log("[OnlinePlayableBadmintonController] Initialized with BadmintonPlayUIController.");
    }

    // RPC 동작을 위한 함수 정의

    public override void TouchGround(EGroundType groundType)
    {
    }

    public override void TouchPenaltyArea()
    {
    }
}
