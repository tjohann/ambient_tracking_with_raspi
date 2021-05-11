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

#include "libhelper.h"
#include "libhelper_private.h"

LIBHELPER_EXPORT char * alloc_string(const char *s)
{
	char *str = NULL;
	size_t len = 0;

	if (s == NULL)
		return NULL;

	len = strlen(s) + 1;

	str = malloc(len);
	if (str == NULL)
		return NULL;

	memset(str, 0, len);
	strncat(str, s, len);

	return str;
}

LIBHELPER_EXPORT sigfunc * my_signal(int signo, sigfunc *func)
{
        struct sigaction actual, old_actual;

        memset(&actual, 0, sizeof(struct sigaction));
        memset(&old_actual, 0, sizeof(struct sigaction));

        actual.sa_handler = func;
        sigemptyset(&actual.sa_mask);
        actual.sa_flags = 0;

        if (signo == SIGALRM)
                ; // do nothing -> we need it for timeout handling
        else
                actual.sa_flags |= SA_RESTART;

        if (sigaction(signo, &actual, &old_actual) < 0) {
                perror("my_signal -> sigaction()");
                return SIG_ERR;
        }

        return old_actual.sa_handler;
}

LIBHELPER_EXPORT int become_daemon(const char* name)
{
	umask(0);

	pid_t pid = fork();
	if (pid == -1) {
		eprintf("Can't fork");
		exit (EXIT_FAILURE);
	}
	if (pid)
		_exit(EXIT_SUCCESS);

	if (setsid() == -1)
		perror("setsid");

	my_signal(SIGHUP, SIG_IGN);

	pid = fork();
	if (pid == -1) {
		perror("Can't fork -> second one");
		return -1;
	}
	if (pid)
		_exit(EXIT_SUCCESS);

	if (chdir("/") == -1) {
		perror("Can't chdir");
		return -1;
	}

	int max_fd = sysconf(_SC_OPEN_MAX);
        if (max_fd == -1)
		max_fd = 1024;

	int i;
        for (i = 0; i < max_fd; i++)
		close(i);

	int fd0 = open("/dev/null", O_RDONLY);
	int fd1 = open("/dev/null", O_RDWR);
	int fd2 = open("/dev/null", O_RDWR);

	openlog(name, LOG_PID | LOG_CONS, LOG_DAEMON);

	if (fd0 != 0 || fd1 != 1 || fd2 != 2)
		syslog(LOG_ERR, "Couldn't open fd's\n");

	return 0;
}
