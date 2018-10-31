#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/* Definitions of data types used in system calls */
#include <sys/types.h>

/* Definitions and structures required for websocket */
#include <sys/socket.h>

/* Constants and structures for internet domain addresses */
#include <netinet/in.h>

/* Include local definitions for GPIO control */
#include "GPIOControl.h"



int main(int argc, char **argv)
{

  /* Set up gpio pointer for direct register access */
  setup_io();
  

//WEBSOCKET CREATION

  /* File descriptors, they store the values returned by
   * the socket system call and accept the system call */
  int sockfd, newsockfd;

  /* Stores the size of the address of the client. This
   * is needed for the accept system call. */
  int clilen;

  /* Structure containing an internet address. */
  struct sockaddr_in serv_addr, cli_addr;

  /* 
   * socket:
   * System call to create a new socket. 
   * 
   * arg1 - domain of the socket   
   *        AF_UNIX for two processes which share a common file systen
   *        AF_INET for any two hosts on the internet 
   * arg2 - type of the socket 
   *        SOCK_STREAM characters read in a continous stream
   *        SOCK_DGRAM message is read in chunks
   * arg3 - protocol
   * 		0 operating system will choose the most appropriate protocol 
   *   		  TCP for stream sockets
   *   		  UDP for datagram sockets
   *
   */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);


  /* Set the address family */
  serv_addr.sin_family = AF_INET;


  /* htons: converts a port number to a network byte order */
  serv_addr.sin_port = htons (PORT_NUMBER);


  /* Sets the IP address of the host. */
  serv_addr.sin_addr.s_addr = INADDR_ANY;


  /* 
   * bind:
   * Binds a socket to an address, current host and
   * port number on which the server will run.
   *
   * arg1 - Socket descriptor, address to which is bound
   * arg2 - Pointer to structure of sockaddr, but serv_addr 
   *        is of type sockaddr_in and so it is cast to 
   *        the correct type.
   * arg3 - Size of the server address.
   *
   */
  if (bind(sockfd,
           (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0)
  {
    perror ("failed to bind");
    return -1;
  }


  /* 
   * listen:
   * Allows the process to listen on the socket for connections
   * 
   * arg1 - Socket file descriptor
   * arg2 - The number of connections that can be waiting 
   *        while the process is handling a connection 
   *
   */
  listen (sockfd, 5);


  /* Stores the size of the client address */
  clilen = sizeof(cli_addr);


  /* 
   * accept: 
   * System call causes the process to block until a client
   * has successfully connected to the server. It returns a
   * new file descriptor, all communication should be done
   * using the new file descriptor.
   *
   * arg1 - Socket file descriptor
   * arg2 - Reference pointer to the adress of the client, 
   *        cli_addr is of type sockaddr_in and so it must
   *        be cast to the correct type.
   * arg3 - Reference pointer to size of the structure. 
   *
   * */
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);






//END OF WEBSOCKET CREATION

  return 0;

}


/* 
 * Set up memory regions to access GPIO
 *
 */
void setup_io()
{
  /* open /dev/mem */
  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
  {
    printf("can't open /dev/mem \n");
    exit(-1);
  }

  /* mmap GPIO */
  gpio_map = mmap(
    NULL,                 // Any adddress in our space will do
    BLOCK_SIZE,           // Map length
    PROT_READ|PROT_WRITE, // Enable reading & writting to mapped memory
    MAP_SHARED,           // Shared with other processes
    mem_fd,               // File to map
    GPIO_BASE             // Offset to GPIO peripheral
  );

  /* No longer need to keep mem_fd after mmap */
  close(mem_fd);

  if (gpio_map == MAP_FAILED)
  {
    //printf("mmap error %d\n", (int)gpio_map); //errno also set!
    exit(-1);
  }

  /* Always use volatile pointer */
  gpio = (volatile unsigned *)gpio_map;
}


/* 
 * NOTE:
 * Setup a websocket for communication to the server
 * the websocket expects a string corresponding to a
 * certain GPIO, which will be toggled.
 *
 */
int setup_socket()
{
  int socket_desc, client_sock, c;
}


/* 
 * INFO:
 * Toggles the input and output of a specific GPIO
 * by checking the current state of the GPIO
 *
 * PARAMETERS:
 * in_gpio
 * [in] Specifies the GPIO to toggle 
 * 
 */
void toggle_gpio(int in_gpio)
{
  /* Set the GPIO to output */
  INP_GPIO(in_gpio);
  OUT_GPIO(in_gpio);

  /* Check the GPIO current state and alternate it */
  if (GET_GPIO(in_gpio) > 0)
  {
    CLR_GPIO = 1 << in_gpio;
  }
  else
  {
    SET_GPIO = 1 << in_gpio;
  }
}


