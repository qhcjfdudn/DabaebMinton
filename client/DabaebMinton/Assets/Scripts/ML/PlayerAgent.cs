using Unity.MLAgents;
using Unity.MLAgents.Actuators;
using Unity.MLAgents.Sensors;
using UnityEngine;

public class PlayerAgent : Agent
{
    private Player _player;
    private BadmintonController _badmintonController;
    private Shuttlecock _shuttlecock;

    public void MoveAgent(ActionSegment<int> act)
    {
        var dirPushed = act[0];
        var jumpPushed = act[1];

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

    public override void Initialize()
    {
        // 학습 환경 설정 위한 호출
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
        MoveAgent(actions.DiscreteActions);
    }

    public override void OnEpisodeBegin()
    {
        
    }
}
