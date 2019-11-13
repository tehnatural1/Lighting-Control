#include "common.h"
#include "common.c"
#include "GPIO.c"
#include "network/serv.c"
#include "network/agent.c"
#include "dictionary.c"


#define MSG_ALIVE_CHK   "WhoYouBe"
#define MSG_BTN_LAYOUT  "getButtons"
#define MSG_ALIVE_RESP  "butter"
#define MSG_ACK         "done"


#define RECV_PORT       9090
#define MAX_READ_SIZE   20


char * extract_data(char *msg, int len, char *action);

int send_all(int sd, void *buffer, size_t length);
int connection_handler(int sd);

void update_dictionary();


int main()
{
    setup_io();
    update_dictionary();
    

    button_layout = get_file_content("layout/layout.json", "rb");
    if (button_layout == NULL)
    {
        printf("    No layout content was generated.\n");
        exit(-1);
    }


    listen_for_connections(RECV_PORT, connection_handler);

    return(0);
}


void update_dictionary()
{
    addentry("XMAS", "192.168.1.76");
    addentry("192.168.1.76", "9090");
    addentry("TABLET", "192.168.1.72");
    addentry("192.168.1.72", "18000");
}


int connection_handler(int sd)
{
    char * msg = 0;
    int len;

    msg = malloc (MAX_READ_SIZE);

    recv (sd, msg, MAX_READ_SIZE, 0);
    len = strlen(msg);

    if (len == 0)
    {
        free (msg);
        return (-1);
    }


    printf("    Message recieved as: %s\n", msg);


    /**/ if (is_number (msg))
    {
        toggle_gpio (atoi (msg));
        send (sd, MSG_ACK, strlen(MSG_ACK), 0);
    }
    else if (strcmp (msg, MSG_ALIVE_CHK) == 0)
    {
        send (sd, MSG_ALIVE_RESP, strlen(MSG_ALIVE_RESP), 0);
    }
    else if (strcmp (msg, MSG_BTN_LAYOUT) == 0)
    {
        send_all(sd, button_layout, strlen(button_layout));
    }
    else
    {
        char action[MAX_READ_SIZE];
        char *ip = extract_data(msg, len, action);

        msg_to (ip, atoi(lookup(ip)), action);
        send (sd, MSG_ACK, strlen(MSG_ACK), 0);
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
        {
            printf("    Failed to send button layout: %d\n", i);
            return(i);
        }

        ptr += i;
        length -= i;
    }
    return(0);
}


char * extract_data(char *msg, int len, char *action)
{
    char host[len];
    int i;

    for (i = 0; i < len; i++)
    {
        host[i] = msg[i];
        if (msg[i] == '_') break;
    }

    host[i] = 0x00;
    i++;

    strncpy(action, msg+i, len-i);
    action[len-i] = 0x00;

    return lookup(host);
}


void connection_timeout(int sig)
{
   printf("   Connection with client timedout: %d\n", sig);
}