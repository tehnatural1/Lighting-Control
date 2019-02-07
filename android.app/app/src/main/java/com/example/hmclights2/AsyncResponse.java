package com.example.hmclights2;

import android.content.Context;
import android.widget.Button;

import org.json.JSONObject;

import java.util.List;

public interface AsyncResponse
{
    void ipscan_complete(String output);
    void butter_complete(List<Button> btns, JSONObject layout_options);
    void set_layout_set(boolean b);

    Context get_context();
    String get_server_ip();
    int get_server_port();

    boolean get_layout_set();
}
