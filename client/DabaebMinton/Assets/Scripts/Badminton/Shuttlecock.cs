using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shuttlecock : MonoBehaviour
{
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
    
    // 자신이 왼쪽/오른쪽 중 어느 court에 있는지 알고 있는 flag 필요

    private Rigidbody2D _rigidbody;

    public void Move(Vector2 newPosition)
    {
        _rigidbody.linearVelocity = Vector2.zero;
        transform.SetPositionAndRotation(
            newPosition,
            Quaternion.AngleAxis(0f, Vector2.zero));
    }

    public void Hit(Vector2 force)
    {
        _rigidbody.linearVelocity = Vector2.zero;
        _rigidbody.AddForce(force);
    }

    public void Read(InputMemoryBitStream inStream)
    {
        float x = inStream.ReadFloat();
        float y = inStream.ReadFloat();

        transform.position = new Vector2(x, y);

        x = inStream.ReadFloat();
        y = inStream.ReadFloat();

        _rigidbody.linearVelocity = new Vector2(x, y);
    }

    private void SetSpriteRotation()
    {
        // AddForce()�� ����Ǳ� ������ �̵� ���������� rotation�� �����Ǿ�� �Ѵ�.
        if (_rigidbody.linearVelocity == Vector2.zero)
        {
            return;
        }

        // Sprite�� ��Ʋ���� ���ư��� ����� ��ġ�ϵ��� ����
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
}
