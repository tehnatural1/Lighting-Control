#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>


#define MSG_TABLET     				"Tablet Input"
#define MSG_ALIVE_CHK  				"WhoYouBe"
#define MSG_BTN_LAYOUT 				"getButtons"
#define MSG_ALIVE_RESP 				"butter"
#define MSG_ACK        				"done"

#define MESSAGE_PORT 				9090

#define TABLET_IP                   "192.168.1.72"
#define TABLET_PORT                 18000
#define TABLET_TOGGLE               "butter"


#define MAX_RECV_CLIENT_TIMEOUT     2
#define MAX_CONNECTION_REQUESTS     5
#define MAX_READ_SIZE               20


int listen_for_connections();
int handle_connection(int sd);
int send_all(int sd, void *buffer, size_t length);
int msg_to(char * ip, int port, char * msg);

void connection_timeout(int sig);

