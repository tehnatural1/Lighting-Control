package com.example.hmclights2;

import android.graphics.Color;

final class Constants {

    static final String EXPECTED_SERVER_RESPONSE             = "butter";

    // Default colors
    static final int DEFAULT_BUTTON_FOREGROUND_COLOR         = Color.parseColor("#ebdbb2");
    static final int DEFAULT_BUTTON_BACKGROUND_COLOR         = Color.parseColor("#458588");
    static final int DEFAULT_BACKGROUND_COLOR                = Color.parseColor("#1d2021");
    static final int DEFAULT_SUCCESS_COLOR                   = Color.parseColor("#8ec07c");
    static final int DEFAULT_FAILURE_COLOR                   = Color.parseColor("#fb4934");

    // Default values
    static final boolean DEFAULT_BUTTON_OPTION_STATE         = true;
    static final boolean DEFAULT_TOAST_DISPLAY               = true;
    static final String DEFAULT_BUTTON_ERR_TEXT              = "ERROR";
    static final String DEFAULT_BUTTON_PADDING               = "0;0;0;0";
    static final String DEFAULT_FORM_PADDING                 = "50;0;50;0";
    static final int DEFAULT_BUTTON_SPACE_HEIGHT             = 20;
    static final int SOCKET_CONNECTION_TIMEOUT               = 500;
    static final int DEFAULT_BUTTON_TEXT_SIZE                = 24;
    static final int ANIMATION_FADE_DURATION                 = 3000;
    static final int SOCKET_RESPONSE_TIMEOUT                 = 1000;
    static final int DEFAULT_BUTTON_HEIGHT                   = 180;
    static final int DEFAULT_PORT                            = 9090;

    // Layout identifiers
    static final String LAYOUT_TOP_SPACER_WEIGHT_IDENTIFIER  = "top_spacer_weight";
    static final String LAYOUT_BOT_SPACER_WEIGHT_IDENTIFIER  = "bot_spacer_weight";
    static final String LAYOUT_SUCCESS_COLOR_IDENTIFIER      = "success";
    static final String LAYOUT_FAILURE_COLOR_IDENTIFIER      = "failure";
    static final String LAYOUT_BACK_COLOR_IDENTIFIER         = "backcolor";
    static final String LAYOUT_BUTTONS_IDENTIFIER            = "buttons";
    static final String LAYOUT_PADDING_IDENTIFIER            = "padding";
    static final String LAYOUT_SPACE_HEIGHT_IDENTIFIER       = "spacer_height";
    static final String LAYOUT_OBJECT_IDENTIFIER             = "layout";

    // Button specific identifiers
    static final String BUTTON_DESCRIPTION_IDENTIFIER        = "description";
    static final String BUTTON_TEXT_COLOR_IDENTIFIER         = "forecolor";
    static final String BUTTON_TEXT_SIZE_IDENTIFIER          = "textsize";
    static final String BUTTON_BROADCAST_IDENTIFIER          = "broadcast";
    static final String BUTTON_PADDING_IDENTIFIER            = "padding";
    static final String BUTTON_HEIGHT_IDENTIFIER             = "height";
    static final String BUTTON_ENABLE_IDENTIFIER             = "enabled";
    static final String BUTTON_TOAST_IDENTIFIER              = "toast";
    static final String BUTTON_COLOR_IDENTIFIER              = "backcolor";
    static final String BUTTON_TEXT_IDENTIFIER               = "text";

    // Status messages
    static final String MSG_OBTAINING_BUTTON_LAYOUT          = "Obtaining layout from server...";
    static final String MSG_SCANNING_FOR_CONNECTION          = "Scanning for Server...";
    static final String MSG_ESTABLISHED_CONNECTION           = "Server connection established.";
    static final String MSG_LAYOUT_LOAD_SUCCESS              = "Layout loaded successfully";
    static final String ERR_UNABLE_TO_CONNECT                = "ERROR: Lighting Server was not found.";
    static final String ERR_UNABLE_TO_LOAD_LAYOUT            = "ERROR: Layout could not be loaded from server.";

    // Commands
    static final String GET_BUTTON_LAYOUT_COMMAND            = "getButtons\0";
    static final String VERIFY_LIGHTING_SERVER_COMMAND       = "WhoYouBe\0";

    // Device storage file information
    static final String DEVICE_STORAGE_FILE                  = "server_address";
    static final String DEVICE_STORAGE_IP_IDENTIFIER         = "ip";
    static final String DEVICE_STORAGE_PORT_IDENTIFIER       = "port";

}
