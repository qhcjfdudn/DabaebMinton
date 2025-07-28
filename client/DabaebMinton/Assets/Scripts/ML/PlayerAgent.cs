using Unity.MLAgents;
using Unity.MLAgents.Actuators;
using Unity.MLAgents.Sensors;
using UnityEngine;

public class PlayerAgent : Agent
{
    private BadmintonController _badmintonController;
    private Player _player;

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
        _player = GetComponent<Player>();
        _badmintonController = FindFirstObjectByType<BadmintonControllerComponent>().Controller;
    }

    public override void OnActionReceived(ActionBuffers actions)
    {
        MoveAgent(actions.DiscreteActions);
    }

    public override void OnEpisodeBegin()
    {
        _badmintonController.StartNewGame();
    }
}
