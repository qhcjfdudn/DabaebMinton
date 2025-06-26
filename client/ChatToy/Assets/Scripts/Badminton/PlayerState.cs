using UnityEngine;


abstract class PlayerState
{
    protected Player _player;

    public virtual void Enter(Player player) {
        _player = player;
    }
    public virtual void Exit() { }
    public virtual void Update() { }
}
class StandingState : PlayerState
{
}
class LeftMoveState : PlayerState
{
    public override void Update()
    {
        Rigidbody2D rigidbody = _player.GetComponent<Rigidbody2D>();
        float curY = rigidbody.linearVelocity.y;
        rigidbody.linearVelocity = new Vector2(-_player.MoveVelocity, curY);
    }
    public override void Exit()
    {
        _player.DecelerateVelocityX();
    }
}
class RightMoveState : PlayerState
{
    public override void Update()
    {
        Rigidbody2D rigidbody = _player.GetComponent<Rigidbody2D>();
        float curY = rigidbody.linearVelocity.y;
        rigidbody.linearVelocity = new Vector2(_player.MoveVelocity, curY);
    }

    public override void Exit()
    {
        _player.DecelerateVelocityX();
    }
}
class JumpState : PlayerState
{
    public override void Update()
    {
        Rigidbody2D rigidbody = _player.GetComponent<Rigidbody2D>();
        if (rigidbody.linearVelocity.y < 0f)
        {
            rigidbody.linearVelocity = new Vector2(rigidbody.linearVelocity.x, 0);
        }

        rigidbody.AddForce(Vector2.up * _player.JumpVelocity);

        _player.JumpValue = false;
    }
}

static class PlayerStateFactory
{
    public static PlayerState Get(Player player, PlayerState playerState)
    {
        if (player.JumpValue == true)
        {
            return new JumpState();
        }
        
        if (player.MoveValue == Vector2.left)
        {
            return new LeftMoveState();
        }

        if (player.MoveValue == Vector2.right)
        {
            return new RightMoveState();
        }

        if (player.MoveValue == Vector2.zero)
        {
            return new StandingState();
        }

        // Default State 필요
        return new StandingState();
    }
}