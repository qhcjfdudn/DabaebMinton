using UnityEngine;

public class BadmintonCourt : MonoBehaviour
{
    private GameManager _gameManager;

    private void Start()
    {
        _gameManager = FindFirstObjectByType<GameManager>();
        if (_gameManager == null)
        {
            Debug.LogError($"[{gameObject.name}] GameManager not found in the scene.");
        }
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        // 현재가 GamePlay라면 아래 코드를 돌겠지만 Training이라면 UI 또는 Game 상태와 이어지는 게 불필요해
        // Reward로 이어지거나 변경이 필요한데
        // 그렇다고 여기서 if문을 사용하는 건 좋지 않다. interface?

        if (collision.collider.CompareTag("Shuttlecock"))
        {
            if (_gameManager.IsGamePaused())
                return;

            _gameManager.PauseGame(EPauseReason.ShuttlecockTouchTheCourt);

            return;
        }
    }
}
