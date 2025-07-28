using UnityEngine;

public class TrainingController : MonoBehaviour
{
    public int maxSteps = 10_000;

    public Player _player1, _player2;
    public Shuttlecock _shuttlecock;

    public Vector2 _player1StartPosition = new Vector2(-3.5f, 2f);
    public Vector2 _player2StartPosition = new Vector2(3.5f, 2f);
    public Vector2 _shuttlecockStartPosition = new Vector2(-2f, 4f);

    private int _trainingTimer;

    


    private void ResetScene()
    {
        _trainingTimer = 0;

        _player1.transform.localPosition = _player1StartPosition;
        _player2.transform.localPosition = _player2StartPosition;
        _shuttlecock.transform.localPosition = _shuttlecockStartPosition;

    }

    private void Start()
    {


        ResetScene();
    }
}
