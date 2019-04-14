package com.example.hmclights2;

final class Constants {

    static final String EXPECTED_SERVER_RESPONSE             = "butter";

    // Default colors
    static final String DEFAULT_BUTTON_FOREGROUND_COLOR         = "#ebdbb2";
    static final String DEFAULT_BUTTON_BACKGROUND_COLOR         = "#458588";
    static final String DEFAULT_BACKGROUND_COLOR                = "#1d2021";
    static final String DEFAULT_SUCCESS_COLOR                   = "#8ec07c";
    static final String DEFAULT_FAILURE_COLOR                   = "#fb4934";

    // Default values
    static final boolean DEFAULT_BUTTON_OPTION_STATE         = true;
    static final boolean DEFAULT_TOAST_DISPLAY               = true;
    static final String DEFAULT_BUTTON_ERR_TEXT              = "ERROR";
    static final String DEFAULT_BUTTON_PADDING               = "0;0;0;0";
    static final String DEFAULT_FORM_PADDING                 = "50;0;50;0";
    static final int DEFAULT_BUTTON_SPACE_HEIGHT             = 20;
    static final int SOCKET_CONNECTION_TIMEOUT               = 2000;
    static final int DEFAULT_BUTTON_TEXT_SIZE                = 24;
    static final int ANIMATION_FADE_DURATION                 = 3000;
    static final int SOCKET_RESPONSE_TIMEOUT                 = 2000;
    static final int DEFAULT_BUTTON_HEIGHT                   = 180;
    static final int DEFAULT_PORT                            = 9090;

    // Layout identifiers
    static final String LAYOUT_TOP_SPACER_WEIGHT_ID  = "top_spacer_weight";
    static final String LAYOUT_BOT_SPACER_WEIGHT_ID  = "bot_spacer_weight";
    static final String LAYOUT_SUCCESS_COLOR_ID     = "success";
    static final String LAYOUT_FAILURE_COLOR_ID     = "failure";
    static final String LAYOUT_BACK_COLOR_ID      = "backcolor";
    static final String LAYOUT_BUTTONS_ID            = "buttons";
    static final String LAYOUT_PADDING_ID           = "padding";
    static final String LAYOUT_SPACE_HEIGHT_ID       = "spacer_height";
    static final String LAYOUT_OBJECT_ID             = "layout";

    // Button specific identifiers
    static final String BTN_DESCRIPTION_ID        = "description";
    static final String BTN_TEXT_COLOR_ID         = "forecolor";
    static final String BTN_TEXT_SIZE_ID          = "textsize";
    static final String BTN_BROADCAST_ID          = "broadcast";
    static final String BTN_PADDING_ID            = "padding";
    static final String BTN_HEIGHT_ID             = "height";
    static final String BTN_ENABLE_ID             = "enabled";
    static final String BTN_TOAST_ID              = "toast";
    static final String BTN_COLOR_ID              = "backcolor";
    static final String BTN_TEXT_ID               = "text";
    static final String BTN_GPIO_ID               = "id";

    // Status messages
    static final String MSG_OBTAINING_BUTTON_LAYOUT          = "Obtaining layout from server...";
    static final String MSG_SCANNING_FOR_CONNECTION          = "Scanning for Server...";
    static final String MSG_ESTABLISHED_CONNECTION           = "Server connection established.";
    static final String MSG_LAYOUT_LOAD_SUCCESS              = "Layout loaded successfully";
    static final String ERR_UNABLE_TO_CONNECT                = "ERROR: Lighting Server was not found.";
    static final String ERR_UNABLE_TO_LOAD_LAYOUT            = "ERROR: Server Connection";

    // Commands
    static final String GET_BUTTON_LAYOUT_COMMAND            = "getButtons\0";
    static final String VERIFY_LIGHTING_SERVER_COMMAND       = "WhoYouBe\0";

}
