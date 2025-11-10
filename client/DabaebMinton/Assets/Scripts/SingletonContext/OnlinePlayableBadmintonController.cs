using System;
using UnityEngine;

public class OnlinePlayableBadmintonController : BadmintonController
{
    private BadmintonPlayUIController _badmintonPlayUIController;

    // UI
    private int _player1Score, _player2Score;
    private int _endScore;

    // Network
    private NetworkManager _networkManager;
    private RPCManager _rpcManager;

    private uint _playerCharacterNetworkId;

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
        _rpcManager = _networkManager._rpcManager;

        _networkManager.OnlinePlayServerIp = PlayerPrefs.GetString("OnlinePlayServerIp");
        _networkManager.OnlinePlayServerPort = UInt16.Parse(PlayerPrefs.GetString("OnlinePlayServerPort"));
        _networkManager.OnlinePlaySessionId = UInt32.Parse(PlayerPrefs.GetString("OnlinePlaySessionId"));
        _networkManager.OnlinePlayPlayerId = UInt32.Parse(PlayerPrefs.GetString("OnlinePlayPlayerId"));

        Debug.Log($"[OnlinePlayableBadmintonController] OnlinePlayServerIp: {_networkManager.OnlinePlayServerIp}");
        Debug.Log($"[OnlinePlayableBadmintonController] OnlinePlayServerPort: {_networkManager.OnlinePlayServerPort}");
        Debug.Log($"[OnlinePlayableBadmintonController] OnlinePlaySessionId: {_networkManager.OnlinePlaySessionId}");
        Debug.Log($"[OnlinePlayableBadmintonController] OnlinePlayPlayerId: {_networkManager.OnlinePlayPlayerId}");

        _networkManager.ConnectToOnlinePlayServer();

        _networkManager.SendHello();
    }

    public override void TouchGround(EGroundType groundType)
    {
    }

    public override void TouchPenaltyArea()
    {
    }

    public override void SetGamePlayStateReady()
    {
        base.SetGamePlayStateReady();

        uint playerId = _networkManager.OnlinePlayPlayerId;

        if (_player1.GetComponent<NetworkComponent>().OwnerPlayerId == playerId)
            _playerCharacterNetworkId = _player1.GetComponent<NetworkComponent>().NetworkId;
        else if (_player2.GetComponent<NetworkComponent>().OwnerPlayerId == playerId)
            _playerCharacterNetworkId = _player2.GetComponent<NetworkComponent>().NetworkId;

        SetClientReadyRpc();
    }

    // RPCs

    public void MovePlayerRpc(Vector2 value)
    {
        OutputMemoryBitStream outStream = _rpcManager.GetStream();

        // string의 key를 쓰지 않고, RPCNameType의 enum을 사용하면 bit가 많이 절약될 것이다.
        outStream.Write(NetworkUtils.Parse4byteStringToUint("MVPL"));
        outStream.Write(_playerCharacterNetworkId);

        outStream.Write(value.x);
        outStream.Write(value.y);
    }

    public void SetClientReadyRpc()
    {
        _rpcManager.GetStream().Write(NetworkUtils.Parse4byteStringToUint("CLRD"));
    }
}
