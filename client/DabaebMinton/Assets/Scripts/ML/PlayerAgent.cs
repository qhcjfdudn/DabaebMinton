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

        // Player Stat: 4
        sensor.AddObservation(_player.GetComponent<Rigidbody2D>().linearVelocity);
        sensor.AddObservation(_player.Power);
        sensor.AddObservation(_player.GetSwingCharger().ChargeGauge);

        // lastTouched: 1
        sensor.AddObservation(_badmintonController.GetLastTouchedIndex());

        // Distance between BadmintonNet: 2
        sensor.AddObservation(_badmintonController.GetDistanceFromBadmintonNetTo(_player.transform.position));
    }

    public override void OnActionReceived(ActionBuffers actions)
    {
        ArraySegment<int> moves = new (actions.DiscreteActions.Array, 0, 2);
        ArraySegment<int> swings = new (actions.DiscreteActions.Array, 2, 2);

        MoveAgent(moves);
        SwingAgent(swings);
        
        CalcReward();
    }

    public override void OnEpisodeBegin()
    {
        _lastSwingInput = _lastActionSwingInput = 0;
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

    private void CalcReward()
    {
        Player lastTouchedPlayer = _badmintonController.GetLastTouchedPlayer();

        if (lastTouchedPlayer != _player)
        {
            float playerDirX = _player.GetComponent<Rigidbody2D>().linearVelocity.x;
            float RelativeShuttlcockPosX = _shuttlecock.transform.position.x - _player.accuracyPoint.transform.position.x;
            if (Mathf.Abs(RelativeShuttlcockPosX) < 0.3f) // shuttlecock을 accuracyPoint에 머물게 한다면
            {
                AddReward(0.1f);
            }
            else if (playerDirX * RelativeShuttlcockPosX > 0) // shuttlecock 방향으로 내가 이동 중이라면 reward
                AddReward(0.05f);
        }
        else if (lastTouchedPlayer == _player)
        {
            // 상대방의 Ground를 구해서 그 위에 존재할 때 점수를 얻는 방식으로 변경 필요
            float xFromNetToMe = _badmintonController.GetDistanceFromBadmintonNetTo(transform.position).x;
            float xFromNetToShuttlecock = _badmintonController.GetDistanceFromBadmintonNetTo(_shuttlecock.transform.position).x;
            if (xFromNetToMe * xFromNetToShuttlecock < 0)
            {
                AddReward(0.1f);
            }
        }
    }
}
