using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

using TMPro;

public class MainMenuSceneController : MonoBehaviour
{
    public Button _localPlayButton;
    public Button _onlinePlayButton;
    public Button _settingsButton;
    public Button _quitButton;

    public TMP_Text _scoreText;
    public int _score;

    public Button[] _difficultyButtons;
    private EShuttlecockSpeed _selectedDifficulty;

    public Button _gameStartButton;

    public RectTransform _mainMenuPanel;
    public RectTransform _levelSettingsPanel;

    public void onClickLocalPlayButton()
    {
        _mainMenuPanel.gameObject.SetActive(false);
        _levelSettingsPanel.gameObject.SetActive(true);
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

    public void OnClickAddScoreButton(int value)
    {
        int nextScore = _score + value;

        if (nextScore < 5)
        {
            Debug.LogWarning("Score cannot be less than 5. Current score: " + _score);
            return;
        }

        if (nextScore > 25)
        {
            Debug.LogWarning("Score cannot exceed 25. Current score: " + _score);
            return;
        }

        _score += value;
        _scoreText.text = _score.ToString();
        Debug.Log("Score updated: " + _score);
    }

    public void OnSelectDifficultyButton(int difficultyType)
    {
        for (int i = 0; i < _difficultyButtons.Length; ++i)
        {
            _difficultyButtons[i].interactable = true; // 다른 난이도 버튼 활성화
        }

        _selectedDifficulty = (EShuttlecockSpeed)difficultyType; // 선택한 난이도 저장
        _difficultyButtons[difficultyType].interactable = false; // 선택한 난이도 버튼 비활성화
    }

    public void OnClickGameStartButton()
    {
        Debug.Log("Game Start button clicked");

        PlayerPrefs.SetInt("score", _score);
        PlayerPrefs.SetInt("difficulty", (int)_selectedDifficulty);

        SceneManager.LoadScene("PlayBadmintonScene");
    }

    private void Start()
    {
        _onlinePlayButton.interactable = false; // 동작 미구현
        _settingsButton.interactable = false; // 동작 미구현

        _score = 21;
        _scoreText.text = _score.ToString();

        _selectedDifficulty = EShuttlecockSpeed.Normal; // 기본 난이도 설정
        _difficultyButtons[(int)_selectedDifficulty].interactable = false;

        _mainMenuPanel.gameObject.SetActive(true);
        _levelSettingsPanel.gameObject.SetActive(false);
    }
}

public enum EPanelState
{
    None = 0,
    MainMenu = 1,
    Settings = 2,
    OnlinePlay = 3,
    LocalPlay = 4
}

public enum EShuttlecockSpeed
{
    Slow = 0,
    Normal = 1,
    Fast = 2
}