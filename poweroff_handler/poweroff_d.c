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
#include <sys/inotify.h>

/* the lcd ctrl fifo */
static int lcd_ctrl_fd = -1;

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

	if (lcd_ctrl_fd > 0)
		close(lcd_ctrl_fd);

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

	if (gpio_set_edge_falling(POWEROFF_BUTTON) < 0) {
		eprintf("can't set falling edge for PIN %d\n", POWEROFF_BUTTON);
		exit(EXIT_FAILURE);
	}
}

static int init_lcd(void)
{
	lcd_ctrl_fd = open(LCD_FIFO, O_WRONLY);
	if (lcd_ctrl_fd < 0) {
		perror("open in main()");
		return -1;;
	}

	return 0;
}

static void clear_lcd(void)
{
	struct lcd_ctrl_request req;
	size_t len = sizeof(struct lcd_ctrl_request);
	memset(&req, 0, len);

	req.cmd = ~(LCD_CLEAR);
	(void) write(lcd_ctrl_fd, &req, len); /* ignore error */
}

/* the main thread */
void * poweroff_handler(__attribute__((__unused__)) void *arg)
{
	int num_read = -1;
	struct inotify_event *event;

	int fd = inotify_init();
	if (fd < 0) {
		perror("cant init inotify");
		exit(EXIT_FAILURE);
	}

#define PATH_MAX_LEN 30
	char path[PATH_MAX_LEN];
	memset(&path, 0, PATH_MAX_LEN);

	snprintf(path, PATH_MAX_LEN, GPIO_VALUE_PATH, POWEROFF_BUTTON);
#ifdef __DEBUG__
	printf("inotify path: %s\n", path);
#endif
	int wd = inotify_add_watch(fd, path, IN_ALL_EVENTS);
	if (wd < 0) {
		perror("cant add watch");
		exit(EXIT_FAILURE);
	}
#define BUF_LEN (sizeof(struct inotify_event))
	char buf[BUF_LEN];
	memset(&buf, 0, BUF_LEN);

	for (;;) {
		gpio_write(POWER_LED, 0);  /* clear the LED */

		num_read = read(fd, buf, BUF_LEN);
		if (num_read == 0) {
			eprintf("inotify'd read return 0 -> ignore it\n");
			continue;
		}

		if (num_read < 0) {
			perror("cant read inotify watch");
			exit(EXIT_FAILURE);
		}

		event = (struct inotify_event *) &buf;
		if (event->mask & IN_MODIFY) {
#ifdef __DEBUG__
			printf("file event IN_MODIFY\n");
#endif
		} else {
#ifdef __DEBUG__
			printf("other file event -> ignore it\n");
#endif
			continue;
		}

		printf("shut down the system in 5 seconds\n");
		(void) gpio_write(POWER_LED, 1); /* only an indication */

		clear_lcd();
		sleep(5);

		/* use runit to halt device */
		//system("init 0");
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

	err = init_lcd();
	if (err < 0) {
		eprintf("can't init LCD\n");
		exit(EXIT_FAILURE);
	}

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
