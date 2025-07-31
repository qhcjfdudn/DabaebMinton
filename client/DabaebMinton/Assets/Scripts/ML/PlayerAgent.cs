using System;
using Unity.MLAgents;
using Unity.MLAgents.Actuators;
using Unity.MLAgents.Sensors;
using UnityEngine;

public class PlayerAgent : Agent
{
    private BadmintonController _badmintonController;
    private Shuttlecock _shuttlecock;
    private Player _player;
    
    private int _lastSwingInput, _lastActionSwingInput;

    public override void Initialize()
    {
        _badmintonController = transform.parent.GetComponentInChildren<BadmintonControllerComponent>().Controller;
        _shuttlecock = _badmintonController.GetShuttlecock();
        
        _player = GetComponent<Player>();
    }

    public override void CollectObservations(VectorSensor sensor)
    {
        // Shuttlecock direction: 3
        Vector2 relativePos = _shuttlecock.transform.position - transform.position;
        sensor.AddObservation(relativePos.normalized);
        sensor.AddObservation(relativePos.magnitude);

        // Player Position: 2
        sensor.AddObservation(transform.localPosition.x);
        sensor.AddObservation(transform.localPosition.y);

        // Player Stat: 3
        sensor.AddObservation(_player.MoveVelocity);
        sensor.AddObservation(_player.JumpVelocity);
        sensor.AddObservation(_player.Power);

        // lastTouched: 1
        int lastTouchedIdx;
        Player lastTouchedPlayer = _badmintonController.GetLastTouchedPlayer();
        if (lastTouchedPlayer == null)
            lastTouchedIdx = 0;
        else if (lastTouchedPlayer == _player)
            lastTouchedIdx = 1;
        else lastTouchedIdx = 2;

        sensor.AddObservation(lastTouchedIdx);
    }

    public override void OnActionReceived(ActionBuffers actions)
    {
        CalcReward();

        ArraySegment<int> moves = new (actions.DiscreteActions.Array, 0, 2);
        ArraySegment<int> swings = new (actions.DiscreteActions.Array, 2, 2);

        MoveAgent(moves);
        SwingAgent(swings);
    }

    public override void OnEpisodeBegin()
    {
        _lastSwingInput = _lastActionSwingInput = 0;
    }

    private void CalcReward()
    {
        // dist의 x 값이 [0, 0.5]일 때 [0.2, 0.0]점을 linear하게 받는다.
        Vector2 dist = _shuttlecock.GetDistanceFrom(_player.accuracyPoint.transform.position);
        float x = Mathf.Clamp(Mathf.Abs(dist.x), 0, 0.5f);
        AddReward(0.2f - 0.4f * x);

        if (_badmintonController.GetLastTouchedPlayer() == _player)
        {
            float xFromNetToMe = _badmintonController.GetDistanceFromBadmintonNetTo(transform.position).x;
            float xFromNetToShuttlecock = _badmintonController.GetDistanceFromBadmintonNetTo(_shuttlecock.transform.position).x;
            if (xFromNetToMe * xFromNetToShuttlecock < 0)
            {
                AddReward(0.1f);
            }
        }
    }

    private void MoveAgent(ArraySegment<int> moves)
    {
        int dirPushed = moves[0];
        int jumpPushed = moves[1];

        switch (dirPushed)
        {
            case 0: // Standing
                _player.MoveValue = Vector2.zero;
                break;
            case 1: // LeftMove
                _player.MoveValue = Vector2.left;
                break;
            case 2: // RightMove
                _player.MoveValue = Vector2.right;
                break;
        }

        if (jumpPushed == 1) // Jump
        {
            _player.JumpValue = true;
        }
        else
        {
            _player.JumpValue = false;
        }
    }

    private void SwingAgent(ArraySegment<int> swings)
    {
        int swingInput = swings[0];
        int actionSwingInput = swings[1];

        if (_lastSwingInput == 1 && swingInput == 0)
        {
            _player.StopCharging(ESwingChargerState.Swing);
        }
        else if (_lastSwingInput == 0 && swingInput == 1)
        {
            _player.StartCharging(ESwingChargerState.Swing);
        }
        _lastSwingInput = swingInput;

        if (_lastActionSwingInput == 1 && actionSwingInput == 0)
        {
            _player.StopCharging(ESwingChargerState.ActionSwing);
        }
        else if (_lastActionSwingInput == 0 && actionSwingInput == 1)
        {
            _player.StartCharging(ESwingChargerState.ActionSwing);
        }
        _lastActionSwingInput = actionSwingInput;
    }

}
