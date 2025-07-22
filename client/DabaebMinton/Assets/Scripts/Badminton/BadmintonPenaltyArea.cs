using UnityEngine;

public class BadmintonPenaltyArea : MonoBehaviour
{
    private BadmintonPlayController _badmintonPlayController;

    private void Start()
    {
        _badmintonPlayController = FindFirstObjectByType<BadmintonPlayController>();
        if (_badmintonPlayController == null)
        {
            Debug.LogError($"[{gameObject.name}] GameManager not found in the scene.");
        }
    }
}