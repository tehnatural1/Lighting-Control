#include <stdio.h>

// Register access
#include <fcntl.h>

// String functions
#include <string.h>

// System call, atoi conversion
#include <stdlib.h>

// isdigit call
#include <ctype.h>

#include <sys/mman.h>
#include <sys/socket.h>

// Internet
#include <arpa/inet.h>
#include <unistd.h>     //write, usleep

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
    printf("mmap error %d\n", (int) gpio_map);
    exit(-1);
  }

  // Using a volatile pointer
  gpio = (volatile unsigned *) gpio_map;

}


int
init(void)
{
  int sock_desc, client_sock, c;

  struct sockaddr_in server, client;
  struct ifreq ifr;


  // Create a socket
  sock_desc = socket(AF_INET, SOCK_STREAM, 0);

  // Prepare the socket structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(MESSAGE_PORT);

  // Bind socket descriptor
  if (
    bind(
      sock_desc,
      (struct sockaddr *) & server,
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
  strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);

  ioctl(sock_desc, SIOCGIFADDR, &ifr);

  // Displaying Server IP and Port
  printf (
    "\nLighting Server Setup - %s : %d\n",
    inet_ntoa(((struct sockaddr_in *) & ifr.ifr_addr) -> sin_addr),
    MESSAGE_PORT
  );


  // Accept an incoming connection
  c = sizeof(struct sockaddr_in);

  pthread_t thread_id;

  while (
    client_sock = accept(
      sock_desc,
      (struct sockaddr *) & client,
      (socklen_t *) & c
    )
  )
  {

    // Logging IP of client connection
    log_to_file("Client connected",inet_ntoa(client.sin_addr));

    // Create a thread to handle connection
    if (
      pthread_create(
        &thread_id,
        NULL,
        new_conn,
        (void *) & client_sock
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
log_to_file(char * tag, char * msg)
{
  char * s = 0;
  struct tm * tm_info;
  int date_size = 26;

  log_file = fopen("x.log", "a+");
  time(& t);
  tm_info = localtime(& t);
  s = malloc (date_size);

  strftime(s, date_size, "%Y-%m-%d %H:%M:%S", tm_info);

  // Write to log file
  fprintf(log_file, "[%s]: %s: %s\n", s, tag, msg);

  // Write to console
  printf("[%s]: %s: %s\n", s, tag, msg);

  fclose(log_file);

  // Clear memory
  free(s);
}


void
*new_conn(void * sock_desc)
{
  // get socket descriptor
  int sock = * (int *) sock_desc;
  int msg_size;
  char * msg = 0;

  msg = malloc (20);

  // Recieve message from a client
  recv(sock, msg, 20, 0);

  // Update log
  log_to_file("Recieved client request", msg);

  /**/ if (is_number(msg))
  {
    log_to_file("Response", "Toggling GPIO");
    toggle_gpio(atoi(msg));
    send(sock, MSG_ACK, strlen(MSG_ACK), 0);
  }
  else if (strcmp(msg, MSG_TABLET) == 0)
  {
    log_to_file("Response", "Altering Tablet Input");
    disable_tablet_input();
    send(sock, MSG_ACK, strlen(MSG_ACK), 0);
  }
  else if (strcmp(msg, MSG_ALIVE_CHK) == 0)
  {
    log_to_file("Response", "Sending Verify");
    send(sock, MSG_ALIVE_RESP, strlen(MSG_ALIVE_RESP), 0);
  }
  else if (strcmp(msg, MSG_BTN_LAYOUT) == 0)
  {
    log_to_file("Response", "Sending layout");
    send_json(sock);
  }

  log_to_file("Socket", "Closing connection");
  shutdown(sock, SHUT_RDWR);
  close(sock);

  free(msg);
  pthread_exit(NULL);
}


int
is_number(char * s)
{
  for (int i = 0; i < strlen(s); i++)
  {
    if (!isdigit(s[i]))
    {
      return 0;
    }
  }
  return 1;
}


int
send_json(int sock)
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
    if (send_all(sock, buffer, strlen(buffer)) < 0) {
      printf("Error sending JSON");
    }

    free(buffer);
  }

  return 0;
}


int
disable_tablet_input()
{
  int connect = system("adb connect 192.168.1.72:5555");
  int attempts = 0;
  int disconnect;
  int input_toggle;

  while (
    input_toggle = system(
      "adb -s 192.168.1.72:5555 shell \"su -c '"
      "sh /data/local/tmp/toggle_input.sh'\""
    ) > 0 && attempts < 20
  )
  {
    attempts++;
    usleep(100000);
  }

  attempts = 0;

  while (
    disconnect = system(
      "adb disconnect"
    ) > 0 && attempts < 20
  )
  {
    attempts++;
    usleep(100000);
  }

  if ((connect + input_toggle + disconnect) != 0)
  {
    return -1;
  }

  return 0;
}


int
send_all(int socket, void *buffer, size_t length)
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
  sock = socket(AF_INET, SOCK_STREAM, 0);

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
  if (send(sock, "4", strlen("4"), 0) < 0)
  {
    puts("Send failed");
    return 1;
  }

  // Close socket
  close(sock);

  return 0;
}
