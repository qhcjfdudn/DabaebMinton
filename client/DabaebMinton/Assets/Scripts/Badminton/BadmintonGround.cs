using UnityEngine;

public class BadmintonGround : MonoBehaviour
{
    public EGroundType _groundType = EGroundType.None;
}

public enum EGroundType
{
    None,
    Left,
    Right,
    Size
}