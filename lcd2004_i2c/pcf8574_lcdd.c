/*
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
 *
 * ----------------------------------------------------------------------------
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
 */

#include <libhelper.h>
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

/* PCF8574 address of the LCD */
#define I2C_ADDR_LCD 0x22

/* HD44780 internal setup time */
#define SETUP_TIME 10000

/* HD44780 default exceution time (37us + buffer) */
#define EXEC_TIME 100

/* address of lines */
#define LINE1
#define LINE2
#define LINE3
#define LINE4 0x04

/* the lcd display */
static int fd = -1;


/* common functions */
void lcd_clear(void);
void lcd_cursor_home(void);
void lcd_display_on(void);
void lcd_display_on_2(void);
void lcd_display_on_3(void);
void lcd_cursor_shift_right(void);
void lcd_cursor_shift_left(void);
void init_lcd(unsigned char addr);


/*
 * clear display
 */
static void cleanup(void)
{
	lcd_clear();

	close(fd);
}

/*
 * write nibble to lcd (handle only EN and BL)
 *
 * see timing chart of datasheet (HD44780)
 */
static void lcd_write_nibble(unsigned char data)
{
	unsigned char value = 0x00 | BL;
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = 0x00 | EN | BL;
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = ((data << 4) & 0xF0) | EN | BL;
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(EXEC_TIME);

	value = value & ~(EN);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);
}

/*
 * write 8 bit of data to lcd (RS == 1)
 */
static void lcd_write_data(unsigned char data)
{
	unsigned char value = 0x00 | RS | BL;
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = 0x00 | RS | EN | BL;
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = (data & 0xF0) | RS | EN | BL;
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(EXEC_TIME);

	value = value & ~(EN);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = ((data << 4) & 0xF0) | RS | EN | BL;
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(EXEC_TIME);

	value = value & ~(EN);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);

	value = value & ~(RS);
	if (write(fd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
	}
	usleep(1);
}

/*
 * goto line 1 -> 0x80
 */
static void goto_line1(void)
{
	lcd_write_nibble(0x08);
	lcd_write_nibble(0x00);

	usleep(1);
}

/*
 * goto line 2 -> 0xC0
 */
static void goto_line2(void)
{
	lcd_write_nibble(0x0C);
	lcd_write_nibble(0x00);

	usleep(1);
}

/*
 * goto line 3 -> 0x94
 */
static void goto_line3(void)
{
	lcd_write_nibble(0x09);
	lcd_write_nibble(0x04);

	usleep(1);
}

/*
 * goto line 4 -> 0xD4
 */
static void goto_line4(void)
{
	lcd_write_nibble(0x0D);
	lcd_write_nibble(0x04);

	usleep(1);
}


/*
 * ----------------------------------------------------------------------------
 * ############################################################################
 * ----------------------------------------------------------------------------
 */


/*
 * clear the display
 */
void lcd_clear(void)
{
	lcd_write_nibble(0x00);
	lcd_write_nibble(0x01);

	usleep(EXEC_TIME);
}

/*
 * set cursor to home
 */
void lcd_cursor_home(void)
{
	lcd_write_nibble(0x00);
	lcd_write_nibble(0x02);

	usleep(EXEC_TIME);
}

/*
 * lcd display on
 * - cursor on
 * - cursor blinking
 */
void lcd_display_on(void)
{
	lcd_write_nibble(0x00);
	lcd_write_nibble(0x0F);

	usleep(EXEC_TIME);
}

/*
 * lcd display on
 * - cursor on
 * - cursor blinking OFF
 */
void lcd_display_on_2(void)
{
	lcd_write_nibble(0x00);
	lcd_write_nibble(0x0E);

	usleep(EXEC_TIME);
}

/*
 * lcd display on
 * - cursor OFF
 * - cursor blinking OFF
 */
void lcd_display_on_3(void)
{
	lcd_write_nibble(0x00);
	lcd_write_nibble(0x0C);

	usleep(EXEC_TIME);
}

/*
 * cursor shift right
 */
void lcd_cursor_shift_right(void)
{
	lcd_write_nibble(0x01);
	lcd_write_nibble(0x04);

	usleep(EXEC_TIME);
}

/*
 * cursor shift left
 */
void lcd_cursor_shift_left(void)
{
	lcd_write_nibble(0x01);
	lcd_write_nibble(0x00);

	usleep(EXEC_TIME);
}

/*
 * Init LCD:
 *
 * - Power supply on -> HD44780 goes through reset, this take 10ms
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
void init_lcd(unsigned char addr)
{
	fd = open("/dev/i2c-1", O_RDWR);
	if (fd < 0) {
		printf("open error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		printf("ioctl error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	usleep(SETUP_TIME); /* HD44780 internal setup time   */

	lcd_write_nibble(0x03);
	usleep(5000);
	lcd_write_nibble(0x03);
	usleep(1);
	lcd_write_nibble(0x03);
  	usleep(1);

	lcd_write_nibble(0x02); /* <- set to 4 bit mode   */
  	usleep(1);

	/* function set */
	lcd_write_nibble(0x02); /* <- 4 bit mode          */
	lcd_write_nibble(0x08); /* <- 2 lines/5x8 fonts   */
	usleep(EXEC_TIME);

	lcd_display_on_3();
	lcd_clear();

	lcd_write_nibble(0x00); /* <- entry mode          */
	lcd_write_nibble(0x06); /*                        */
	usleep(EXEC_TIME);
}


int main(void)
{
	init_lcd(I2C_ADDR_LCD);

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	goto_line4();

	lcd_write_data('l' );
	lcd_write_data('i' );
	lcd_write_data('n' );
	lcd_write_data('e' );
	lcd_write_data(':' );
	lcd_write_data('4' );

	goto_line3();

	lcd_write_data('l' );
	lcd_write_data('i' );
	lcd_write_data('n' );
	lcd_write_data('e' );
	lcd_write_data(':' );
	lcd_write_data('3' );

	goto_line2();

	lcd_write_data('l' );
	lcd_write_data('i' );
	lcd_write_data('n' );
	lcd_write_data('e' );
	lcd_write_data(':' );
	lcd_write_data('2' );

	goto_line1();

	lcd_write_data('l' );
	lcd_write_data('i' );
	lcd_write_data('n' );
	lcd_write_data('e' );
	lcd_write_data(':' );
	lcd_write_data('1' );

	usleep(10000000);

	return EXIT_SUCCESS;
}
