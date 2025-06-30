using UnityEngine;

public abstract class PlayerHandMovement
{
    protected PlayerHand _owner;

    public virtual void OnStandingLeft(int frameIdx) { }
    public virtual void OnStandingRight(int frameIdx) { }
    public virtual void OnMoveLeft(int frameIdx) { }
    public virtual void OnMoveRight(int frameIdx) { }
}

public static class PlayerHandMovementFactory
{
    public static PlayerHandMovement Get(PlayerHand playerHand, ECharacterID characterID)
    {
        switch (characterID)
        {
            case ECharacterID.Daramgee:
                return new PlayerHandMovementDaramgee(playerHand);
            case ECharacterID.Baebsae:
                return new PlayerHandMovementBaebsae(playerHand);
            default:
                return null;
        }
    }
}