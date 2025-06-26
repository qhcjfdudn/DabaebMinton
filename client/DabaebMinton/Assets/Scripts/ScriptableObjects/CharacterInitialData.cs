using UnityEngine;

[CreateAssetMenu(fileName = "CharacterInitialData", menuName = "Scriptable Objects/InitialData/CharacterInitialData")]
public class CharacterInitialData : ScriptableObject
{
    public ECharacterID characterID;
    public float moveVelocity;
    public float jumpVelocity;
    public float power;
}
