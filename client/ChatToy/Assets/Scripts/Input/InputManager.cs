using UnityEngine;
using UnityEngine.InputSystem;

public class InputManager : MonoBehaviour
{
    PlayerInput _playerInput;

    void Start()
    {
        _playerInput = GetComponent<PlayerInput>();
    }

    public void SetActionMapBy(EPlayMode playMode)
    {
        switch (playMode)
        {
            case EPlayMode.isLocal:
                _playerInput.SwitchCurrentActionMap("LocalPlayMode");
                Debug.Log("InputManagerController - Local Play Mode");
                break;
            case EPlayMode.isOnline:
                _playerInput.SwitchCurrentActionMap("OnlinePlayMode");
                Debug.Log("InputManagerController - Online Play Mode");
                break;
            default:
                Debug.LogError("InputManagerController - Invalid Play Mode");
                break;
        }
    }
}
