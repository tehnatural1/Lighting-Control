package com.example.hmclights2;


public interface AsyncResponse
{
    void ipscan_complete(String output);
    void butter_complete(String s);
    String get_server_ip();
    int get_server_port();
}
