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
 */

#include <libhelper.h>

/* the write to lcd_daemon fifo */
static int lcd_fd = -1;

/* the lcd type info */
static unsigned char lcd_type = 0;
static unsigned char lcd_max_line = 0;
static unsigned char lcd_max_col = 0;

extern char *__progname;

/* common functions */


static void
__attribute__((noreturn)) usage(void)
{
	putchar('\n');
	fprintf(stdout, "Usage: ./%s -[h]                                  \n",
		__progname);
	fprintf(stdout, "       -t 2          -> LCD1602(:1)/LCD2004(:2)   \n");
	fprintf(stdout, "       -h            -> show this help            \n");
	putchar('\n');
	fprintf(stdout, "Example: LCD size 20x4                            \n");
	fprintf(stdout, "        ./%s -t 2                                 \n",
		__progname);

	exit(EXIT_FAILURE);
}

static void cleanup(void)
{
	if (lcd_fd > 0)
		close(lcd_fd);

	fprintf(stdout, "application is down -> bye\n");
}

static int init_lcd(int type)
{
	lcd_fd = open(LCD_FIFO, O_WRONLY);
	if (lcd_fd < 0) {
		perror("open in main()");
		return -1;;
	}

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

	return 0;
}


int main(int argc, char *argv[])
{
	int type = -1;

	int c;
	while ((c = getopt(argc, argv, "ht:")) != -1) {
		switch (c) {
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

	if (type == -1)
		usage();

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	err = init_lcd(type);
	if (err < 0) {
		eprintf("can't init LCD\n");
		exit(EXIT_FAILURE);
	}

	puts("applications is up and running");

	/*
	 * for testing
	 */
	struct lcd_request req;
	size_t len = sizeof(struct lcd_request);
	memset(&req, 0, len);

        /* dummy waiting after start */
	usleep(500000);

        /* clear display */
	req.line = -1 * LCD_CLEAR;

	printf("sizeof struct lcd_request %d\n", (int) len);
	printf("value to send %s\n", req.str);
	printf("to line: %d\n", req.line);
	printf("to cursor position: %d\n", req.cur_pos);

	err = write(lcd_fd, &req, len);
	if (err != (int) len)
		printf("error ... %d\n", err);

        /* dummy waiting */
	usleep(500000);

	/* send dummy text */
	memset(&req, 0, len);
	req.line = 2;
	req.cur_pos = 3;
	strncpy(req.str, "This is a text, which is to large!", lcd_max_col);

	printf("sizeof struct lcd_request %d\n", (int) len);
	printf("value to send %s\n", req.str);
	printf("to line: %d\n", req.line);
	printf("to cursor position: %d\n", req.cur_pos);

	err = write(lcd_fd, &req, len);
	if (err != (int) len)
		printf("error ... %d\n", err);


	/*
	 * get fifo for the sensor data
	 */
	struct sensor_fifo_req fifo_req;
	len = sizeof(struct sensor_fifo_req);
	memset(&fifo_req, 0, len);

	fifo_req.interval = 1;
	fifo_req.pid = getpid();

	int sensor_fifo = open(SENSOR_FIFO, O_WRONLY);
	if (sensor_fifo < 0) {
		perror("open in main()");
		return -1;;
	}

	err = write(sensor_fifo, &fifo_req, len);
	if (err != (int) len)
		printf("error ... %d\n", err);





	/* dummy waiting */
	usleep(1000000);

	return EXIT_SUCCESS;
}
