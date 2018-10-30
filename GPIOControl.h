/* Set for rpi3 model */
#define BCM2708_PERI_BASE 0x3F000000

/* GPIO controller */
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE        (4 * 1024)

/* GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) */
#define INP_GPIO(g) *(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO(g) *(gpio + ((g) / 10)) |=  (1 << (((g) % 10) * 3))
#define GET_GPIO(g) (* (gpio + 13)&(1 << g))

/* sets bits which are 1 and ignores bits which are 0 */
#define SET_GPIO *(gpio + 7)

/* clears bits which are 1 and ignores bits which are 0 */
#define CLR_GPIO *(gpio + 10)

/* used for opening '/dev/mem' */
int mem_fd;

/* for mapping the device into memory */
void *gpio_map;

/* sets up the gpio_map */
void setup_io();

/* Alternates a GPIO's state */
void toggle_gpio(int);

/* I/O Access */
volatile unsigned *gpio;



