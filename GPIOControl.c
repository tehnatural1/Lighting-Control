
#include <stdio.h>

// Register access
#include <fcntl.h>


// String functions
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/socket.h>

// Internet
#include <arpa/inet.h>
#include <unistd.h>     //write

// For threading requests (lpthread)
#include <pthread.h>


// Used to get local IP address
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

// Time
#include <time.h>

// Holding definitions
#include "GPIOControl.h"


void
main()
{
  // Set up gpio pointer for register access
  setup_io();

  init();
}


/*
 * INFO:
 * Toggles the input and output of a specified GPIO
 * by checking if the GPIO is currently outputting
 * any voltage.
 *
 * PARAMETERS:
 * g:
 * Specifies the GPIO to toggle.
 *
 */
void
toggle_gpio(int g)
{
  /* Setting the GPIO state */
  INP_GPIO(g);
  OUT_GPIO(g);

  /* Check if GPIO outputting voltage */
  if (GET_GPIO(g) > 0)
  {
    GPIO_CLR = 1<<g;
  }
  else
  {
    GPIO_SET = 1<<g;
  }
}



/*
 * INFO:
 * Set up a memory regions to access GPIO directly
 *
 */
void
setup_io()
{

  // open /dev/mem REQUIRES SU
  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0)
  {
    printf ("can't open /dev/mem \n");
    exit(-1);
  }


  // mmap GPIO
  gpio_map = mmap(
    NULL,                  // Any adddress in our space will do
    BLOCK_SIZE,            // Map length
    PROT_READ|PROT_WRITE,  // Enable reading & writting to mapped memory
    MAP_SHARED,            // Shared with other processes
    mem_fd,                // File to map
    GPIO_BASE              // Offset to GPIO peripheral
  );


  //No need to keep mem_fd open after mmap
  close(mem_fd);


  if (gpio_map == MAP_FAILED)
  {
    printf (
      "mmap error %d\n",
      (int)gpio_map
    );

    exit(-1);
  }

  // Using a volatile pointer
  gpio = (volatile unsigned *)gpio_map;

}


/*
 * INFO:
 * Initialize a socket to accept incoming connections
 * then trigger gpio events based on the message sent via
 * the connection
 *
 */
int
init(void)
{

  int sock_desc, client_sock, c;

  struct sockaddr_in server, client;
  struct ifreq ifr;


  // Create a socket
  sock_desc = socket(
    AF_INET,
    SOCK_STREAM,
    0
  );

  // Prepare the socket structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(MESSAGE_PORT);


  // Bind socket descriptor
  if (
    bind(
      sock_desc,
      (struct sockaddr *)&server,
      sizeof(server)
    ) < 0)
  {
    perror ("binding failed");
    return 1;
  }

  listen(sock_desc, 3);


  // Type of address to retrieve - IPv4 IP address
  ifr.ifr_addr.sa_family = AF_INET;

  // Copy the interface name in the ifreq structure
  strncpy(
    ifr.ifr_name,
    "wlan0",
    IFNAMSIZ - 1
  );

  ioctl(
    sock_desc,
    SIOCGIFADDR,
    &ifr
  );


  // Displaying Server IP and Port
  printf (
    "\nLighting Server Setup - %s : %d\n",
    inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
    MESSAGE_PORT
  );


  printf("Waiting for connections\n");

  // Accept an incoming connection
  c = sizeof(struct sockaddr_in);

  pthread_t thread_id;

  while (
    client_sock = accept(
      sock_desc,
      (struct sockaddr *)&client,
      (socklen_t*)&c
    )
  )
  {

    // Logging IP of client connection
    log_to_file(
      "Client connected",
      inet_ntoa(client.sin_addr)
    );


    // Create a thread to handle connection
    if (
      pthread_create(
        &thread_id,
        NULL,
        new_conn,
        (void*)&client_sock
      ) < 0)
    {
      perror ("Failed to create a new thread.");
      return 1;
    }

  }

  if (client_sock < 0)
  {
    perror ("Failed to accept connection.");
    return 1;
  }

  return 0;

}


void
log_to_file(char *mess_desc, char *mess_item)
{

  char s[26];
  struct tm* tm_info;

  log_file = fopen("x.log", "a+");
  time(&t);

  tm_info = localtime(&t);

  strftime(s, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  // Writing log
  fprintf(
    log_file,
    "[%s]: %s: %s\n",
    s,
    mess_desc,
    mess_item
  );

  printf(
    "[%s]: %s: %s\n",
    s,
    mess_desc,
    mess_item
  );

  fclose(log_file);

  //memset(s, 0, sizeof(s));

  // Clear memory
  //free(s);

}



/*
 * INFO:
 * Handles each client connection and depending on the message
 * recieved over the websocket the corresponding GPIO on the
 * raspberry pi is triggered
 *
 */
void
*new_conn(void *sock_desc)
{

  // get socket descriptor
  int sock = *(int*)sock_desc;
  int msg_size;
  char *client_message = 0;

  client_message = malloc (20);

  // Recieve message from a client
  while (
    msg_size = recv(
      sock,
      client_message,
      20,
      0
    ) > 0)
  {

    // End of string marker
    //client_message[msg_size] = '\0';

    // Update log
    log_to_file(
      "Recieved client message",
      client_message
    );

    /**/ if (strcmp(client_message, "Patio") == 0)
    {
      toggle_gpio(PATIO);
    }
    else if (strcmp(client_message, "Stairway") == 0)
    {
      toggle_gpio(STAIRWAY);
    }
    else if (strcmp(client_message, "Living Room") == 0)
    {
      toggle_gpio(LIVINGROOM);
    }
    else if (strcmp(client_message, "Hallway") == 0)
    {
      toggle_gpio(HALLWAY);
    }
    else if (strcmp(client_message, "Tablet Input") == 0)
    {
      printf("attempting to connect to remote device");
      int status = system("adb connect 192.168.1.72:5555");
      int second = system("adb -s 192.168.1.72:5555 shell \"su -c 'echo 0 > /sys/class/sec/tsp/input/enabled && echo 0 > /sys/class/sec/sec_touchkey/input/enabled'\"");
      //toggle_christmas_lights();
    }
    else if (strcmp(client_message, "WhoYouBe") == 0)
    {
      send(sock, "butter", strlen("butter"), 0);
    }
    else if (strcmp(client_message, "getButtons") == 0)
    {
      char * buffer = 0;
      FILE * f = fopen ("buttons.json", "rb");
      long length;

      if (f)
      {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length);

        if (buffer)
        {
          fread (buffer, 1, length, f);
        }
        fclose (f);
      }

      if (buffer)
      {
        //printf("responding: %s\n", buffer);

        //send(sock, buffer, strlen(buffer), 0);

        if (send_all(sock, buffer, strlen(buffer)) < 0) {
          printf("error");
        }

        printf("finished responding!\n");

        free(buffer);

      }
    }

    // Disables further send and receive operations.
    shutdown(sock, SHUT_RDWR);

  }

  if (msg_size == 0)
  {
      struct sockaddr_in addr;
      socklen_t addr_len = sizeof(addr);
      int err = getpeername(
        sock,
        (struct sockaddr *)&addr,
        &addr_len
      );

      // Log IP of client disconnection
      log_to_file(
        "Client disconnected",
        inet_ntoa(addr.sin_addr)
      );

  }
  else if (msg_size == -1)
  {
    perror ("recv failed.");
  }

  free(client_message);

  return 0;
}


int send_all(int socket, void *buffer, size_t length)
{
    char *ptr = (char*) buffer;
    while (length > 0)
    {
        int i = send(socket, ptr, length, 0);
        if (i < 1) return -1;
        ptr += i;
        length -= i;
    }
    return 0;
}


int
toggle_christmas_lights()
{
  int sock;
  struct sockaddr_in xmas_pi;

  // Create socket
  sock = socket(
    AF_INET,
    SOCK_STREAM,
    0
  );


  if (sock == -1)
  {
    printf("Could not create socket");
    return 1;
  }

  xmas_pi.sin_addr.s_addr = inet_addr("192.168.1.76");
  xmas_pi.sin_family = AF_INET;
  xmas_pi.sin_port = htons(MESSAGE_PORT);

  // Connect to other pi
  if (
    connect(
      sock,
      (struct sockaddr *)&xmas_pi,
      sizeof(xmas_pi)
    ) < 0)
  {
    perror("connect failed. Error");
    return 1;
  }

  // Send message to other pi
  if (
    send(
      sock,
      "4",
      strlen("4"),
      0
    ) < 0)
  {
    puts("Send failed");
    return 1;
  }

  // Close socket
  close(sock);

  // Clear memory
  //free(sock);
  //free(xmas_pi);

  return 0;
}
