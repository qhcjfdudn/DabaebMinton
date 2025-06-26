
using UnityEngine;

public abstract class ShuttlecockMovementStrategy
{
    protected Shuttlecock _owner;
    protected float dampingBackup;
    protected float gravityBackup;
    
    protected ShuttlecockMovementData _movementData;

    protected ShuttlecockMovementStrategy(Shuttlecock owner)
    {
        _owner = owner;
    }

    public virtual void enable() {
        Rigidbody2D rb = _owner.GetComponent<Rigidbody2D>();
        dampingBackup = rb.linearDamping;
        gravityBackup = rb.gravityScale;
        rb.linearDamping = _movementData.Damping;
        rb.gravityScale = _movementData.GravityScale;
    }

    public virtual void disable()
    {
        Rigidbody2D rb = _owner.GetComponent<Rigidbody2D>();
        rb.linearDamping = dampingBackup;
        rb.gravityScale = gravityBackup;
    }
    public virtual void FixedUpdate()
    {
        Rigidbody2D rb = _owner.GetComponent<Rigidbody2D>();
        rb.linearVelocityX *= Mathf.Exp(-1f * Time.fixedDeltaTime);
    }
}

public class ShuttlecockMovementSlowStrategy : ShuttlecockMovementStrategy
{
    public ShuttlecockMovementSlowStrategy(Shuttlecock owner) : base(owner) {
        _movementData = Resources.Load<ShuttlecockMovementData>("ScriptableObjects/ShuttlecockMovement/ShuttlecockMovementData_Slow");
    }
}

public class ShuttlecockMovementNormalStrategy : ShuttlecockMovementStrategy
{
    public ShuttlecockMovementNormalStrategy(Shuttlecock owner) : base(owner) {
        _movementData = Resources.Load<ShuttlecockMovementData>("ScriptableObjects/ShuttlecockMovement/ShuttlecockMovementData_Normal");
    }
}

public class ShuttlecockMovementFastStrategy : ShuttlecockMovementStrategy
{
    public ShuttlecockMovementFastStrategy(Shuttlecock owner) : base(owner) {
        _movementData = Resources.Load<ShuttlecockMovementData>("ScriptableObjects/ShuttlecockMovement/ShuttlecockMovementData_Fast");
    }
}

public static class ShuttlecockMovementStrategyFactory
{
    public enum EShuttlecockMovementType
    {
        Slow,
        Normal,
        Fast,
        SIZE
    }
    public static ShuttlecockMovementStrategy CreateShuttlecockMovementStrategy(Shuttlecock shuttlecock, EShuttlecockMovementType movementType)
    {
        switch (movementType)
        {
            case EShuttlecockMovementType.Slow:
                return new ShuttlecockMovementSlowStrategy(shuttlecock);

            case EShuttlecockMovementType.Normal:
                return new ShuttlecockMovementNormalStrategy(shuttlecock);

            case EShuttlecockMovementType.Fast:
                return new ShuttlecockMovementFastStrategy(shuttlecock);

            // 다른 전략 추가 가능
            default:
                throw new System.ArgumentException("Invalid strategy type");
        }
    }
}