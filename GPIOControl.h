/* RPi 3B+ hardware specific */
#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE        (4 * 1024)

/* Disables the ability of a GPIO to output voltage */
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))

/* Allows the GPIO to output voltage */
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))

/* Returns the current state of the GPIO */
#define GET_GPIO(g) (*(gpio+13)&(1<<g))

/* Sets the GPIO to output voltage */
#define GPIO_SET *(gpio+7)

/* Sets the voltage output of the GPIO to 0 */
#define GPIO_CLR *(gpio+10)

/* GPIO pins are set to specific area of the house */
#define MSG_TABLET     "Tablet Input"
#define MSG_ALIVE_CHK  "WhoYouBe"
#define MSG_BTN_LAYOUT "getButtons"
#define MSG_ALIVE_RESP "butter"
#define MSG_ACK        "done"

/* Port to accept communications on */
#define MESSAGE_PORT 9090


int disable_tablet_input();
int init();
int is_number(char *);
int mem_fd;
int send_all(int, void *, size_t);
int send_json(int);
int toggle_christmas_lights();

void *gpio_map;
void *new_conn(void *);

void log_to_file(char *, char *);
void toggle_gpio(int);
void setup_io();

volatile unsigned *gpio;

FILE *log_file;

time_t t;
