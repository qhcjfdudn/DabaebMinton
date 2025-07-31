using UnityEngine;

public class TrainingController : BadmintonController
{
    private PlayerAgent _player1Agent, _player2Agent;

    public override bool ActionSwingShuttlecock(Player player)
    {
        bool isSwingSuccess = base.ActionSwingShuttlecock(player);

        if (isSwingSuccess)
            player.GetComponent<PlayerAgent>().AddReward(0.5f);

        return isSwingSuccess;
    }

    public override bool SwingShuttlecock(Player player)
    {
        bool isSwingSuccess = base.SwingShuttlecock(player);

        if (isSwingSuccess)
            player.GetComponent<PlayerAgent>().AddReward(0.3f);

        return isSwingSuccess;
    }

    public override void Initialize()
    {
        base.Initialize();

        _player1Agent = _player1.GetComponent<PlayerAgent>();
        _player2Agent = _player2.GetComponent<PlayerAgent>();

        ResetScene();
    }

    public override void TouchGround(EGroundType groundType)
    {
        switch (groundType)
        {
            case EGroundType.Left:
                _player1Agent.SetReward(-1.0f);
                _player2Agent.SetReward(1.0f);
                break;
            case EGroundType.Right:
                _player1Agent.SetReward(1.0f);
                _player2Agent.SetReward(-1.0f);
                break;
        }

        _player1Agent.EndEpisode();
        _player2Agent.EndEpisode();

        ResetScene();
    }

    public override void TouchPenaltyArea()
    {
        if (_lastTouchedPlayer == _player1)
        {
            _player1Agent.SetReward(-1.0f);
            _player2Agent.SetReward(1.0f);
        }
        else if (_lastTouchedPlayer == _player2)
        {
            _player2Agent.SetReward(-1.0f);
            _player1Agent.SetReward(1.0f);
        }

        _player1Agent.EndEpisode();
        _player2Agent.EndEpisode();

        ResetScene();
    }

    private void ResetScene()
    {
        _player1.transform.localPosition = new Vector2((_shortServiceLine + Random.Range(0f, 3f)) * -1f, 1.25f);
        _player1.GetComponent<Rigidbody2D>().linearVelocity = Vector2.zero;
        _player1.MoveValue = Vector2.zero;
        
        _player2.transform.localPosition = new Vector2(_shortServiceLine + Random.Range(0f, 3f), 1.25f);
        _player2.GetComponent<Rigidbody2D>().linearVelocity = Vector2.zero;
        _player2.MoveValue = Vector2.zero;

        float randomizedShuttlecockStartPosition = (_shortServiceLine + Random.Range(0f, 3f)) * (Random.Range(0f, 1f) < 0.5f ? 1 : -1);
        _shuttlecock.Move(new Vector2(randomizedShuttlecockStartPosition, 6));

        _lastTouchedPlayer = null;
    }
}
