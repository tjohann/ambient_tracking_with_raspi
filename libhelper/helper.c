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
                perror("sigaction in my_sginal()");
                return SIG_ERR;
        }

        return old_actual.sa_handler;
}

LIBHELPER_EXPORT int set_cloexec(int fd)
{
	int err = fcntl(fd, F_GETFD, 0);
	if (err == -1) {
		perror("fcntl in set_cloexec()");
		return -1;
	}

	int new = err | FD_CLOEXEC;

	if (fcntl(fd, F_SETFD, new) == -1) {
		perror("fcntl in set_cloexec() -> second call");
		return -1;
	}

	return 0;
}

LIBHELPER_EXPORT int lock_file(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLK, &fl) == -1) {
		perror("fcntl in lock_file()");
		return -1;
	}

	return 0;
}

LIBHELPER_EXPORT int already_running(const char* lockfile)
{
	int fd = open(lockfile, O_RDWR | O_CREAT, LOCKMODE);
	if (fd < 0) {
		perror("open in already_running()");
		return -1;
	}

	if (lock_file(fd) < 0) {
		if ((errno == EAGAIN) || (errno == EACCES)) {
			close(fd);
			return 1;
		}
		eprintf("can`t lock lockfile in already_running()\n");
		return -1;
	}

	if (ftruncate(fd, 0) < 0) {
		perror("ftruncate in already_running()");
		return -1;
	}

#define MAX 16
	char buf[MAX];
	memset(buf, 0, MAX);
	snprintf(buf, MAX, "%ld", (long) getpid());

	if (write(fd, buf, strlen(buf) + 1) < 0 ){
		perror("write in already_running()");
		return -1;
	}

	return 0;
}

LIBHELPER_EXPORT int become_daemon(const char* name)
{
	umask(0);

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork in become_daemon()");
		exit (EXIT_FAILURE);
	}
	if (pid)
		_exit(EXIT_SUCCESS);

	if (setsid() == -1)
		perror("setsid in become_daemon()");

	(void) my_signal(SIGHUP, SIG_IGN);

	pid = fork();
	if (pid == -1) {
		perror("fork in become_daemon() -> second call");
		return -1;
	}
	if (pid)
		_exit(EXIT_SUCCESS);

	if (chdir("/") == -1) {
		perror("chdir in become_daemon()");
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
		syslog(LOG_ERR, "fd0...fd2 not correct in become_daemon()\n");

	return 0;
}

LIBHELPER_EXPORT int init_i2c_device(char *adapter, unsigned char addr)
{
	int fd = open(adapter, O_RDWR);
	if (fd < 0) {
		perror("open in init_i2c_device()");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		perror("ioctl in init_i2c_device()");
		return -1;
	}

	return fd;
}

LIBHELPER_EXPORT int create_read_fifo(char *name)
{
	umask(0);

	if ((mkfifo(name, FIFOMODE) < 0) && (errno != EEXIST)) {
		perror("can't make fifo in create_fifo()");
		return -1;
	}

	int fd = open(name, O_RDONLY);
	if (fd < 0) {
		perror("open in create_read_fifo()");
		return -1;
	}

	return fd;
}

LIBHELPER_EXPORT int create_write_fifo(char *name)
{
	umask(0);

	if ((mkfifo(name, FIFOMODE) < 0) && (errno != EEXIST)) {
		perror("can't make fifo in create_fifo()");
		return -1;
	}

	int fd = open(name, O_WRONLY);
	if (fd < 0) {
		perror("open in create_read_fifo()");
		return -1;
	}

	return fd;
}
