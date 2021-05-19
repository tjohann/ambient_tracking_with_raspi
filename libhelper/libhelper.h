/*
  GPL
  (c) 2021, thorsten.johannvorderbrueggen@t-online.de

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

#ifndef _LIBHELPER_H_
#define _LIBHELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <linux/i2c-dev.h>
#include <pthread.h>

#define eprintf(format, ...) fprintf (stderr, format, ##__VA_ARGS__)
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FIFOMODE (S_IRUSR | S_IWUSR | S_IWGRP)

#define LCD_FIFO "/var/run/lcd_daemon.fifo"

/* define bit/pin positions to be used */
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

#define MAX_LINE_LCD2004 20
/* the fifo request struct for LCD2004 */
struct lcd_2004_request {
	char str[MAX_LINE_LCD2004 + 1]; /* 20 char per line + \0 */
	char line;    /* 1 ... 4 the line      */
};

#define MAX_LINE_LCD1602 16
/* the fifo request struct for LCD1602 */
struct lcd_1602_request {
	char str[MAX_LINE_LCD1602 + 1]; /* 16 char per line + \0 */
	char line;     /* 1 ... 2 the line     */
};


/* shortcut for old signal api (my_signal()) */
typedef	void sigfunc(int);

/*
 * signal function similiar to Richard Stevens proposal
 */
sigfunc * my_signal(int signo, sigfunc *func);

char * alloc_string(const char *s);
int become_daemon(const char* name);
int set_cloexec(int fd);
int lock_file(int fd);
int already_running(const char* lockfile);
int init_i2c_device(char *adapter, unsigned char addr);
int create_read_fifo(char *name);
int create_write_fifo(char *name);
void clr_flag(int fd, int flags);

#endif
