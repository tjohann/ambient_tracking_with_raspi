/*
 * initial version to learn how to get the values of the
 * sensorpi modul (see https://wiki.52pi.com for more details)
 * function:
 *
 * - ...
 * - ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c-dev.h>


int main(void)
{
	puts("\nHello env_sensor modul!\n");

	return EXIT_SUCCESS;
}
