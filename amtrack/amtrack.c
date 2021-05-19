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

extern char *__progname;

/* common functions */


static void
__attribute__((noreturn)) usage(void)
{
	putchar('\n');
	fprintf(stdout, "Usage: ./%s -[h]                    \n", __progname);
	fprintf(stdout, "       -h            -> show this help          \n");
	putchar('\n');
	fprintf(stdout, "Example:                                        \n");
	fprintf(stdout, "        ./%s                        \n", __progname);

	exit(EXIT_FAILURE);
}

static void cleanup(void)
{
	if (lcd_fd > 0)
		close(lcd_fd);

	fprintf(stdout, "application is down -> bye");
}



int main(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;
		default:
			eprintf("ERROR -> no valid argument\n");
			usage();
		}
	}


	/*
	 * check kdo arguments
	 * ...
	 */

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	/*
	 * do the "rest"
	 */

	puts("applications is up and running");


	lcd_fd = open(LCD_FIFO, O_WRONLY);
	if (lcd_fd < 0) {
		perror("open in main()");
		exit(EXIT_FAILURE);
	}

	(void) write_lcd_fifo(lcd_fd, LCD2004, 2, 5, "this is a test");

	/* dummy waiting */
	usleep(10000000);

	return EXIT_SUCCESS;
}
