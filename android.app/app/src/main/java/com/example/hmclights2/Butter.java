package com.example.hmclights2;

import android.os.AsyncTask;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;

import static com.example.hmclights2.Constants.SOCKET_CONNECTION_TIMEOUT;
import static com.example.hmclights2.Constants.SOCKET_RESPONSE_TIMEOUT;


class Butter extends AsyncTask<String, Void, String>
{
    AsyncResponse delegate = null;

    @Override
    protected void onPostExecute(String s)
    {
        if ((s != null) && !s.equals(""))
        {
            delegate.butter_complete(s);
        }
    }

    @Override
    protected String doInBackground(String... cmds)
    {
        for (String cmd : cmds)
        {
            try
            {
                StringBuilder sb = new StringBuilder();
                Socket s = new Socket();
                String line;

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

                while ((line = in.readLine()) != null)
                {
                    sb.append(line);
                }

                in.close();
                out.close();
                s.close();

                return sb.toString();

            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
        return null;
    }
}