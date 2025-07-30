using UnityEngine;

public class TrainingController : BadmintonController
{
    public int maxSteps = 10_000;

    private PlayerAgent _player1Agent, _player2Agent;

    private Vector2 _player1StartPosition = new Vector2(-3.5f, 2f);
    private Vector2 _player2StartPosition = new Vector2(3.5f, 2f);
    private Vector2 _shuttlecockStartPosition = new Vector2(-2f, 4f);

    //private int _trainingTimer;

    public override bool ActionSwingShuttlecock(Player player)
    {
        bool isSwingSuccess = base.ActionSwingShuttlecock(player);

        if (isSwingSuccess)
            player.GetComponent<PlayerAgent>().AddReward(0.1f);

        return isSwingSuccess;
    }

    public override bool SwingShuttlecock(Player player)
    {
        bool isSwingSuccess = base.SwingShuttlecock(player);

        if (isSwingSuccess)
            player.GetComponent<PlayerAgent>().AddReward(0.1f);

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
        //_trainingTimer = 0;

        _player1.transform.localPosition = _player1StartPosition;
        _player2.transform.localPosition = _player2StartPosition;

        _shuttlecock.Move(_shuttlecockStartPosition);
    }
}
