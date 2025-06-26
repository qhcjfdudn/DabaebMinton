using Newtonsoft.Json;
using System;
using System.Linq;
using UnityEngine;

using static NetworkUtils;

public class ScrptTestToyObject : MonoBehaviour
{
    void Start()
    {
        int[] numbers = { 3, 2, 5, 4, 1, 6, 7 };

        var ret = numbers.Where(x => x <= 3);   // method syntax
        var ret2 = from x in numbers            // query syntax
                   where x <= 3
                   select x;

        numbers[0] = 0;

        foreach (int num in ret)
            Debug.Log(num);

        Debug.Log($"sizeof(uint) << 3: {sizeof(uint) << 3}");

        Debug.Log("GetRequiredBits Test");
        Debug.Log($"GetRequiredBits(0) Expected: 0, Actual: {GetRequiredBits(0)}");
        Debug.Log($"GetRequiredBits(1) Expected: 1, Actual: {GetRequiredBits(1)}");
        Debug.Log($"GetRequiredBits(2) Expected: 2, Actual: {GetRequiredBits(2)}");
        Debug.Log($"GetRequiredBits(3) Expected: 2, Actual: {GetRequiredBits(3)}");
        Debug.Log($"GetRequiredBits(4) Expected: 3, Actual: {GetRequiredBits(4)}");
        Debug.Log("GetRequiredBits Test End");



        Debug.Log($"°¨¼èÀ²: {Mathf.Exp(-1f * Time.fixedDeltaTime)}");
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Q))
        {
            Debug.Log("A key pressed");

            Destroy(this);
        }
    }

    private void OnDisable()
    {
        Debug.Log("OnDisable");
    }

    private void OnDestroy()
    {
        Debug.Log("OnDestroy");
    }
}
