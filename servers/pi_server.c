#include "common.h"
#include "common.c"
#include "GPIO.c"
#include "network/serv.c"
#include "network/agent.c"


#define MSG_TABLET      "Tablet Input"
#define MSG_ALIVE_CHK   "WhoYouBe"
#define MSG_BTN_LAYOUT  "getButtons"
#define MSG_ALIVE_RESP  "butter"
#define MSG_ACK         "done"


#define TABLET_IP       "192.168.1.72"
#define TABLET_PORT     18000
#define TABLET_TOGGLE   "butter"


#define RECV_PORT       9090
#define MAX_READ_SIZE   20


int send_all(int sd, void *buffer, size_t length);
int connection_handler(int sd);


int main()
{
    setup_io();


    button_layout = get_file_content("layout/layout.json", "rb");
    if (button_layout == NULL)
    {
        printf("    No layout content was generated.\n");
        exit(-1);
    }


    listen_for_connections(RECV_PORT, connection_handler);

    return(0);
}


int connection_handler(int sd)
{
    char * msg = 0;

    msg = malloc (MAX_READ_SIZE);

    recv (sd, msg, MAX_READ_SIZE, 0);


    printf("    Message recieved as: %s\n", msg);


    /**/ if (is_number (msg))
    {
        toggle_gpio (atoi (msg));
        send (sd, MSG_ACK, strlen(MSG_ACK), 0);
    }
    else if (strcmp (msg, MSG_TABLET) == 0)
    {
        msg_to (TABLET_IP, TABLET_PORT, TABLET_TOGGLE);
        send (sd, MSG_ACK, strlen(MSG_ACK), 0);
    }
    else if (strcmp (msg, MSG_ALIVE_CHK) == 0)
    {
        send (sd, MSG_ALIVE_RESP, strlen(MSG_ALIVE_RESP), 0);
    }
    else if (strcmp (msg, MSG_BTN_LAYOUT) == 0)
    {
        int rc = send_all(sd, button_layout, strlen(button_layout));


        if (rc < 0)
        {
            printf("    Unable to send button layout: %d\n", rc);
        }
    }

    free (msg);

    return(1);
}


int send_all(int sd, void *buffer, size_t length)
{
    char * ptr;
    int i;

    
    ptr = (char*) buffer;
    while (length > 0)
    {
        i = send (sd, ptr, length, 0);
        
        if (i < 1)
            return(-1);

        ptr += i;
        length -= i;
    }
    return(0);
}


void connection_timeout(int sig)
{
   printf("   Connection with client timedout: %d\n", sig);
}