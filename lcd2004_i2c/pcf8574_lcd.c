/*
 * initial version to learn how to trigger a LCD2004/LCD1602
 * via PCF8574 which is connected to I2C
 *
 * function:
 * - connect LEDs with output of PCF8574
 * - let all LED blink
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c-dev.h>

/* the second PCF8574 address */
#define I2C_ADDR 0x27

/* trigger all LEDs on/off*/
#define LED_ON  0xFF
#define LED_OFF 0x00

/* time in usec */
#define BLINK_TIME 1000000

int main(void)
{
	int value;
	int fd;

	fd = open("/dev/i2c-1", O_RDWR);
	if (fd < 0) {
		printf("open error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0) {
		printf("ioctl error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		if (value == LED_ON)
			value = LED_OFF;
		else
			value = LED_ON;

		if (write(fd, &value, 1) != 1) {
			printf("write error: %s\n", strerror(errno));
		}

		usleep(BLINK_TIME);
	}

	return EXIT_SUCCESS;
}
