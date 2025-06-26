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
        if (collision.collider.CompareTag("Shuttlecock"))
        {
            _gameManager.PauseGame(EPauseReason.ShuttlecockTouchTheGround);

            return;
        }
    }
}
