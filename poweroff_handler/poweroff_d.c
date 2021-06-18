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

extern char *__progname;

static void
__attribute__((noreturn)) usage(void)
{
	putchar('\n');
	fprintf(stdout, "Usage: ./%s -[h]                              \n",
		__progname);
	fprintf(stdout, "       -h            -> show this help        \n");
	putchar('\n');
	fprintf(stdout, "Example:                                      \n");
	fprintf(stdout, "        ./%s                                  \n",
		__progname);

	exit(EXIT_FAILURE);
}

static void cleanup(void)
{
	(void) gpio_unexport(POWEROFF_BUTTON);
	(void) gpio_unexport(POWER_LED);

	puts("daemon is down -> bye");
}


static void init_pins(void)
{
	if (gpio_export(POWEROFF_BUTTON ) < 0) {
		eprintf("can't export PIN %d\n", POWEROFF_BUTTON);
		exit(EXIT_FAILURE);
	}

	if (gpio_export(POWER_LED ) < 0) {
		eprintf("can't export PIN %d\n", POWER_LED);
		exit(EXIT_FAILURE);
	}




}


int main(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "a")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;
		default:
			eprintf("ERROR -> no valid argument\n");
			usage();
		}
	}

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	init_pins();

	/*
	  pthread_t tid;
	  err = pthread_create(&tid, NULL, read_sensor, NULL);
	  if (err != 0) {
	  eprintf("can't create thread\n");
	  exit(EXIT_FAILURE);
	  }
	*/

	puts("daemon is up and running");

	sleep(60);
	return EXIT_SUCCESS;
}
