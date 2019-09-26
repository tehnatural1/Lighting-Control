#include "connections.h"


int listen_for_connections(int port, int (*connection_handler)())
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


    memset (&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons (port);


    rc = bind (sd, (struct sockaddr *)&addr, sizeof(addr));
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


    tv.tv_sec = MAX_RECV_SND_TIMEOUT;
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

            connection_handler(rc);

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