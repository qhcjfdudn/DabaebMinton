using UnityEngine;
using UnityEngine.UI;
using TMPro;
using System;
using System.Text;

public class UIController : MonoBehaviour
{
    public Button _usernameSettingButton;
    public TMP_InputField _usernameInputField;
    public TMP_Text _usernameTextField;

    private NetworkManager _networkManager;

    private void Start()
    {
        _usernameSettingButton.onClick.AddListener(ToggleUsernameInputField);

        _usernameInputField.gameObject.SetActive(false);
        _usernameInputField.onEndEdit.AddListener(OnSubmitUsernameInputField);
        _usernameInputField.onDeselect.AddListener(_ => _usernameInputField.gameObject.SetActive(false));

        _networkManager = NetworkManager.Instance;
    }

    private void ToggleUsernameInputField()
    {
        if (_usernameInputField.gameObject.activeSelf)
            _usernameInputField.gameObject.SetActive(false);
        else
            _usernameInputField.gameObject.SetActive(true);
    }

    private void OnSubmitUsernameInputField(string username)
    {
        if (string.IsNullOrWhiteSpace(username))
            return;

        Debug.Log($"UsernameInputField: {username}");

        SetUsernameRPC(username);

        
    }

    public void SetUsernameRPC(string username)
    {
        OutputMemoryBitStream output = new OutputMemoryBitStream();

        // 2bits header, 8bits string length, length bits content
        output.WriteBits(2, 2);
        output.WriteBits(Encoding.UTF8.GetByteCount(username), 8);
        output.Write(username);

        Debug.Log($"output.Count: {output.Count}");
        _networkManager.OutBuffer.WriteBits(output.StreamBuffer, output.Count);

        // 편의상 발신해보자. 추후 NetworkManager가 자동으로 발신하는 코드로 변경
        //_networkManagerUI.SendPacketsToServer();
    }

    public void SetUsername(string username)
    {
        _usernameInputField.gameObject.SetActive(false);
        _usernameInputField.text = null;

        _usernameTextField.text = username;
    }

    public void OnClickToggleNetworkModeButton()
    {
        GameManager gameManager = FindFirstObjectByType<GameManager>().GetComponent<GameManager>();
        gameManager.TogglePlayMode();
    }
}