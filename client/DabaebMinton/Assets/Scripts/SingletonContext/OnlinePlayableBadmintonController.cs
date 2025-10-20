using System;
using UnityEngine;

public class OnlinePlayableBadmintonController : BadmintonController
{
    private BadmintonPlayUIController _badmintonPlayUIController;

    private int _player1Score, _player2Score;
    private int _endScore;

    private NetworkManager _networkManager;

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

        _networkManager = NetworkManager.Instance;
        _networkManager.OnlinePlayServerIp = PlayerPrefs.GetString("OnlinePlayServerIp");
        _networkManager.OnlinePlayServerPort = UInt16.Parse(PlayerPrefs.GetString("OnlinePlayServerPort"));
        _networkManager.OnlinePlaySessionId = UInt32.Parse(PlayerPrefs.GetString("OnlinePlaySessionId"));
        Debug.Log($"[OnlinePlayableBadmintonController] OnlinePlayServerIp: {_networkManager.OnlinePlayServerIp}");
        Debug.Log($"[OnlinePlayableBadmintonController] OnlinePlayServerPort: {_networkManager.OnlinePlayServerPort}");
        Debug.Log($"[OnlinePlayableBadmintonController] OnlinePlaySessionId: {_networkManager.OnlinePlaySessionId}");

        _networkManager.ConnectToOnlinePlayServer();
        _networkManager.SendHello();
    }

    // RPC 동작을 위한 함수 정의

    public override void TouchGround(EGroundType groundType)
    {
    }

    public override void TouchPenaltyArea()
    {
    }

}
