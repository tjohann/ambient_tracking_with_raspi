/*
 * pcf8574_lcd.c -> userspace lcd application
 *
 * GPL
 * (c) 2021, thorsten.johannvorderbrueggen@t-online.de
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * !!!! ACTUAL state of development !!!!
 *
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

/* the first PCF8574 address -> LCD */
#define I2C_ADDR_LCD 0x22

/* the second PCF8574 address -> LEDs */
#define I2C_ADDR_LED 0x27

/* trigger all LEDs on/off*/
#define LED_ON  0xFF
#define LED_OFF 0x00

/* time in usec -> 1000000 == 1s */
#define BLINK_TIME 1000000


void __attribute__((noreturn)) blink_leds(void)
{
	int value = LED_OFF;
	int fd = -1;

	fd = open("/dev/i2c-1", O_RDWR);
	if (fd < 0) {
		printf("open error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, I2C_SLAVE, I2C_ADDR_LED) < 0) {
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
}

int main(void)
{
	blink_leds();

	return EXIT_SUCCESS;
}
