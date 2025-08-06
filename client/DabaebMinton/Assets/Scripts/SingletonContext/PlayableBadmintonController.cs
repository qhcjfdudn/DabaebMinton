using UnityEngine;
using UnityEngine.SceneManagement;

public class PlayableBadmintonController : BadmintonController
{
    private BadmintonPlayUIController _badmintonPlayUIController;

    private int _player1Score, _player2Score;
    private int _endScore;

    public PlayableBadmintonController(BadmintonPlayUIController badmintonPlayUIController)
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

        Debug.Log("[PlayableBadmintonController] Initialized with BadmintonPlayUIController.");
    }

    public override void TouchGround(EGroundType groundType)
    {
        if (IsGamePaused())
        {
            return;
        }

        _scoredPlayer = GetScoredPlayer(groundType);
        AddScoreTo(_scoredPlayer);

        if (groundType == EGroundType.Left)
        {
            PauseGame(EPauseReason.ShuttlecockTouchGroundLeft);
        }
        else if (groundType == EGroundType.Right)
        {
            PauseGame(EPauseReason.ShuttlecockTouchGroundRight);
        }
        else
        {
            Debug.LogWarning("[TouchGround] Unknown ground type.");
        }

        CheckGameEnd();
    }

    public override void TouchPenaltyArea()
    {
        if (IsGamePaused())
            return;

        _scoredPlayer = GetScoredPlayer();

        PauseGame(EPauseReason.ShuttlecockTouchPenaltyArea);
        AddScoreTo(_scoredPlayer);
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
}
