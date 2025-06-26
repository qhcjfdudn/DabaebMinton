using UnityEngine;

public class ChargeGaugeCanvas : MonoBehaviour
{
    private void LateUpdate()
    {
        transform.forward = Camera.main.transform.forward;
    }
}
