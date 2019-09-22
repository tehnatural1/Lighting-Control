#include "Connections.h"
#include "common.h"


int listen_for_connections()
{
    struct sockaddr_in addr;
    struct timeval tv;
    int sd, rc;
    

    sd = socket (AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("    socket failed");
        return(-1);
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons (MESSAGE_PORT);


    rc = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc != 0)
    {
        perror("    bind failed");
        close(sd);
        return(-2);
    }


    rc = listen(sd, MAX_CONNECTION_REQUESTS);
    if (rc != 0)
    {
        perror("    listen failed");
        close(sd);
        return(-3);
    }


    tv.tv_sec = MAX_RECV_CLIENT_TIMEOUT;
    tv.tv_usec = 0;


    for(;;)
    {
        errno = 0;
        printf("\n   Wait for an incoming connection to arrive\n");


        rc = accept(sd, NULL, NULL);
        printf("   accept() completed.  rc = %d, errno = %d\n", rc, errno);
        if (rc >= 0)
        {
            printf("   Incoming connection was received\n");

            setsockopt (
                rc, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof tv
            );
            setsockopt (
                rc, SOL_SOCKET, SO_SNDTIMEO, (const char *) &tv, sizeof tv
            );

            handle_connection(rc);

            close(rc);
        }
        else
        {
            perror("   errno string");
        }

    }


    close(sd);
    return(0);
}


int handle_connection(int sd)
{
    char * msg = 0;

    msg = malloc (MAX_READ_SIZE);

    recv (sd, msg, MAX_READ_SIZE, 0);


    printf("    Message recieved as: %s", msg);


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
    char *ptr = (char*) buffer;


    while (length > 0)
    {
        int i = send (sd, ptr, length, 0);
        if (i < 1) return(-1);
        ptr += i;
        length -= i;
    }
    return(0);
}


void connection_timeout(int sig)
{
   printf("   Connection with client timedout: %d\n", sig);
}


int msg_to(char * ip, int port, char * msg)
{
    struct sockaddr_in addr;
    struct timeval tv;
    int sd, rc;


    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("    socket failed");
        return(-1);
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);


    tv.tv_sec = 0;
    tv.tv_usec = 200000;


    setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof tv);
    setsockopt (sd, SOL_SOCKET, SO_SNDTIMEO, (const char *) &tv, sizeof tv);


    rc = connect (sd, (struct sockaddr *)&addr, sizeof addr);
    if (rc < 0)
    {
        perror("    connect failed");
        close(sd);
        return(-2);
    }


    rc = send (sd, msg, strlen (msg), 0);
    if (rc < 0)
    {
        perror("    send failed");
        close(sd);
        return(-3);
    }


    close(sd);
    return(0);
}