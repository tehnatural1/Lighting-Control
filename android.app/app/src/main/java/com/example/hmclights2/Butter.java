package com.example.hmclights2;

import android.graphics.Color;
import android.os.AsyncTask;
import android.view.ContextThemeWrapper;
import android.widget.Button;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static com.example.hmclights2.Constants.BUTTON_BROADCAST_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_COLOR_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_DESCRIPTION_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_ENABLE_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_HEIGHT_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_PADDING_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_TEXT_COLOR_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_TEXT_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_TEXT_SIZE_IDENTIFIER;
import static com.example.hmclights2.Constants.BUTTON_TOAST_IDENTIFIER;
import static com.example.hmclights2.Constants.DEFAULT_BUTTON_BACKGROUND_COLOR;
import static com.example.hmclights2.Constants.DEFAULT_BUTTON_ERR_TEXT;
import static com.example.hmclights2.Constants.DEFAULT_BUTTON_FOREGROUND_COLOR;
import static com.example.hmclights2.Constants.DEFAULT_BUTTON_HEIGHT;
import static com.example.hmclights2.Constants.DEFAULT_BUTTON_OPTION_STATE;
import static com.example.hmclights2.Constants.DEFAULT_BUTTON_PADDING;
import static com.example.hmclights2.Constants.DEFAULT_BUTTON_TEXT_SIZE;
import static com.example.hmclights2.Constants.DEFAULT_TOAST_DISPLAY;
import static com.example.hmclights2.Constants.LAYOUT_BUTTONS_IDENTIFIER;
import static com.example.hmclights2.Constants.LAYOUT_OBJECT_IDENTIFIER;
import static com.example.hmclights2.Constants.SOCKET_CONNECTION_TIMEOUT;
import static com.example.hmclights2.Constants.SOCKET_RESPONSE_TIMEOUT;


class Butter extends AsyncTask<String, Void, Void>
{
    private List<Button> btns = new ArrayList<>();
    private JSONObject layout_options = null;
    AsyncResponse delegate = null;

    protected void onPostExecute(Void v)
    {
        if (!delegate.get_layout_set())
        {
            delegate.set_layout_set(true);
            delegate.butter_complete(btns, layout_options);
        }
    }

    protected Void doInBackground(String... cmds)
    {
        for (String cmd : cmds)
        {
            try
            {
                StringBuilder sb = new StringBuilder();
                Socket s = new Socket();

                s.connect(
                        new InetSocketAddress(
                                delegate.get_server_ip(),
                                delegate.get_server_port()
                        ),
                        SOCKET_CONNECTION_TIMEOUT
                );

                s.setSoTimeout(SOCKET_RESPONSE_TIMEOUT);

                PrintWriter out = new PrintWriter(
                        s.getOutputStream(),
                        true
                );

                BufferedReader in = new BufferedReader(
                        new InputStreamReader(s.getInputStream())
                );

                out.write(cmd);
                out.flush();

                String line;
                while ((line = in.readLine()) != null)
                {
                    sb.append(line);
                }

                in.close();
                out.close();
                s.close();

                String resp = sb.toString();

                if (!resp.equals(""))
                {
                    JSONObject data = new JSONObject(resp);
                    JSONArray values = data.getJSONArray(LAYOUT_BUTTONS_IDENTIFIER);

                    layout_options = data.getJSONObject(LAYOUT_OBJECT_IDENTIFIER);

                    for (int i = 0; i < values.length(); i++)
                    {
                        JSONObject btn = values.getJSONObject(i);
                        Button tmp = createButton(btn);

                        if (tmp != null) btns.add(tmp);
                    }
                }
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
        return null;
    }

    private int extractInt(JSONObject j, String s, int d)
    {
        try
        {
            if (j.getInt(s) > 0) return j.getInt(s);
        }
        catch (Exception e)
        {}
        return d;
    }

    private String extractStr(JSONObject j, String s, String d)
    {
        try
        {
            return j.getString(s);
        }
        catch (Exception e)
        {
            return d;
        }
    }

    private Boolean extractBl(JSONObject j, String s, Boolean d)
    {
        try
        {
            return j.getBoolean(s);
        }
        catch (Exception e)
        {
            return d;
        }
    }

    private int extractClr(JSONObject j, String s, int d)
    {
        try
        {
            return Color.parseColor(j.getString(s));
        }
        catch (Exception e)
        {
            return d;
        }
    }

    private Button createButton(JSONObject cmd)
    {
        // Declared as final as these variables will be used in the onclick listener
        final String text = extractStr(
                cmd,
                BUTTON_TEXT_IDENTIFIER,
                DEFAULT_BUTTON_ERR_TEXT
        ) + '\0';

        final String desc = extractStr(
                cmd,
                BUTTON_DESCRIPTION_IDENTIFIER,
                DEFAULT_BUTTON_ERR_TEXT
        ) + '\0';

        final Boolean toast = extractBl(
                cmd,
                BUTTON_TOAST_IDENTIFIER,
                DEFAULT_TOAST_DISPLAY
        );

        final String[] padding = (extractStr(
                cmd,
                BUTTON_PADDING_IDENTIFIER,
                DEFAULT_BUTTON_PADDING)
        ).split(";");

        final int f_color = extractClr(
                cmd,
                BUTTON_TEXT_COLOR_IDENTIFIER,
                DEFAULT_BUTTON_BACKGROUND_COLOR
        );

        final  int b_color = extractClr(
                cmd,
                BUTTON_COLOR_IDENTIFIER,
                DEFAULT_BUTTON_FOREGROUND_COLOR
        );

        final int text_size = extractInt(
                cmd,
                BUTTON_TEXT_SIZE_IDENTIFIER,
                DEFAULT_BUTTON_TEXT_SIZE
        );

        final int height = extractInt(
                cmd,
                BUTTON_HEIGHT_IDENTIFIER,
                DEFAULT_BUTTON_HEIGHT
        );

        int buttonStyle = R.style.ButtonAppTheme;

        boolean broadcast = extractBl(
                cmd,
                BUTTON_BROADCAST_IDENTIFIER,
                DEFAULT_BUTTON_OPTION_STATE
        );

        boolean enabled = extractBl(
                cmd,
                BUTTON_ENABLE_IDENTIFIER,
                DEFAULT_BUTTON_OPTION_STATE
        );

        if (!enabled) return null;

        Button btn = new Button(
                new ContextThemeWrapper(
                        delegate.get_context(),
                        buttonStyle
                ),
                null,
                buttonStyle
        );

        btn.setHeight(height);
        btn.setText(text);
        btn.setTextSize(text_size);
        btn.setScaleX(0f);
        btn.setScaleY(0f);

        if (f_color != DEFAULT_BUTTON_BACKGROUND_COLOR)
        {
            btn.setTextColor(f_color);
        }
        if (b_color != DEFAULT_BUTTON_FOREGROUND_COLOR)
        {
            btn.setBackgroundColor(b_color);
        }

        if (!Arrays.stream(padding).allMatch("0"::equals))
        {
            btn.setPadding(
                    Integer.parseInt(padding[0]),
                    Integer.parseInt(padding[1]),
                    Integer.parseInt(padding[2]),
                    Integer.parseInt(padding[3])
            );
        }

        btn.setContentDescription(desc);
        btn.setTag(toast);

        return btn;
    }
}