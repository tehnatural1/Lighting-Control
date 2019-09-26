#include <stdio.h>
#include <fcntl.h>      // Register access
#include <sys/mman.h>   // PROT_READ, PROT_SHARED


/* RPi 3B+ hardware specific */
#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE        (4 * 1024)

#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define GET_GPIO(g) (*(gpio+13)&(1<<g))

#define GPIO_SET *(gpio+7)
#define GPIO_CLR *(gpio+10)



volatile unsigned *gpio;

void setup_io();
void toggle_gpio(int);