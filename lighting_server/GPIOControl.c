#include "common.h"
#include "common.c"
#include "GPIO.h"
#include "GPIO.c"
#include "Connections.h"
#include "Connections.c"


int main()
{
    setup_io();


    button_layout = get_file_content("buttons.json", "rb");
    if (button_layout == NULL)
    {
        printf("    No layout content was generated.\n");
        exit(-1);
    }


    listen_for_connections();

    return(0);
}