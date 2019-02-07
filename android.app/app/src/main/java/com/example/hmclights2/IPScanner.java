package com.example.hmclights2;

import android.os.AsyncTask;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.net.SocketException;
import java.util.Enumeration;

import static com.example.hmclights2.Constants.DEFAULT_PORT;
import static com.example.hmclights2.Constants.EXPECTED_SERVER_RESPONSE;
import static com.example.hmclights2.Constants.VERIFY_LIGHTING_SERVER_COMMAND;


public class IPScanner extends AsyncTask<Void, Void, String>
{
    private final int DEFAULT_SOCKET_TIMEOUT = 50;
    public AsyncResponse delegate = null;

    private String get_ip_address() throws SocketException
    {
        NetworkInterface ni = NetworkInterface.getByName("wlan0");
        Enumeration<InetAddress> inetAddresses = ni.getInetAddresses();

        while (inetAddresses.hasMoreElements())
        {
            InetAddress ia = inetAddresses.nextElement();
            if (!ia.isLinkLocalAddress() && ia.getHostAddress().split("\\.").length == 4)
            {
                return ia.getHostAddress();
            }
        }

        return null;
    }

    protected String doInBackground(Void... arg0)
    {
        String ip = null;

        try
        {
            ip = get_ip_address();
        }
        catch (SocketException e)
        {
            e.printStackTrace();
            System.err.println("Unable to access host through Network Interface.");
        }

        if ((ip == null) || ip.equals("127.0.0.1")) return null;

        String n_ip = ip.substring(0, ip.lastIndexOf('.') + 1);

        for (int i = 1; i <= 254; i++)
        {
            try
            {
                InetAddress addr = InetAddress.getByName(n_ip + Integer.toString(i));

                if (addr.isReachable(DEFAULT_SOCKET_TIMEOUT))
                {
                    if (is_connection(addr.getHostAddress(), DEFAULT_PORT))
                    {
                        return addr.getHostAddress();
                    }
                }
            }
            catch (IOException e)
            {
                e.printStackTrace();
                System.err.println("Unable to access stdin/stdout PIPE.");
            }
        }

        return null;
    }

    protected void onPostExecute(String result)
    {
        delegate.ipscan_complete(result);
    }

    private boolean is_connection(String ip, int port)
    {
        boolean is_lighting = false;

        try
        {
            Socket s = new Socket();
            s.connect(
                    new InetSocketAddress(ip, port),
                    DEFAULT_SOCKET_TIMEOUT
            );
            s.setSoTimeout(DEFAULT_SOCKET_TIMEOUT);

            PrintWriter out = new PrintWriter(
                    s.getOutputStream(),
                    true
            );
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(s.getInputStream())
            );

            out.write(VERIFY_LIGHTING_SERVER_COMMAND);
            out.flush();

            String line;
            while ((line = in.readLine()) != null)
            {
                if (line.equals(EXPECTED_SERVER_RESPONSE))
                {
                    is_lighting = true;
                }
            }

            in.close();
            s.close();
        }
        catch (IOException e)
        {
            e.printStackTrace();
            System.err.println("Unable to access stdin/stdout PIPE.");
        }

        return is_lighting;
    }
}
