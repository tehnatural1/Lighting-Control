package com.example.hmclights2;

import android.app.Activity;
import android.content.SharedPreferences;

public class ServerAddress
{
    SharedPreferences prefs;

    public ServerAddress(Activity activity)
    {
        prefs = activity.getPreferences(Activity.MODE_PRIVATE);
    }

    String getIP()
    {
        return prefs.getString("ip", "192.168.1.48");
    }

    int getPort()
    {
        return prefs.getInt("port", 9090);
    }

    void setIP(String ip, int port)
    {
        prefs.edit().putInt("port", port).apply();
        prefs.edit().putString("ip", ip).apply();
    }
}
