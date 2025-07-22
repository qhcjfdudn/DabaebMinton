using UnityEngine;

public class BadmintonPlayUIController : MonoBehaviour
{
    public RectTransform _showingPlayResultPanel;
    public TMPro.TMP_Text _winPlayerText;
    public UIScore _uiScore;

    public void SetScore(int playerIndex, int score)
    {
        _uiScore.SetScore(playerIndex, score);
    }

    public void ShowWinnerText(string winner)
    {
        _winPlayerText.text = winner;
        _showingPlayResultPanel.gameObject.SetActive(true);
    }

    private void Start()
    {
        _showingPlayResultPanel.gameObject.SetActive(false);
    }
}
