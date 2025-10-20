using UnityEngine;
using UnityEngine.InputSystem;

public class OnlinePlayModeInputManager : MonoBehaviour
{
    public void OnPlayerMove(InputAction.CallbackContext context)
    {
        if (context.started)
            return;

        if (context.performed)
        {
            MoveRpc(context.ReadValue<Vector2>());
            return;
        }

        if (context.canceled)
        {
            MoveRpc(Vector2.zero);
        }
    }

    private void MoveRpc(Vector2 moveValue)
    {
        Debug.Log($"[OnlinePlayModeInputManager] - moveValue: {moveValue}");

        // RPC 전송
        // Player 지정할 필요 없음. SessionId를 넘기면 Server에서 처리 예정.
        
        // OutStream을 NetworkManager로부터 받아 옴. 값을 stream으로 출력한다.
        // NetworkManager가 적절한 주기로 전송한다.
    }
}
