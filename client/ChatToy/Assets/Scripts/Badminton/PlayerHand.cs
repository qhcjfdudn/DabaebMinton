using UnityEngine;

public class PlayerHand : MonoBehaviour
{
    public PlayerHandMovement _handMovement;

    public SpriteRenderer _racketSpriteRenderer;

    public void OnMoveLeft(int frameIdx)
    {
        _handMovement.OnMoveLeft(frameIdx);
    }

    public void OnMoveRight(int frameIdx)
    {
        _handMovement.OnMoveRight(frameIdx);
    }

    private void Awake()
    {
        _handMovement = PlayerHandMovementFactory.Get(this, GetComponentInParent<Player>().CharacterID);
    }

    private void Start()
    {
        PlayerRacket racket = GetComponentInChildren<PlayerRacket>();
        if (racket == null)
        {
            Debug.LogError("PlayerRacket component not found in parent.");
        }
        else
        {
            _racketSpriteRenderer = racket.GetComponent<SpriteRenderer>();
            if (_racketSpriteRenderer == null)
            {
                Debug.LogError("SpriteRenderer component not found in PlayerRacket.");
            }
        }
    }
}
