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
#include <i2c/smbus.h>
#include <pthread.h>

#define eprintf(format, ...) fprintf (stderr, format, ##__VA_ARGS__)
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FIFOMODE (S_IRUSR | S_IWUSR | S_IWGRP)

#define LCD_FIFO "/var/run/lcd_daemon.fifo"
#define SENSOR_FIFO "/var/run/sensor_daemon.fifo"

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

/* define supported LCD types */
#define LCD1602 1
#define LCD2004 2

/* LCD 2004 specific defines */
#define LCD2004_MAX_COL 20
#define LCD2004_MAX_LINE 4

/* LCD 1602 specific defines */
#define LCD1602_MAX_COL 16
#define LCD1602_MAX_LINE 2

/* define LCD cmd's */
#define LCD_CLEAR BIT0
#define LCD_HOME  BIT1

/*
 * the lcd request struct
 * note: if line > 0  -> str contains the text
 *       if line < 0 -> cmd negativ of LCD_CLEAR
 */
struct lcd_request {
	signed char line;
	char cur_pos;
	char str[LCD2004_MAX_COL + 1];
};

/* docker-pi specific defines */
struct sensor_data {
	char dummy;               /* dummy value         */
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
