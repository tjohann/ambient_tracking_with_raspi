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

#define LOCKFILE "/var/run/lcd_daemon.pid"
#define DAEMON_FIFO LCD_FIFO

#define LCD_WRITE_NIBBLE(data) do {					\
		if (lcd_write_nibble(data) < 0) {			\
			syslog(LOG_ERR, "error in lcd_write_nibble()"); \
			return -1; }					\
	} while(0)

#define LCD_WRITE_DATA(data) do {					\
		if (lcd_write_data(data) < 0) {				\
			syslog(LOG_ERR, "error in lcd_write_data()");	\
			return -1; }					\
	} while(0)

#define INIT_LCD_DEVICE(adapter, addr) do {				\
		lcd = init_i2c_device(adapter, addr);			\
		if (lcd < 0) {						\
			eprintf("ERROR: can't init LCD");		\
			return -1; }					\
	} while(0)

/* the lcd display */
static int lcd = -1;

/* the lcd type info */
static unsigned char lcd_type = 0;
static unsigned char lcd_max_line = 0;
static unsigned char lcd_max_col = 0;

/* the daemon read fifo */
static int read_fifo = -1;

extern char *__progname;

/* common functions */
int lcd_clear(void);
int lcd_cursor_home(void);
int lcd_display_on(void);
int lcd_display_on_2(void);
int lcd_display_on_3(void);
int lcd_cursor_shift_right(void);
int lcd_cursor_shift_left(void);
int lcd_write_line(struct lcd_request req);
int lcd_write_string (unsigned char cur_pos, char * str);
int init_lcd(char *adapter, unsigned char addr, int type);


static void
__attribute__((noreturn)) usage(void)
{
	putchar('\n');
	fprintf(stdout, "Usage: ./%s -[a:hi:t:]                            \n",
		__progname);
	fprintf(stdout, "       -i /dev/i2c-X -> I2C adapter               \n");
	fprintf(stdout, "       -a 22         -> LCD address (in hex)      \n");
	fprintf(stdout, "       -t 2          -> LCD1602(:1)/LCD2004(:2)   \n");
	fprintf(stdout, "       -h            -> show this help            \n");
	putchar('\n');
	fprintf(stdout, "Example(s): LCD at first adapter with address 0x22\n");
	fprintf(stdout, "            from type LCD2004                     \n");
	fprintf(stdout, "           ./%s -i /dev/i2c-1 -a 22 -t 2          \n",
		__progname);
	fprintf(stdout, "or                                                \n");
	fprintf(stdout, "            LCD at first adapter with address 0x27\n");
	fprintf(stdout, "            from type LCD1602                     \n");
	fprintf(stdout, "            ./%s -i /dev/i2c-1 -a 27 -t 1         \n",
		__progname);
	putchar('\n');

	exit(EXIT_FAILURE);
}

static void cleanup(void)
{
	/* ignore all errors */

	lcd_clear();

	if (lcd > 0)
		close(lcd);

	unlink(DAEMON_FIFO);
	unlink(LOCKFILE);

	syslog(LOG_INFO, "daemon is down -> bye");
}

static void daemon_handling(void)
{
	if (become_daemon(__progname) < 0) {
		eprintf("ERROR: can't become a daemon\n");
		exit(EXIT_FAILURE);
	}

	int err = already_running(LOCKFILE);
	if (err == 1) {
		syslog(LOG_ERR, "i'm already running");
		exit(EXIT_FAILURE);
	} else if (err < 0) {
		syslog(LOG_ERR, "can't setup lockfile");
		exit(EXIT_FAILURE);
	}
}

/*
 * write nibble to lcd (handle only EN and BL)
 *
 * see timing chart of datasheet (HD44780)
 */
static int lcd_write_nibble(unsigned char data)
{
	unsigned char value = 0x00 | BL;
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	value = 0x00 | EN | BL;
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	value = ((data << 4) & 0xF0) | EN | BL;
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(EXEC_TIME);

	value = value & ~(EN);
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	return 0;
}

/*
 * write 8 bit of data to lcd (RS == 1)
 */
static int lcd_write_data(unsigned char data)
{
	unsigned char value = 0x00 | RS | BL;
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	value = 0x00 | RS | EN | BL;
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	value = (data & 0xF0) | RS | EN | BL;
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(EXEC_TIME);

	value = value & ~(EN);
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	value = ((data << 4) & 0xF0) | RS | EN | BL;
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(EXEC_TIME);

	value = value & ~(EN);
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	value = value & ~(RS);
	if (write(lcd, &value, 1) != 1) {
		printf("write error: %s\n", strerror(errno));
		return -1;
	}
	usleep(1);

	return 0;
}

/*
 * line 1 -> 0x80
 */
static int goto_line1(void)
{
	LCD_WRITE_NIBBLE(0x08);
	LCD_WRITE_NIBBLE(0x00);

	usleep(1);
	return 0;
}

/*
 * line 2 -> 0xC0
 */
static int goto_line2(void)
{
	LCD_WRITE_NIBBLE(0x0C);
	LCD_WRITE_NIBBLE(0x00);

	usleep(1);
	return 0;
}

/*
 * line 3 -> 0x94
 */
static int goto_line3(void)
{
	LCD_WRITE_NIBBLE(0x09);
	LCD_WRITE_NIBBLE(0x04);

	usleep(1);
	return 0;
}

/*
 * line 4 -> 0xD4
 */
static int goto_line4(void)
{
	LCD_WRITE_NIBBLE(0x0D);
	LCD_WRITE_NIBBLE(0x04);

	usleep(1);
	return 0;
}

/*
 * say hello after startup
 */
static int say_hello(void)
{
	int i = 0;

	switch(lcd_type) {
	case LCD1602:
		goto_line1();
		LCD_WRITE_DATA('*');
		for (i = 1; i < lcd_max_col; i++)
			(void) lcd_cursor_shift_right();
		LCD_WRITE_DATA('*');

		goto_line2();
		LCD_WRITE_DATA('*' );
		for (i = 1; i < lcd_max_col; i++)
			(void) lcd_cursor_shift_right();
		LCD_WRITE_DATA('*');

		break;
	case LCD2004:
		goto_line1();
		for (i = 1; i < lcd_max_col; i++)
			LCD_WRITE_DATA('*');

		goto_line4();
		for (i = 1; i < lcd_max_col; i++)
			LCD_WRITE_DATA('*');

		goto_line2();
		LCD_WRITE_DATA('*' );
		for (i = 1; i < lcd_max_col; i++)
			(void) lcd_cursor_shift_right();
		LCD_WRITE_DATA('*');

		goto_line3();
		LCD_WRITE_DATA('*' );
		for (i = 1; i < lcd_max_col; i++)
			(void) lcd_cursor_shift_right();
		LCD_WRITE_DATA('*');

		break;
	default:
		eprintf("LCD is not supported!\n");
		return -1;
	}

	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * ############################################################################
 * ----------------------------------------------------------------------------
 */

int lcd_clear(void)
{
	LCD_WRITE_NIBBLE(0x00);
	LCD_WRITE_NIBBLE(0x01);

	usleep(EXEC_TIME);
	return 0;
}

int lcd_cursor_home(void)
{
	LCD_WRITE_NIBBLE(0x00);
	LCD_WRITE_NIBBLE(0x02);

	usleep(EXEC_TIME);
	return 0;
}

/*
 * lcd display on
 * - cursor on
 * - cursor blinking
 */
int lcd_display_on(void)
{
	LCD_WRITE_NIBBLE(0x00);
	LCD_WRITE_NIBBLE(0x0F);

	usleep(EXEC_TIME);
	return 0;
}

/*
 * lcd display on
 * - cursor on
 * - cursor blinking OFF
 */
int lcd_display_on_2(void)
{
	LCD_WRITE_NIBBLE(0x00);
	LCD_WRITE_NIBBLE(0x0E);

	usleep(EXEC_TIME);
	return 0;
}

/*
 * lcd display on
 * - cursor OFF
 * - cursor blinking OFF
 */
int lcd_display_on_3(void)
{
	LCD_WRITE_NIBBLE(0x00);
	LCD_WRITE_NIBBLE(0x0C);

	usleep(EXEC_TIME);
	return 0;
}

int lcd_cursor_shift_right(void)
{
	LCD_WRITE_NIBBLE(0x01);
	LCD_WRITE_NIBBLE(0x04);

	usleep(EXEC_TIME);
	return 0;
}

int lcd_cursor_shift_left(void)
{
	LCD_WRITE_NIBBLE(0x01);
	LCD_WRITE_NIBBLE(0x00);

	usleep(EXEC_TIME);
	return 0;
}

int lcd_write_string (unsigned char cur_pos, char *str)
{
	int i = 0;

	if (cur_pos != 0)
		for (i = 1; i < cur_pos; i++)
			(void) lcd_cursor_shift_right();

	for (i = 0; i < lcd_max_col - cur_pos -1; i++ )
		LCD_WRITE_DATA(str[i]);

	return 0;
}

int lcd_write_line(struct lcd_request req)
{

#ifdef __DEBUG__
	syslog(LOG_INFO, "value of req.line: %d", req.line);
	syslog(LOG_INFO, "value of req.curs_pos: %d", req.cur_pos);
	syslog(LOG_INFO, "value of req.str: %s", req.str);
#endif

	switch(req.line) {
	case 1:
		if (goto_line1() != 0)
			return -1;
		break;
	case 2:
		if (goto_line2() != 0)
			return -1;
		break;
	case 3:
		if (goto_line3() != 0)
			return -1;
		break;
	case 4:
		if (goto_line4() != 0)
			return -1;
		break;
	default:
		syslog(LOG_ERR, "value of req.line is to large: %d",
			req.line);
		return -1;
	}

	if (lcd_write_string(req.cur_pos, req.str) != 0) {
		syslog(LOG_ERR, "can't write string %s", req.str);
		return -1;
	}

	return 0;
}

int lcd_write_cmd(struct lcd_request req)
{

#ifdef __DEBUG__
	syslog(LOG_INFO, "value of req.line: %d", req.line);
	syslog(LOG_INFO, "value of req.curs_pos: %d", req.cur_pos);
	syslog(LOG_INFO, "value of req.str: %s", req.str);
#endif

	switch(req.line) {
	case LCD_CLEAR:
		if (lcd_clear() != 0)
			return -1;
		break;
	case LCD_HOME:
		if (lcd_cursor_home() != 0)
			return -1;
		break;
	default:
		syslog(LOG_ERR, "value of req.line is to large: %d",
			req.line);
		return -1;
	}

	return 0;
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
int init_lcd(char *adapter, unsigned char addr, int type)
{
	switch(type) {
	case 1:
		lcd_type = LCD1602;
		lcd_max_line = LCD1602_MAX_LINE;
		lcd_max_col = LCD1602_MAX_COL;
		break;
	case 2:
		lcd_type = LCD2004;
		lcd_max_line = LCD2004_MAX_LINE;
		lcd_max_col = LCD2004_MAX_COL;
		break;
	default:
		eprintf("LCD is not supported!\n");
		return -1;
	}

	INIT_LCD_DEVICE(adapter, addr);
	usleep(SETUP_TIME); /* HD44780 internal setup time   */

	LCD_WRITE_NIBBLE(0x03);
	usleep(5000);
	LCD_WRITE_NIBBLE(0x03);
	usleep(1);
	LCD_WRITE_NIBBLE(0x03);
  	usleep(1);

	LCD_WRITE_NIBBLE(0x02); /* <- set to 4 bit mode   */
  	usleep(1);

	/* function set */
	LCD_WRITE_NIBBLE(0x02); /* <- 4 bit mode          */
        LCD_WRITE_NIBBLE(0x08); /* <- 2 lines/5x8 fonts   */
	usleep(EXEC_TIME);

	if (lcd_display_on_3() < 0)
		return -1;
	if (lcd_clear() < 0)
		return -1;

	LCD_WRITE_NIBBLE(0x00); /* <- entry mode          */
	LCD_WRITE_NIBBLE(0x06); /*                        */
	usleep(EXEC_TIME);

	return 0;
}

/* the main thread */
void * server_handling(void *arg)
{
	/* this call won't block -> clear of flag is below */
	read_fifo = create_read_fifo(DAEMON_FIFO);
	if (read_fifo < 0) {
		syslog(LOG_ERR, "can't setup read fifo");
		exit(EXIT_FAILURE);
	}

	/* now no EOF possible */
	int dummy_fd = open(DAEMON_FIFO, O_WRONLY);
	if (dummy_fd < 0) {
		syslog(LOG_ERR, "open in server_handling()");
		exit(EXIT_FAILURE);
	}

	/* clear O_NONBLOCK -> the read will now block */
	clr_flag(read_fifo, O_NONBLOCK);

	struct lcd_request req;
	size_t len = sizeof(struct lcd_request);
	memset(&req, 0, len);

	for (;;) {
		if (read(read_fifo, &req, len) != (int) len) {
			syslog(LOG_ERR,
				"len of request not valid -> ignore it");
			continue;
		}

		if (req.line != 0) {
			if (lcd_write_line(req) != 0) {
				syslog(LOG_ERR,
					"can't write line -> ignore it");
				continue;
			}
		} else {
			if (lcd_write_cmd(req) != 0) {
				syslog(LOG_ERR,
					"can't write cmd -> ignore it");
				continue;
			}
		}

		memset(&req, 0, len);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	char *adapter = NULL;
	unsigned char addr = 0x00;
	int type = -1;

	int c;
	while ((c = getopt(argc, argv, "a:hi:t:")) != -1) {
		switch (c) {
		case 'i':
			adapter = optarg;
			break;
		case 'a':
			addr = (unsigned char) strtoul(optarg, NULL, 16);
			break;
		case 't':
			type = atoi(optarg);
			break;
		case 'h':
			usage();
			break;
		default:
			eprintf("ERROR -> no valid argument\n");
			usage();
		}
	}

	if ((adapter == NULL) || (addr == 0) || (type == -1))
		usage();

	fprintf(stdout, "try to open %s@0x%x\n", adapter, (int) addr);

	if (access(adapter, R_OK | W_OK) == -1 ) {
		perror("ERROR: can't access /dev/YOUR_PROVIDED_I2C_ADAPTER");
		usage();
	}

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	daemon_handling();

	err = init_lcd(adapter, addr, type);
	if (err < 0) {
		syslog(LOG_ERR, "can't init LCD");
		exit(EXIT_FAILURE);
	}

	pthread_t tid;
	err = pthread_create(&tid, NULL, server_handling, NULL);
	if (err != 0) {
		syslog(LOG_ERR, "can't create thread");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_INFO, "daemon is up and running");
	(void) say_hello();

	(void) pthread_join(tid, NULL);
	return EXIT_SUCCESS;
}
