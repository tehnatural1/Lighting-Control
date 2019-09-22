#include "common.h"
#include "common.c"
#include "GPIO.h"
#include "GPIO.c"
#include "Connections.h"
#include "Connections.c"


int main()
{
    setup_io();

    read_button_layout();

    listen_for_connections();

    return(0);
}