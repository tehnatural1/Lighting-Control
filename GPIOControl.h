
/*
 * These definitions depend on the chipset of the
 * raspberry pi, currently this is set to a model 3b+
 *
 */
#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE        (4 * 1024)


/* Disables the ability of a GPIO to output voltage */
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))

/* Allows the GPIO to output voltage */
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))

/*
 * Returns the current state of the GPIO
 * 0, if no voltage is being output
 * 1+, if voltage is currently being output on the GPIO
 *
 */
#define GET_GPIO(g) (*(gpio+13)&(1<<g))

/* Sets the GPIO to output voltage */
#define GPIO_SET *(gpio+7)

/* Sets the voltage output of the GPIO to 0 */
#define GPIO_CLR *(gpio+10)

/* GPIO pins are set to specific area of the house */
#define LIVINGROOM  4
#define STAIRWAY   23
#define PATIO      24
#define HALLWAY    25

/* Port to accept communications on */
#define MESSAGE_PORT 9090


int init();

/* toggles a pin on or off depending on it's current state */
void toggle_gpio(int);

void *new_conn(void *);

/* Sets a point for direct register access */
void setup_io();

int mem_fd;

void *gpio_map;

/* I/O access */
volatile unsigned *gpio;

int send_all(int, void *, size_t);

void log_to_file(char *, char *);

FILE *log_file;

time_t t;

int toggle_christmas_lights();
