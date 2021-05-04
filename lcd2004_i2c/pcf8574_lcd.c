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
 *
 * !!! TODO:   !!!!
 * - handle usleep EINTR
 *
 *
 * !!!! REAL stuff !!!!
 *
 *  Pinning:
 *
 *  LCD  -- PCF8574
 * ----------------
 *   rs  --  P0
 *   rw  --  P1
 *   en  --  P2
 *   bl  --  P3  (backlight)
 *  db4  --  P4
 *  db5  --  P5
 *  db6  --  P6
 *  db7  --  P7
 *
 * Functions using the "pinning" model:
 * - lcd_write_nibble
 * - ...
 *
 * Functions using the byte mode:
 * - lcd_write_data_lcd
 * - ...
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


/* define bit/pin positions to be used below */
enum bit_pos_priv {
	BIT0 = 1 << 0,
	BIT1 = 1 << 1,
	BIT2 = 1 << 2,
	BIT3 = 1 << 3,
	BIT4 = 1 << 4,
	BIT5 = 1 << 5,
	BIT6 = 1 << 6,
	BIT7 = 1 << 7
};

/*
 * Define pins and set pin to high!
 *
 * PCF8547 <-> LCD
 *  P7  P6  P5  P4 P3 P2 P1 P0
 *  |   |   |   |  |  |  |  |
 * DB7 DB6 DB5 DB4 BL EN RW RS
 */
#define RS BIT0
#define RW BIT1
#define EN BIT2
#define BL BIT3


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


/*
 * write nibble to lcd (handle only EN and BL)
 */
void lcd_write_nibble(int fd, unsigned char data)
{
	unsigned char value = 0x00 | EN | BL;
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}

	value = (data << 4) & 0xF0 | EN | BL;
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = value | ~(EN);
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
}


/*
 * write 8 bit of data to lcd (RS == 1)
 */
void lcd_write_data(int fd, unsigned char data)
{
	unsigned char value = 0x00 | RS | EN | BL;
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}

	value = (data & 0xF0) | RS | EN | BL;
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = value | ~(EN);
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}

	value = 0x00 | RS | EN | BL;
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}

	value = ((data << 4) & 0xF0) | RS | EN | BL;
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = value | ~(EN);
	printf("byte to send -> value 0x%x in %s\n", value, __FUNCTION__);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
}


/*
 * Init LCD:
 *
 * - Power supply on -> HD44780 goes through reset, so the following
 *   is not always needed, but doing it does not harm and brings the
 *   controller up in defined state
 * - BL and EN == 0
 * - RS R/W || DB7 DB6 DB5 DB4
 *   0   0  ||  0   0   1   1   -> 0x03  (Function set -> 8 bit mode)
 * - wait for > 4.1ms
 * - RS R/W || DB7 DB6 DB5 DB4
 *   0   0  ||  0   0   1   1   -> 0x03  (Function set -> 8 bit mode)
 * - wait for > 100us
 * - RS R/W || DB7 DB6 DB5 DB4
 *   0   0  ||  0   0   1   1   -> 0x03  (Function set -> 8 bit mode)
 *
 * Setup the LCD to wanted operation mode:
 * - RS R/W || DB7 DB6 DB5 DB4
 *   0   0  ||  0   0   1   0   -> 0x02  (Function set -> 4 bit mode)
 *
 * - ...
 */
void init_lcd(void)
{
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

	lcd_write_nibble(fd, 0x03);
	usleep(5);
	lcd_write_nibble(fd, 0x03);
	usleep(1);
	lcd_write_nibble(fd, 0x03);

	usleep(1);
	lcd_write_nibble(fd, 0x02); /* <- set to 4 bit mode */

	close(fd);
}


int main(void)
{
	init_lcd();

	return EXIT_SUCCESS;
}
