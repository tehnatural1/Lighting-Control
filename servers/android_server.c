#include "network/serv.c"
#include "common.c"
#include <pthread.h>
#include <sys/reboot.h>


#define SRV_PORT 18000
#define MSG_RESP "done"
#define TABLET_SCREEN_TIME "/sys/devices/13800000.decon_fb/power/runtime_active_time"


void toggle_touch_controls(void);
void * monitor_screen_state(void * vargp);
int set_file_content(char * fp);
int connection_handler(int sd);


int main(int argc, char **argv)
{
    pthread_t t;
    int rc;

    rc = pthread_create(&t, NULL, monitor_screen_state, NULL);
    if (rc < 0)
    {
        printf("    Unable to monitor screen state");
    }

    listen_for_connections(SRV_PORT, connection_handler);
}


int connection_handler(int sd)
{
    toggle_touch_controls();
    send (sd, MSG_RESP, strlen(MSG_RESP), 0);
    return (0);
}


void * monitor_screen_state(void * vargp)
{
    char * last_runtime = get_file_content(TABLET_SCREEN_TIME, "r");
    char * this_runtime;


    printf("    Monitoring screen power state");
    while (1)
    {
        sleep(180);

        this_runtime = get_file_content(TABLET_SCREEN_TIME, "r");

        if (strcmp (this_runtime, last_runtime) == 0)
        {
            printf ("   No screen activity for 1 minute, shutting down...\n");
            sync();
            reboot(LINUX_REBOOT_CMD_POWER_OFF);
        }
        else
        {
            free(last_runtime);
            last_runtime = this_runtime;
        }
    }
}


void toggle_touch_controls()
{
    set_file_content("/sys/class/sec/sec_touchkey/input/enabled");
    set_file_content("/sys/class/sec/tsp/input/enabled");
}


int set_file_content(char * fp)
{
    FILE * f;
    long len;
    char * buf;
    int rc;


    f = fopen(fp, "rw+");
    if (f == NULL)
    {
        perror("    file not found");
        return(-1);
    }


    fseek (f, 0, SEEK_END);
    len = ftell (f);
    if (len < 1)
    {
        perror("    file has no content");
        fclose(f);
        return(-2);
    }


    fseek (f, 0, SEEK_SET);
    buf = malloc (len);
    if (!buf)
    {
        perror("    could not allocate buffer");
        fclose(f);
        return(-3);
    }


    fread (buf, 1, len, f);
    buf[strcspn(buf, "\r\n")] = 0x00;
    rewind(f);


    rc = strcmp("0", buf);
    if (rc == 0)
    {
        fwrite ("1", strlen("1"), 1, f);
    }
    else
    {
        fwrite ("0", strlen("0"), 1, f);
    }
    

    free (buf);
    fclose (f);

    return(0);
}