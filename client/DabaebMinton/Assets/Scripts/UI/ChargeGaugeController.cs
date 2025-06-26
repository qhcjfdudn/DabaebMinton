using UnityEngine;
using UnityEngine.UI;

public class ChargeGaugeController : MonoBehaviour
{
    [Header("UI Components")]
    [SerializeField] private Image _gaugeFill;

    [Header("Power Settings")]
    [SerializeField] private float _minPower;
    [SerializeField] private float _maxPower;

    private float _currentPower = 0f;

    private SwingCharger _swingCharger;

    // 외부에서 전투력을 설정할 수 있게 메서드 제공
    public void SetPower(float value)
    {
        _currentPower = Mathf.Clamp(value, _minPower, _maxPower);
        UpdateGauge();
    }

    private void UpdateGauge()
    {
        float t = Mathf.Clamp01((_currentPower - _minPower) / (_maxPower - _minPower));
        _gaugeFill.fillAmount = t;
    }

    private void Awake()
    {
        _swingCharger = transform.parent.GetComponent<SwingCharger>();
    }

    private void Start()
    {
        _minPower = SwingCharger.MIN_CHARGE_VALUE;
        _maxPower = SwingCharger.MAX_CHARGE_VALUE;
    }

    private void Update()
    {
        _currentPower = _swingCharger.ChargeGauge;
        UpdateGauge();
    }
}
