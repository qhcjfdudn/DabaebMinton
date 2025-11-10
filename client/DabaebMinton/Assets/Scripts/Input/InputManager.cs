using UnityEngine;
using UnityEngine.InputSystem;

public class InputManager : MonoBehaviour
{
    PlayerInput _playerInput;

    public void SetActionMapBy(EPlayMode playMode)
    {
        switch (playMode)
        {
            case EPlayMode.Local:
                SwitchCurrentActionMap("LocalPlayMode");
                Debug.Log("[InputManager] - Local Play Mode");
                break;
            case EPlayMode.Online:
                SwitchCurrentActionMap("OnlinePlayMode");
                Debug.Log("[InputManager] - Online Play Mode");
                break;
            default:
                Debug.LogError("[InputManager] - Invalid Play Mode");
                break;
        }
    }

    private void Awake()
    {
        _playerInput = GetComponent<PlayerInput>();
    }

    private void SwitchCurrentActionMap(string actionMapName)
    {
        DisableAllActionMaps();
        _playerInput.actions.FindActionMap(actionMapName).Enable();
    }

    private void DisableAllActionMaps()
    {
        foreach (var actionMap in _playerInput.actions.actionMaps)
        {
            actionMap.Disable();
        }
    }
}
