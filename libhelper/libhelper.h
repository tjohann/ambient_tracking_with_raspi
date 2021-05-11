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
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>

#define eprintf(format, ...) fprintf (stderr, format, ##__VA_ARGS__)

/* shortcut for old signal api (my_signal()) */
typedef	void sigfunc(int);

/*
 * alloc a string from heap
 */
char * alloc_string(const char *s);

/*
 * become a daemon and setup syslog
 */
int become_daemon(const char* name);

/*
 * signal function similiar to Richard Stevens proposal
 */
sigfunc * my_signal(int signo, sigfunc *func);

#endif
