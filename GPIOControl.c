#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "GPIOControl.h"


int main(int argc, char **argv)
{

  /* Set up gpio pointer for direct register access */
  setup_io();

  toggle_gpio(27);

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
    printf("mmap error %d\n", (int)gpio_map); //errno also set!
    exit(-1);
  }

  /* Always use volatile pointer */
  gpio = (volatile unsigned *)gpio_map;
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


