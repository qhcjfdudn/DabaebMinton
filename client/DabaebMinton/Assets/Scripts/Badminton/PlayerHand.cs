using UnityEngine;

public class PlayerHand : MonoBehaviour
{
    public PlayerHandMovement _handMovement;

    public SpriteRenderer _racketSpriteRenderer;

    private Player _player;

    public void OnStangindLeft(int frameIdx)
    {
        _handMovement.OnStandingLeft(frameIdx);
    }

    public void OnStandingRight(int frameIdx)
    {
        _handMovement.OnStandingRight(frameIdx);
    }

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
        _player = transform.parent.GetComponent<Player>();
        _handMovement = PlayerHandMovementFactory.Get(this, GetComponentInParent<Player>().CharacterID);
    }

    private void Start()
    {
        if (_player.isLeftSide())
        {
            transform.SetLocalPositionAndRotation(new Vector3(0.239f, -0.146f, 0f), Quaternion.Euler(0f, 0f, 290f));
        }
        else
        {
            transform.SetLocalPositionAndRotation(new Vector3(-0.238f, -0.16f, 0f), Quaternion.Euler(0f, 0f, -20f));
        }

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
