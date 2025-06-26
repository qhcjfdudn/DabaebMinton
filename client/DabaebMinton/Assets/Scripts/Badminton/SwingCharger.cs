using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class SwingCharger : MonoBehaviour
{
    public const float MIN_CHARGE_VALUE = 0.2f;
    public const float MAX_CHARGE_VALUE = 1.5f;

    public float min_charge_gauge_log_value, max_charge_gauge_log_value;
    public float min_ratio_value, max_ratio_value;

    [SerializeField]
    [Range(MIN_CHARGE_VALUE, MAX_CHARGE_VALUE)]
    private float _chargeGauge;
    private float _chargingTime;

    public float ChargeGauge
    {
        get { return _chargeGauge; }
        private set { _chargeGauge = value; }
    }

    private ESwingChargerState _currentState;
    private int selectedSwingStateBits;

    public void StartCharging(ESwingChargerState state)
    {
        selectedSwingStateBits |= (int)state;
        _currentState = state;

        ChargeGauge = MIN_CHARGE_VALUE;

        _chargingTime = 0;
    }

    public bool StopCharging(ESwingChargerState state)
    {
        selectedSwingStateBits -= selectedSwingStateBits & (int)state;

        if (_currentState != state)
            return false;

        _currentState = ESwingChargerState.Charged;

        return true;
    }

    public void Release()
    {
        ESwingChargerState nextState = GetNextState();

        if (nextState == ESwingChargerState.Idle)
        {
            _currentState = ESwingChargerState.Idle;
            ChargeGauge = MIN_CHARGE_VALUE;
            _chargingTime = 0;
        }
        else
            StartCharging(nextState);
    }

    private ESwingChargerState GetNextState()
    {
        if ((selectedSwingStateBits & (int)ESwingChargerState.Swing) > 0)
            return ESwingChargerState.Swing;

        if ((selectedSwingStateBits & (int)ESwingChargerState.ActionSwing) > 0)
            return ESwingChargerState.ActionSwing;

        return ESwingChargerState.Idle;
    }

    /// <summary>
    /// Logarithmic function to calculate charge gauge based on charging time.
    /// </summary>
    /// <param name="chargingTime"></param>
    /// <returns>The power of charge gauge</returns>
    private float GetChargeGauge(float chargingTime)
    {
        float chargingValue = Mathf.Log(min_charge_gauge_log_value + (max_charge_gauge_log_value - min_charge_gauge_log_value) * chargingTime);
        float chargingRatio = (chargingValue - min_ratio_value) / (max_ratio_value - min_ratio_value);

        return (MAX_CHARGE_VALUE - MIN_CHARGE_VALUE) * chargingRatio + MIN_CHARGE_VALUE;
    }

    private void Start()
    {
        ChargeGauge = MIN_CHARGE_VALUE;
        _currentState = ESwingChargerState.Idle;
        selectedSwingStateBits = 0;

        min_charge_gauge_log_value = Mathf.Exp(-1f);
        max_charge_gauge_log_value = Mathf.Exp(1f);
        min_ratio_value = Mathf.Log(min_charge_gauge_log_value);
        max_ratio_value = Mathf.Log(max_charge_gauge_log_value);
    }

    private void FixedUpdate()
    {
        if (_currentState == ESwingChargerState.Idle || _currentState == ESwingChargerState.Charged)
            return;

        if (ChargeGauge >= MAX_CHARGE_VALUE)
            return;

        _chargingTime = Mathf.Min(_chargingTime + Time.fixedDeltaTime, 1f);
        ChargeGauge = GetChargeGauge(_chargingTime);
    }
}

public enum ESwingChargerState
{
    Idle, Swing = 1, ActionSwing = 2, Charged = 4, ALL = 7
}
