using UnityEngine;
using UnityEngine.InputSystem;

public class OnlinePlayModeInputManager : MonoBehaviour
{
    GameManager _gameManager;
    Player _player;

    private void Start()
    {
        _gameManager = FindFirstObjectByType<GameManager>().GetComponent<GameManager>();
        //_player = FindPlayer("Player2");
    }

    public void OnPlayerMove(InputAction.CallbackContext context)
    {
        if (_player == null)
            _player = FindPlayer("Player2");

        if (context.started)
            return;

        if (context.performed)
            Move(_player, context.ReadValue<Vector2>());
    }

    private void Move(Player player, Vector2 moveValue)
    {
        Debug.Log($"OnlinePlayModeInputManager - player: {player}, moveValue: {moveValue}");

        // RPC 전송
    }

    public void OnPlayerJump(InputAction.CallbackContext context)
    {
        if (_player == null)
            _player = FindPlayer("Player2");

        if (context.started)
            return;

        if (context.performed)
            Jump(_player.GetComponent<Player>());

    }

    private void Jump(Player player)
    {
        Debug.Log($"OnlinePlayModeInputManager - Jump player: {player}");

        // online이면 RPC 전송
    }

    private Player FindPlayer(string playerName)
    {
        GameObject playerGameObject = GameObject.Find(playerName);

        if (playerGameObject == null)
        {
            Debug.LogError($"{playerName} not found!");
            throw new System.Exception($"{playerName} not found!");
        }

        return playerGameObject.GetComponent<Player>();
    }
}
