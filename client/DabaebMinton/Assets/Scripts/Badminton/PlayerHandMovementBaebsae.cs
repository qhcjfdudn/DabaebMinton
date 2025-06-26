using UnityEngine;

public class PlayerHandMovementBaebsae : PlayerHandMovement
{
    public PlayerHandMovementBaebsae(PlayerHand owner)
    {
        _owner = owner;
    }

    public override void OnMoveLeft(int frameIdx)
    {
        switch (frameIdx)
        {
            case 0:
                _owner.transform.localPosition = new Vector3(-0.238f, -0.16f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, -20);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 1:
                _owner.transform.localPosition = new Vector3(-0.244f, -0.142f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, -25);
                _owner._racketSpriteRenderer.sortingOrder = 0;
                break;
            case 2:
                _owner.transform.localPosition = new Vector3(-0.125f, -0.172f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 10);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 3:
                _owner.transform.localPosition = new Vector3(0.136f, -0.172f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 90);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 4:
                _owner.transform.localPosition = new Vector3(0.225f, -0.166f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 90);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 5:
                _owner.transform.localPosition = new Vector3(0.237f, -0.16f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 95);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 6:
                _owner.transform.localPosition = new Vector3(0.136f, -0.172f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 90);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            case 7:
                _owner.transform.localPosition = new Vector3(-0.125f, -0.172f, 0);
                _owner.transform.localRotation = Quaternion.Euler(0, 0, 10);
                _owner._racketSpriteRenderer.sortingOrder = -1;
                break;
            default:
                Debug.LogWarning("알 수 없는 프레임 인덱스입니다.");
                break;
        }
    }
}
