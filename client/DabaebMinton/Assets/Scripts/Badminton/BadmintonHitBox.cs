using UnityEngine;

public class BadmintonHitBox : MonoBehaviour
{
    // 이 값으로 object의 영역을 결정한다.
    // 그 후 shuttlecock이 이 영역에 들어오고 나가면 flag를 켜고 끈다.
    private float _localPosXFromPlayer = 0.33f, _localPosYFromPlayer = 0.24f;
    private float width = 1.12f, height = 1.43f;

    private Player _player;
    private AccuracyPoint _accuracyPoint;

    public bool IncludesShuttlecock { get; private set; }

    private void Awake()
    {
        _player = transform.parent.GetComponent<Player>();
        _accuracyPoint = GetComponentInChildren<AccuracyPoint>();
    }

    private void Start()
    {
        if (_player.isLeftSide())
        {
            transform.localPosition = new Vector2(_localPosXFromPlayer, _localPosYFromPlayer);
            _accuracyPoint.transform.localPosition = new Vector2(0.25f, 0);
        }
        else
        {
            transform.localPosition = new Vector2(-_localPosXFromPlayer, _localPosYFromPlayer);
            _accuracyPoint.transform.localPosition = new Vector2(-0.25f, 0);
        }

        float racketHeight = transform.parent.GetComponentInChildren<PlayerRacket>().GetRacketHeight();
        float racketRatio = racketHeight / 0.68f;

        transform.localScale = new Vector2(width * racketRatio, height * racketRatio);
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Shuttlecock"))
        {
            IncludesShuttlecock = true;
        }
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.CompareTag("Shuttlecock"))
        {
            IncludesShuttlecock = false;
        }
    }
}
