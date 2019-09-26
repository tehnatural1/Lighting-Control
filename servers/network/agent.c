#include "connections.h"


int msg_to(char * ip, int port, char * msg)
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
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);


    tv.tv_sec = MAX_RECV_SND_TIMEOUT;
    tv.tv_usec = 0;


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