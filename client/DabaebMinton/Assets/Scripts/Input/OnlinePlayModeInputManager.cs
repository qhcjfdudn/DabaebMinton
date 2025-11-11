using UnityEngine;
using UnityEngine.InputSystem;

public class OnlinePlayModeInputManager : MonoBehaviour
{
    private OnlinePlayableBadmintonController _onlinePlayableBadmintonController;

    public void SetOnlinePlayableBadmintonController(OnlinePlayableBadmintonController controller)
    {
        _onlinePlayableBadmintonController = controller;
    }

    public void OnPlayerMove(InputAction.CallbackContext context)
    {
        if (context.started)
            return;

        if (context.performed)
        {
            _onlinePlayableBadmintonController.MovePlayerRpc(context.ReadValue<Vector2>());
            return;
        }

        if (context.canceled)
        {
            _onlinePlayableBadmintonController.MovePlayerRpc(Vector2.zero);
        }
    }

    public void OnPlayerJump(InputAction.CallbackContext context)
    {
        if (context.performed)
        {
            _onlinePlayableBadmintonController.JumpPlayerRpc();
            return;
        }
    }
}
