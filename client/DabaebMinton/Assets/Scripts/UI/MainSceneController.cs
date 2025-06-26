using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class MainSceneController : MonoBehaviour
{
    public Button _localPlayButton;
    public Button _onlinePlayButton;
    public Button _settingsButton;
    public Button _quitButton;

    public void onClickLocalPlayButton()
    {
        SceneManager.LoadScene("PlayBadmintonScene");
    }

    public void onClickOnlinePlayButton()
    {
        Debug.Log("Online Play button clicked");
        // Add logic to start online play
    }

    public void onClickSettingsButton()
    {
        Debug.Log("Settings button clicked");
        // Add logic to open settings
    }

    public void onClickQuitButton()
    {
        Debug.Log("Quit button clicked");
        // Add logic to quit the game
        Application.Quit();
    }

    private void Start()
    {
        _onlinePlayButton.interactable = false; // 동작 미구현
        _settingsButton.interactable = false; // 동작 미구현
    }
}
