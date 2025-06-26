using UnityEngine;

public class BadmintonHitBox : MonoBehaviour
{
    // 이 값으로 object의 영역을 결정한다.
    // 그 후 shuttlecock이 이 영역에 들어오고 나가면 flag를 켜고 끈다.
    private float _localPosXFromPlayer = 0.33f, _localPosYFromPlayer = 0.24f;
    private float width = 1.12f, height = 1.43f;

    private AccuracyPoint _accuracyPoint;

    public bool IncludesShuttlecock { get; private set; }

    private void Awake()
    {
        _accuracyPoint = GetComponentInChildren<AccuracyPoint>();
    }

    private void Start()
    {
        // Player 이름으로 HitBox 영역을 결정하는 게 마음에 안 들지만 당장 더 나은 아이디어가 떠오르지 않는다.
        if (transform.parent.name == "Player1")
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
