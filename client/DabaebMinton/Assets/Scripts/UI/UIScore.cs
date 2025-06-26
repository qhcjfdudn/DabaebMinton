using TMPro;
using UnityEngine;

public class UIScore : MonoBehaviour
{
    public float Player1Score { 
        get { return _player1Score; }
        private set { _player1Score = value; }
    }
    public float Player2Score {
        get { return _player2Score; }
        private set { _player2Score = value; }
    }

    private float _player1Score, _player2Score;
    TMP_Text _player1ScoreText, _player2ScoreText;

    public void AddScore(int player)
    {
        if (player == 1)
        {
            _player1Score++;
            _player1ScoreText.text = _player1Score.ToString();
        }
        else if (player == 2)
        {
            _player2Score++;
            _player2ScoreText.text = _player2Score.ToString();
        }
    }

    private void Awake()
    {
        _player1Score = _player2Score = 0f;

        _player1ScoreText = transform.Find("Player1Score").GetComponent<TMP_Text>();
        _player2ScoreText = transform.Find("Player2Score").GetComponent<TMP_Text>();

        _player1ScoreText.text = _player1Score.ToString();
        _player2ScoreText.text = _player2Score.ToString();
    }
}
