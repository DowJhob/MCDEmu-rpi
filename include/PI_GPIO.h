/*
 PI_GPIO.h  (c)
*/

#define GPIO_BASE  (0x20200000)

#define BLOCK_SIZE (4096) /* size of one MMU page */

#define BCM_GPIO_IN   (0)
#define BCM_GPIO_OUT  (1)
#define BCM_GPIO_ALT0 (4)
#define BCM_GPIO_ALT1 (5)
#define BCM_GPIO_ALT2 (6)
#define BCM_GPIO_ALT3 (7)
#define BCM_GPIO_ALT4 (3)
#define BCM_GPIO_ALT5 (2)
#define BCM_GPIO_ALT3 (7)


// Set or Clear the pins :
#define RPI_GPSET0   (7)
#define RPI_GPCLR0  (10)
#define RPI_GPLEV0  (13)


void PI_GPIO_set_n(int port);

void PI_GPIO_clr_n(int port);

int PI_GPIO_lev_n(int port);

unsigned * PI_IOmmap(int where);

void PI_GPIO_config(int port, int mode);
