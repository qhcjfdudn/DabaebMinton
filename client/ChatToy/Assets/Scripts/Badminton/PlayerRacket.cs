using UnityEngine;

public class PlayerRacket : MonoBehaviour
{
    [Header("Badminton Racket Settings")]
    [Tooltip("BadmintonRacket ScriptableObject의 List를 구현합니다.")]
    public RacketDatabase _racketDatabase;

    [Tooltip("현재 선택된 BadmintonRacket입니다. \n" +
        "Racket의 이름을 정확히 입력하지 않으면 Default Racket이 선택됩니다.")]
    public string _selectedRacketName;

    public SpriteRenderer _racketSpriteRenderer;

    private RacketData _currentRacket;

    public float GetRacketHeight()
    {
        return _currentRacket.height;
    }

    private void ApplyRacketStats()
    {
        Debug.Log($"[{_currentRacket.racketName}] 적용됨 - 무게: {_currentRacket.weight}, 반발력: {_currentRacket.repulsionPower}");
        // 캐릭터 성능에 반영하는 코드 작성

        _racketSpriteRenderer.sprite = _currentRacket.sprite;
    }

    private void Awake()
    {
        _racketSpriteRenderer = GetComponent<SpriteRenderer>();

        _currentRacket = _racketDatabase.GetRacketBy(_selectedRacketName);
        ApplyRacketStats();
    }
}
