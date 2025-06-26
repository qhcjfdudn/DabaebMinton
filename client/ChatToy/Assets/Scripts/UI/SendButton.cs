using UnityEngine;

public class SendButton : MonoBehaviour
{
    public NetworkManager _networkManager;

    private void OnSubmit()
    {
        Debug.Log("OnSubmit - Send Button");
        _networkManager.SendPacketsToServer();
    }
}
