using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.InputSystem;

public class LocalPlayModeInputManager : MonoBehaviour
{
    private GameManager _gameManager;
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
            Jump(_player1);

    }

    public void OnPlayer2Jump(InputAction.CallbackContext context)
    {
        if (_player2 == null)
            _player2 = FindPlayer("Player2");

        if (context.started)
            return;

        if (context.performed)
            Jump(_player2);
    }

    public void OnPlayer1Swing(InputAction.CallbackContext context)
    {
        if (_player1 == null)
            _player1 = FindPlayer("Player1");

        SwingCharger charger = _player1.GetComponentInChildren<SwingCharger>();

        if (context.started)
        {
            charger.StartCharging(ESwingChargerState.Swing);
            return;
        }

        // charge 세기에 따라 때린다.
        if (context.canceled)
        {
            if (charger.StopCharging(ESwingChargerState.Swing))
            {
                _gameManager.SwingShuttlecock(_player1);
                charger.Release();
            }

            return;
        }
    }

    public void OnPlayer2Swing(InputAction.CallbackContext context)
    {
        if (_player2 == null)
            _player2 = FindPlayer("Player2");

        SwingCharger charger = _player2.GetComponentInChildren<SwingCharger>();

        if (context.started)
        {
            charger.StartCharging(ESwingChargerState.Swing);
            return;
        }

        if (context.canceled)
        {
            if (charger.StopCharging(ESwingChargerState.Swing))
            {
                _gameManager.SwingShuttlecock(_player2);
                charger.Release();
            }

            return;
        }
    }

    public void OnPlayer1ActionSwing(InputAction.CallbackContext context)
    {
        if (_player1 == null)
            _player1 = FindPlayer("Player1");

        SwingCharger charger = _player1.GetComponentInChildren<SwingCharger>();

        if (context.started)
        {
            charger.StartCharging(ESwingChargerState.ActionSwing);
            return;
        }

        if (context.canceled)
        {
            if (charger.StopCharging(ESwingChargerState.ActionSwing))
            {
                _gameManager.ActionSwingShuttlecock(_player1);
                charger.Release();
            }

            return;
        }
    }
    
    public void OnPlayer2ActionSwing(InputAction.CallbackContext context)
    {
        if (_player2 == null)
            _player2 = FindPlayer("Player2");

        SwingCharger charger = _player2.GetComponentInChildren<SwingCharger>();

        if (context.started)
        {
            charger.StartCharging(ESwingChargerState.ActionSwing);
            return;
        }


        if (context.canceled)
        {
            if (charger.StopCharging(ESwingChargerState.ActionSwing))
            {
                _gameManager.ActionSwingShuttlecock(_player2);
                charger.Release();
            }

            return;
        }
    }

    public void OnInitShuttlecock(InputAction.CallbackContext context)
    {
        if (context.started)
            return;

        if (context.performed)
        {
            _gameManager.StartNewGame();
            _gameManager.ChangeShuttlecockMovementStrategy();
            return;
        }

        if (context.canceled)
            return;
    }

    private void Move(Player player, Vector2 moveValue)
    {
        player.MoveValue = moveValue;
    }

    private void Jump(Player player)
    {
        player.JumpValue = true;
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
        _gameManager = 
            FindFirstObjectByType<GameManager>()
            .GetComponent<GameManager>();
    }
}
