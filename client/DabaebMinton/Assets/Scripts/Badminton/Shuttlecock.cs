using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shuttlecock : MonoBehaviour
{
    public BadmintonControllerComponent _badmintonControllerComponent;
    public NetAction _netAction;

    private ShuttlecockMovementStrategy _movementStrategy;
    public ShuttlecockMovementStrategy MovementStrategy {
        get { return _movementStrategy; }
        set
        {
            _movementStrategy?.disable();
            _movementStrategy = value;
            _movementStrategy.enable();
        }
    }

    private Rigidbody2D _rigidbody;

    public void Move(Vector2 newPosition)
    {
        _rigidbody.linearVelocity = Vector2.zero;
        transform.SetLocalPositionAndRotation(
            newPosition,
            Quaternion.AngleAxis(0f, Vector2.zero));
    }

    public void Hit(Vector2 force)
    {
        _rigidbody.linearVelocity = Vector2.zero;
        _rigidbody.AddForce(force);
    }

    public Vector2 GetDistanceFrom(Vector2 position)
    {
        return new Vector2(transform.position.x - position.x, transform.position.y - position.y);
    }

    public void Read(InputMemoryBitStream inStream)
    {
        bool hasLocation = inStream.ReadBool();
        if (hasLocation)
        {
            _rigidbody.position = inStream.ReadVector2();
        }

        bool hasVelocity = inStream.ReadBool();
        if (hasVelocity)
        {
            _rigidbody.linearVelocity = inStream.ReadVector2();
        }
    }

    private void SetSpriteRotation()
    {
        if (_rigidbody.linearVelocity.magnitude <= 1f)
        {
            return;
        }

        Vector2 norm = _rigidbody.linearVelocity.normalized;
        float angle = Mathf.Atan2(norm.y, norm.x) * Mathf.Rad2Deg;
        transform.rotation = Quaternion.AngleAxis(angle + 90, Vector3.forward);
    }

    private void Awake()
    {
        _rigidbody = GetComponent<Rigidbody2D>();

        _netAction = GetComponent<NetAction>();
        _netAction._readAction = Read;

        var value = ShuttlecockMovementStrategyFactory.EShuttlecockMovementType.Normal;
        MovementStrategy = ShuttlecockMovementStrategyFactory.CreateShuttlecockMovementStrategy(this, value);
    }

    private void Update()
    {
        SetSpriteRotation();
    }

    private void FixedUpdate()
    {
        MovementStrategy.FixedUpdate();
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.collider.CompareTag("Ground"))
        {
            BadmintonGround ground = collision.collider.GetComponent<BadmintonGround>();
            _badmintonControllerComponent.Controller.TouchGround(ground._groundType);

            return;
        }

        if (collision.collider.CompareTag("PenaltyArea"))
        {
            BadmintonPenaltyArea penaltyArea = collision.collider.GetComponent<BadmintonPenaltyArea>();
            _badmintonControllerComponent.Controller.TouchPenaltyArea();
            
            return;
        }
    }
}
