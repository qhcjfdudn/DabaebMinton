using System;
using Unity.MLAgents;
using Unity.MLAgents.Actuators;
using Unity.MLAgents.Sensors;
using UnityEngine;

public class PlayerAgent : Agent
{
    private Player _player;
    private BadmintonController _badmintonController;
    private Shuttlecock _shuttlecock;

    public override void Initialize()
    {
        _player = GetComponent<Player>();
        _badmintonController = transform.parent.GetComponentInChildren<BadmintonControllerComponent>().Controller;
        _shuttlecock = _badmintonController.GetShuttlecock();
    }

    public override void CollectObservations(VectorSensor sensor)
    {
        sensor.AddObservation(_shuttlecock.transform.localPosition.x);
        sensor.AddObservation(_shuttlecock.transform.localPosition.y);
    }

    public override void OnActionReceived(ActionBuffers actions)
    {
        ArraySegment<int> moves = new ArraySegment<int>(actions.DiscreteActions.Array, 0, 2);
        ArraySegment<int> swings = new ArraySegment<int>(actions.DiscreteActions.Array, 2, 2);

        MoveAgent(moves);
        SwingAgent(swings);
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
        int swing = swings[0];
        int actionSwing = swings[1];

        if (swing == 0)
        {
            _player.StartCharging(ESwingChargerState.Swing);
        }
        else if (swing == 1)
        {
            _player.StopCharging(ESwingChargerState.Swing);
        }

        if (actionSwing == 0)
        {
            _player.StartCharging(ESwingChargerState.ActionSwing);
        }
        else if (actionSwing == 1)
        {
            _player.StopCharging(ESwingChargerState.ActionSwing);
        }
    }
}
