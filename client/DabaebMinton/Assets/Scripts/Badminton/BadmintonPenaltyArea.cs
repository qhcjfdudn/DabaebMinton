using UnityEngine;

public class BadmintonPenaltyArea : MonoBehaviour
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
        if (collision.collider.CompareTag("Shuttlecock"))
        {
            Debug.Log($"[Collision] {gameObject.name}");

            _gameManager.PauseGame(EPauseReason.ShuttlecockTouchTheCourt);

            return;
        }
    }
}
