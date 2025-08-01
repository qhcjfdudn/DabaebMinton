using UnityEngine;
using Unity.MLAgents;

public class TrainingController : BadmintonController
{
    private float MAX_STEP;
    private int currentStep;

    private SimpleMultiAgentGroup _team1, _team2;

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
            player.GetComponent<PlayerAgent>().AddReward(0.5f);

        return isSwingSuccess;
    }

    public override void Initialize()
    {
        base.Initialize();

        _team1 = new SimpleMultiAgentGroup();
        _team2 = new SimpleMultiAgentGroup();

        _team1.RegisterAgent(_player1.GetComponent<PlayerAgent>());
        _team2.RegisterAgent(_player2.GetComponent<PlayerAgent>());

        MAX_STEP = 30f / Time.fixedDeltaTime;

        ResetScene();
    }

    public override void TouchGround(EGroundType groundType)
    {
        switch (groundType)
        {
            case EGroundType.Left:
                _team1.AddGroupReward(-1.0f);

                if (_lastTouchedPlayer == _player1)
                    _team2.AddGroupReward(0.3f);

                else if (_lastTouchedPlayer == _player2)
                    _team2.AddGroupReward(1.0f);

                break;
            case EGroundType.Right:
                _team2.AddGroupReward(-1.0f);

                if (_lastTouchedPlayer == _player2)
                    _team1.AddGroupReward(0.3f);

                else if (_lastTouchedPlayer == _player1)
                    _team1.AddGroupReward(1.0f);

                break;
        }

        _team1.EndGroupEpisode();
        _team2.EndGroupEpisode();

        ResetScene();
    }

    public override void TouchPenaltyArea()
    {
        if (_lastTouchedPlayer == _player1)
        {
            _team1.AddGroupReward(-1.0f);
            _team2.AddGroupReward(0.1f);
        }
        else if (_lastTouchedPlayer == _player2)
        {
            _team2.AddGroupReward(-1.0f);
            _team1.AddGroupReward(0.1f);
        }

        _team1.EndGroupEpisode();
        _team2.EndGroupEpisode();

        ResetScene();
    }

    public override void FixedUpdate()
    {
        ++currentStep;

        CheckReachedMaxStep();
    }

    private void CheckReachedMaxStep()
    {
        if (currentStep < MAX_STEP)
            return;

        _team1.EndGroupEpisode();
        _team2.EndGroupEpisode();

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

        currentStep = 0;
    }
}
