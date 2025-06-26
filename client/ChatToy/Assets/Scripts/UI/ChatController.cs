using TMPro;
using UnityEngine;
using System.Text;

public class ChatController : MonoBehaviour
{
    public TMP_InputField _inputField;
    public GameObject _messagePrefab;
    public Transform _content;

    NetworkManager _networkManager;

    private void Start()
    {
        _networkManager = NetworkManager.Instance;
    }

    void Update()
    {
        if (Input.GetKey(KeyCode.Return))
        {
            string userInput = GetUserInputFromUI();
            
            AddNewChatRPC(userInput);

            _inputField.text = string.Empty;
        }
    }

    private string GetUserInputFromUI()
    {
        string userInput = _inputField.text;

        if (string.IsNullOrWhiteSpace(userInput))
            return null;

        return userInput;
    }

    private void AddNewChatRPC(string userInput)
    {
        if (userInput == null)
            return;

        OutputMemoryBitStream output = new OutputMemoryBitStream();
        output.WriteBits(1, 2);
        output.WriteBits(Encoding.UTF8.GetByteCount(userInput), 8);
        output.Write(userInput);

        _networkManager.OutBuffer.WriteBits(output.StreamBuffer, output.Count);

        //_networkManagerUI.SendPacketsToServer();
    }

    public void AddNewChat(string text)
    {
        GameObject newMessage = Instantiate(_messagePrefab, _content);
        TMP_Text messageText = newMessage.GetComponent<TMP_Text>();
        messageText.text = text;
    }
}
