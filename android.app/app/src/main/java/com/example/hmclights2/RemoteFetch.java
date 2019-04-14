package com.example.hmclights2;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;

import org.json.JSONObject;

import android.content.Context;


public class RemoteFetch {

    public static JSONObject getJSON(Context context, String city, String api_url)
    {
        try
        {
            System.err.println(city);

            URL url = new URL(
                    String.format(
                            api_url,
                            city,
                            context.getString(R.string.open_weather_maps_app_id)
                    )
            );

            url.openConnection();

            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(url.openStream())
            );

            StringBuffer sb = new StringBuffer(1024);
            String line;

            while((line = reader.readLine()) != null)
            {
                sb.append(line).append("\n");
            }

            reader.close();

            JSONObject data = new JSONObject(sb.toString());

            System.err.println(sb.toString());

            if(data.getInt("cod") != 200)
            {
                return null;
            }

            return data;
        }
        catch(Exception e)
        {
            e.printStackTrace();
            return null;
        }
    }
}