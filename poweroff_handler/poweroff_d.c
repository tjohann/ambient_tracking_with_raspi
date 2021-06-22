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

	if (gpio_set_direction(POWEROFF_BUTTON, GPIO_IN ) < 0) {
		eprintf("can't set direction for PIN %d\n", POWEROFF_BUTTON);
		exit(EXIT_FAILURE);
	}

	if (gpio_set_direction(POWER_LED, GPIO_OUT ) < 0) {
		eprintf("can't set direction for PIN %d\n", POWER_LED);
		exit(EXIT_FAILURE);
	}
}


/* the main thread */
void * poweroff_handler(void *arg)
{
	int powerswt_val = -1;
	int powerled_val = -1;
	int ret = -1;

	for (;;) {
		powerled_val = ~powerled_val;

		powerswt_val = gpio_read(POWEROFF_BUTTON);
		printf("value of POWER-SWT %d\n", powerswt_val);

		ret = gpio_write(POWER_LED, powerled_val);
		printf("value of POWER-LED %d ... error val %d\n",
			powerled_val, ret);

		sleep(1);
	}

	return NULL;
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

	pthread_t tid;
	err = pthread_create(&tid, NULL, poweroff_handler, NULL);
	if (err != 0) {
		eprintf("can't create thread\n");
		exit(EXIT_FAILURE);
	}

	puts("daemon is up and running");

	(void) pthread_join(tid, NULL);
	return EXIT_SUCCESS;
}
