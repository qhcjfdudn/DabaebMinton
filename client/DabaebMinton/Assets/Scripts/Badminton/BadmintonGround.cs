using UnityEngine;

public class BadmintonGround : MonoBehaviour
{
    public EGroundType _groundType = EGroundType.None;

    private BadmintonPlayController _gameManager;

    private void Start()
    {
        _gameManager = FindFirstObjectByType<BadmintonPlayController>();
        if (_gameManager == null)
        {
            Debug.LogError($"[{gameObject.name}] BadmintonPlayController not found in the scene.");
        }
    }
}

public enum EGroundType
{
    None,
    Left,
    Right,
    Size
}