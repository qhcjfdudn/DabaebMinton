using UnityEngine;

public class TrainingController : BadmintonController
{
    public int maxSteps = 10_000;

    public Vector2 _player1StartPosition = new Vector2(-3.5f, 2f);
    public Vector2 _player2StartPosition = new Vector2(3.5f, 2f);
    public Vector2 _shuttlecockStartPosition = new Vector2(-2f, 4f);

    private int _trainingTimer;

    public override void Initialize()
    {
        base.Initialize();

        ResetScene();
    }

    public override void TouchGround(EGroundType groundType)
    {
        base.TouchGround(groundType);
        
        // reward: 1
    }

    public override void TouchPenaltyArea()
    {
        base.TouchPenaltyArea();
        
        // reward: -1
    }

    private void ResetScene()
    {
        _trainingTimer = 0;

        _player1.transform.localPosition = _player1StartPosition;
        _player2.transform.localPosition = _player2StartPosition;
        _shuttlecock.transform.localPosition = _shuttlecockStartPosition;

    }

    // Update가 없으면 timer를 쓸 수가 없네?
}
