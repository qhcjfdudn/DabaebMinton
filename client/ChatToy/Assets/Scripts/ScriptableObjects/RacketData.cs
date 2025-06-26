using UnityEngine;

[CreateAssetMenu(fileName = "RacketData", menuName = "Scriptable Objects/Badminton/RacketData")]
public class RacketData : ScriptableObject
{
    public string racketName;
    public float weight;            // 낮을수록 가볍다
    public float height;            // 라켓 길이
    public float repulsionPower;    // 반발력
    public Sprite icon;             // UI용 아이콘
    public Sprite sprite;           // Game Image

    // 필요한 속성 더 추가 가능
}
