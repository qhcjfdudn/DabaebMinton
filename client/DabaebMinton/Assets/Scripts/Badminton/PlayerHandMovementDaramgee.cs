using UnityEngine;

public class PlayerHandMovementDaramgee : PlayerHandMovement
{
    public PlayerHandMovementDaramgee(PlayerHand owner)
    {
        _owner = owner;
    }

    public override void OnStandingLeft(int frameIdx)
    {
        switch (frameIdx)
        {
            case 0:
                _owner.transform.localPosition = new Vector3(0.132f, -0.171f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 70);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 1:
                _owner.transform.localPosition = new Vector3(0.132f, -0.171f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 75);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            default:
                Debug.LogWarning("알 수 없는 프레임 인덱스입니다.");
                break;
        }
    }

    public override void OnStandingRight(int frameIdx)
    {
        switch (frameIdx)
        {
            case 0:
                _owner.transform.localPosition = new Vector3(0.132f, -0.171f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 270);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 1:
                _owner.transform.localPosition = new Vector3(0.132f, -0.171f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 275);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            default:
                Debug.LogWarning("알 수 없는 프레임 인덱스입니다.");
                break;
        }
    }

    public override void OnMoveRight(int frameIdx)
    {
        switch (frameIdx)
        {
            case 0:
                _owner.transform.localPosition = new Vector3(0.239f, -0.146f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 290);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 1:
                _owner.transform.localPosition = new Vector3(0.283f, -0.095f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 310);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 2:
                _owner.transform.localPosition = new Vector3(0.166f, -0.095f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 280);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 3:
                _owner.transform.localPosition = new Vector3(-0.023f, -0.117f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 220);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 4:
                _owner.transform.localPosition = new Vector3(-0.179f, -0.173f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 160);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 5:
                _owner.transform.localPosition = new Vector3(-0.179f, -0.173f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 150);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 6:
                _owner.transform.localPosition = new Vector3(-0.023f, -0.117f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 220);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 7:
                _owner.transform.localPosition = new Vector3(0.166f, -0.095f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 280);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            default:
                Debug.LogWarning("알 수 없는 프레임 인덱스입니다.");
                break;
        }
    }

    public override void OnMoveLeft(int frameIdx)
    {
        switch (frameIdx)
        {
            case 0:
                _owner.transform.localPosition = new Vector3(0.215f, -0.161f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 95);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 1:
                _owner.transform.localPosition = new Vector3(0.215f, -0.161f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 110);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 2:
                _owner.transform.localPosition = new Vector3(0.122f, -0.146f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 90);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 3:
                _owner.transform.localPosition = new Vector3(-0.083f, -0.107f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 350);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 4:
                _owner.transform.localPosition = new Vector3(-0.225f, -0.171f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 340);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 5:
                _owner.transform.localPosition = new Vector3(-0.225f, -0.156f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 325);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 6:
                _owner.transform.localPosition = new Vector3(-0.083f, -0.117f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 350);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 7:
                _owner.transform.localPosition = new Vector3(0.108f, -0.141f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 80);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            default:
                Debug.LogWarning("알 수 없는 프레임 인덱스입니다.");
                break;
        }
    }

}
