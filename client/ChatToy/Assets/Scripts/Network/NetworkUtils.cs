using System;

public static class NetworkUtils
{
    public static int GetRequiredBits(int num)
    {
        if (num < 0)
            return sizeof(int) << 3;

        int ret = 0;

        while (num >= (1 << ret))
            ++ret;

        return ret;
    }

    public static int GetRequiredBits<T>(T value) where T : Enum
    {
        return GetRequiredBits((int)(object)value);
    }
}
