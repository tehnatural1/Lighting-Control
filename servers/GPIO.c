#include "GPIO.h"


void setup_io()
{

    int mem_fd = open ("/dev/mem", O_RDWR|O_SYNC);
    
    
    if (mem_fd < 0)
    {
        printf ("Requires root! Cannot open /dev/mem \n");
        exit (-1);
    }


    void *gpio_map = mmap(
        NULL,                   // Any adddress in our space will do
        BLOCK_SIZE,             // Map length
        PROT_READ|PROT_WRITE,   // Enable reading & writting to mapped memory
        MAP_SHARED,             // Shared with other processes
        mem_fd,                 // File to map
        GPIO_BASE               // Offset to GPIO peripheral
    );


    close (mem_fd);


    if (gpio_map == MAP_FAILED)
    {
        printf ("Unable to map out gpio\n");
        exit (-1);
    }


    gpio = (volatile unsigned *) gpio_map;

}


void toggle_gpio(int g)
{
    INP_GPIO (g);
    OUT_GPIO (g);


    if (GET_GPIO (g) > 0)
    {
        GPIO_CLR = 1<<g;
    }
    else
    {
        GPIO_SET = 1<<g;
    }
}