using UnityEngine;
using UnityEngine.InputSystem;

public class LocalPlayModeInputManager : MonoBehaviour
{
    private BadmintonController _badmintonController;
    private Player _player1, _player2;

    public void OnPlayer1Move(InputAction.CallbackContext context)
    {
        if (_player1 == null)
            _player1 = FindPlayer("Player1");

        if (context.started)
        {
            return;
        }

        if (context.performed)
        {
            Move(_player1, context.ReadValue<Vector2>());
            return;
        }

        if (context.canceled)
        {
            Move(_player1, Vector2.zero);
            return;
        }
    }

    public void OnPlayer2Move(InputAction.CallbackContext context)
    {
        if (_player2 == null)
            _player2 = FindPlayer("Player2");

        if (context.started)
            return;

        if (context.performed)
        {
            Move(_player2, context.ReadValue<Vector2>());
            return;
        }

        if (context.canceled)
        {
            Move(_player2, Vector2.zero);
        }
    }

    public void OnPlayer1Jump(InputAction.CallbackContext context)
    {
        if (_player1 == null)
            _player1 = FindPlayer("Player1");

        if (context.started)
            return;

        if (context.performed)
            Jump(_player1, true);

        if (context.canceled)
            Jump(_player1, false);

    }

    public void OnPlayer2Jump(InputAction.CallbackContext context)
    {
        if (_player2 == null)
            _player2 = FindPlayer("Player2");

        if (context.started)
            return;

        if (context.performed)
            Jump(_player2, true);

        if (context.canceled)
            Jump(_player2, false);
    }

    public void OnPlayer1Swing(InputAction.CallbackContext context)
    {
        if (_player1 == null)
            _player1 = FindPlayer("Player1");

        if (context.started)
        {
            _player1.StartCharging(ESwingChargerState.Swing);
            return;
        }

        // charge 세기에 따라 때린다.
        if (context.canceled)
        {
            _player1.StopCharging(ESwingChargerState.Swing);
            return;
        }
    }

    public void OnPlayer2Swing(InputAction.CallbackContext context)
    {
        if (_player2 == null)
            _player2 = FindPlayer("Player2");

        if (context.started)
        {
            _player2.StartCharging(ESwingChargerState.Swing);
            return;
        }

        if (context.canceled)
        {
            _player2.StopCharging(ESwingChargerState.Swing);
            return;
        }
    }

    public void OnPlayer1ActionSwing(InputAction.CallbackContext context)
    {
        if (_player1 == null)
            _player1 = FindPlayer("Player1");

        if (context.started)
        {
            _player1.StartCharging(ESwingChargerState.ActionSwing);
            return;
        }

        if (context.canceled)
        {
            _player1.StopCharging(ESwingChargerState.ActionSwing);
            return;
        }
    }
    
    public void OnPlayer2ActionSwing(InputAction.CallbackContext context)
    {
        if (_player2 == null)
            _player2 = FindPlayer("Player2");

        if (context.started)
        {
            _player2.StartCharging(ESwingChargerState.ActionSwing);
            return;
        }


        if (context.canceled)
        {
            _player2.StopCharging(ESwingChargerState.ActionSwing);
            return;
        }
    }

    private void Move(Player player, Vector2 moveValue)
    {
        player.MoveValue = moveValue;
    }

    private void Jump(Player player, bool value)
    {
        player.JumpValue = value;
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

    private string GetBindingName(InputAction.CallbackContext context)
    {
        var action = context.action;
        return action.bindings[action.GetBindingIndexForControl(context.control)].name.ToLower();
    }

    private void Start()
    {
        _badmintonController =
            FindFirstObjectByType<BadmintonControllerComponent>()
            .GetComponent<BadmintonControllerComponent>()
            .Controller;
    }
}
