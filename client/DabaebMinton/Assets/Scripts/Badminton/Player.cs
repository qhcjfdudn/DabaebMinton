using System;
using UnityEngine;

public class Player : MonoBehaviour
{
    [field: SerializeField]
    public ECharacterID CharacterID {  get; private set; }

    [field: SerializeField]
    public float MoveVelocity { get; private set; }

    [field: SerializeField]
    public float JumpVelocity { get; private set; }

    [field: SerializeField]
    public float Power { get; private set; }

    public Vector2 MoveValue { get; set; }

    public bool JumpValue { get; set; }
    public int JumpCount { get; set; } = 0;

    private Rigidbody2D _rigidbody;
    private Animator _animator;
    private PlayerHand _playerHand;
    private BadmintonHitBox _hitBox;
    private SwingCharger _swingCharger;

    private PlayerState _state;

    public void InitializeStat(CharacterInitialData characterInitialData)
    {
        MoveVelocity = characterInitialData.moveVelocity;
        JumpVelocity = characterInitialData.jumpVelocity;
        Power = characterInitialData.power;
    }

    public void OnAnimationFrameUpdate(string eventData)
    {
        string[] splitedData = eventData.Split('.');

        if (splitedData.Length != 2)
        {
            Debug.LogError($"Invalid event data: {eventData}");
            return;
        }

        string frameName = splitedData[0];
        int frameIdx = int.Parse(splitedData[1]);

        switch (frameName)
        {
            case "LeftStanding":
                _playerHand.OnStangindLeft(frameIdx);
                break;
            case "RightStanding":
                _playerHand.OnStandingRight(frameIdx);
                break;
            case "LeftMove":
                _playerHand.OnMoveLeft(frameIdx);
                break;
            case "RightMove":
                _playerHand.OnMoveRight(frameIdx);
                break;
            case "Jump":
                break;
            default:
                Debug.LogError($"Unknown frame name: {frameName}");
                return;
        }
    }

    public void DecelerateVelocityX()
    {
        _rigidbody.linearVelocityX *= 0.2f;
    }

    public void StartCharging(ESwingChargerState state)
    {
        _swingCharger.StartCharging(state);
    }

    public void StopCharging(ESwingChargerState state)
    {
        if (_swingCharger.StopCharging(state))
        {
            BadmintonController controller = transform.parent.GetComponentInChildren<BadmintonControllerComponent>().Controller;

            switch (state)
            {
                case ESwingChargerState.Swing:
                    controller.SwingShuttlecock(this);
                    break;

                case ESwingChargerState.ActionSwing:
                    controller.ActionSwingShuttlecock(this);
                    break;
            }

            _swingCharger.Release();
        }
    }

    private enum EAnimationMoveState
    {
        Standing, Move
    };
    private enum EAnimationDirectionState
    {
        Left, Right
    };

    private void UpdateState()
    {
        PlayerState nextState = PlayerStateFactory.Get(this, _state);

        if (nextState != null && _state.GetType() != nextState.GetType())
        {
            _state.Exit();
            _state = nextState;
            _state.Enter(this);

            ChangeAnimation();
        }

        _state.Update();
    }
    private void ChangeAnimation()
    {
        Type stateType = _state.GetType();
        if (stateType == typeof(LeftMoveState))
        {
            _animator.SetInteger("Direction", (int)EAnimationDirectionState.Left);
            _animator.SetInteger("Move", (int)EAnimationMoveState.Move);
        }
        else if (stateType == typeof(RightMoveState))
        {
            _animator.SetInteger("Direction", (int)EAnimationDirectionState.Right);
            _animator.SetInteger("Move", (int)EAnimationMoveState.Move);
        }
        else if (stateType == typeof(JumpState))
        {

        }
        else if (stateType == typeof(StandingState))
        {
            _animator.SetInteger("Move", (int)EAnimationMoveState.Standing);
        }
    }

    private void Awake()
    {
        _rigidbody = GetComponent<Rigidbody2D>();
        _animator = GetComponent<Animator>();
        
        _playerHand = GetComponentInChildren<PlayerHand>();
        _hitBox = GetComponentInChildren<BadmintonHitBox>();
        _swingCharger = GetComponentInChildren<SwingCharger>();

        _state = new StandingState();
    }

    // Update is called once per frame
    private void Update()
    {
        UpdateState();
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        if ((collision.gameObject.CompareTag("Ground") || collision.gameObject.CompareTag("PenaltyArea")) && JumpCount > 0)
        {
            JumpCount = 0;
        }
    }
}

public enum ECharacterID
{
    None, Daramgee, Baebsae
}