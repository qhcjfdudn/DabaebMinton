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

    public static uint ParseClassIdToUint(string s)
    {
        if (s.Length != 4)
            throw new ArgumentException("Must be exactly 4 chars");

        return ((uint)s[0] << 24) | ((uint)s[1] << 16) | ((uint)s[2] << 8) | s[3];
    }
}
