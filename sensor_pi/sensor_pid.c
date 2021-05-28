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

/*
 * !!!! ACTUAL state of development !!!!
 *
 * initial version to learn how to get the values of the
 * sensorpi modul (see https://wiki.52pi.com for more details)
 * function:
 *
 * - ...
 * - ...
 */

#include <libhelper.h>

/* setup time */
#define SETUP_TIME 10000

#define LOCKFILE "/var/run/sensor_daemon.pid"
#define DAEMON_FIFO "/var/run/sensor_daemon.fifo"

/*
 * the different register addresses
 * (see https://wiki.52pi.com for more details)
 */
#define FIRST_REG             0x01 /* the address of the first register */
#define LAST_REG              0x0D /* the address of the last register */
#define TEMP_REG              0x01 /* Ext. Temperature [Unit:degC] */
#define LIGHT_REG_L           0x02 /* Light Brightness Low 8 Bit [Unit:Lux] */
#define LIGHT_REG_H           0x03 /* Light Brightness High 8 Bit [Unit:Lux] */
#define STATUS_REG            0x04 /* Status Function */
#define ON_BOARD_TEMP_REG     0x05 /* OnBoard Temperature [Unit:degC] */
#define ON_BOARD_HUMIDITY_REG 0x06 /* OnBoard Humidity [Uinit:%] */
#define ON_BOARD_SENSOR_ERROR 0x07 /* 0(OK) - 1(Error) */
#define BMP280_TEMP_REG       0x08 /* P. Temperature [Unit:degC] */
#define BMP280_PRESSURE_REG_L 0x09 /* P. Pressure Low 8 Bit [Unit:Pa] */
#define BMP280_PRESSURE_REG_M 0x0A /* P. Pressure Mid 8 Bit [Unit:Pa] */
#define BMP280_PRESSURE_REG_H 0x0B /* P. Pressure High 8 Bit [Unit:Pa] */
#define BMP280_STATUS 	      0x0C /* 0(OK) - 1(Error) */
#define HUMAN_DETECT          0x0D /* 0(No Active Body) - 1(Active Body)  */

/* the sensor pi module */
static int sensor = -1;

extern char *__progname;

/* common functions */
int init_sensor_hub(char *adapter, unsigned char addr);

static void
__attribute__((noreturn)) usage(void)
{
	putchar('\n');
	fprintf(stdout, "Usage: ./%s -[hi:a:]                          \n",
		__progname);
	fprintf(stdout, "       -i /dev/i2c-X -> I2C adapter           \n");
	fprintf(stdout, "       -a 17         -> I2C address (in hex)  \n");
	fprintf(stdout, "       -h            -> show this help        \n");
	putchar('\n');
	fprintf(stdout, "Example:                                      \n");
	fprintf(stdout, "        ./%s -i /dev/i2c-1 -a 17              \n",
		__progname);

	exit(EXIT_FAILURE);
}

static void cleanup(void)
{
	syslog(LOG_INFO, "daemon is down -> bye");
}

static void daemon_handling(void)
{
	if (become_daemon(__progname) < 0) {
		eprintf("ERROR: can't become a daemon\n");
		exit(EXIT_FAILURE);
	}

	int err = already_running(LOCKFILE);
	if (err == 1) {
		syslog(LOG_ERR, "i'm already running");
		exit(EXIT_FAILURE);
	} else if (err < 0) {
		syslog(LOG_ERR, "can't setup lockfile");
		exit(EXIT_FAILURE);
	}
}

int get_values(void)
{
	__s32 buf[LAST_REG + 1]; /* ignore buf[0] */
	memset(buf, 0, (LAST_REG + 1) * sizeof(__s32));

	for (int i = FIRST_REG; i <= LAST_REG; i++) {
		buf[i] = i2c_smbus_read_byte_data(sensor, i);
		if (buf[i] < 0) {
			syslog(LOG_ERR, "can't read register value");
			return -1;
		}

		syslog(LOG_INFO, "value of register: %d", buf[i]);
	}

	/* handle external temp sensor */
	if (buf[STATUS_REG] & 0x01) {
		syslog(LOG_ERR, "external temperature sensor overrange");
	} else if (buf[STATUS_REG] & 0x02) {
		syslog(LOG_ERR, "no external temperature sensor");
	} else {
		syslog(LOG_INFO, "current external sensor temperature: %d cels",
			buf[TEMP_REG]);
	}

	return 0;
}

int init_sensor_hub(char *adapter, unsigned char addr)
{
	sensor = init_i2c_device(adapter, addr);
	if (sensor < 0) {
		eprintf("ERROR: can't init Sensor-Hub");
		return -1;
	}

	usleep(SETUP_TIME);
	return 0;
}


int main(int argc, char *argv[])
{
	char *adapter = NULL;
	unsigned char addr = 0x00;

	int c;
	while ((c = getopt(argc, argv, "a:hi:")) != -1) {
		switch (c) {
		case 'i':
			adapter = optarg;
			break;
		case 'a':
			addr = (unsigned char) strtoul(optarg, NULL, 16);
			break;
		case 'h':
			usage();
			break;
		default:
			eprintf("ERROR -> no valid argument\n");
			usage();
		}
	}

	if ((adapter == NULL) || (addr == 0))
		usage();

	fprintf(stdout, "try to open %s@0x%x\n", adapter, (int) addr);

	if (access(adapter, R_OK | W_OK) == -1 ) {
		perror("ERROR: can't access /dev/YOUR_PROVIDED_I2C_ADAPTER");
		usage();
	}

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	daemon_handling();

	err = init_sensor_hub(adapter, addr);
	if (err < 0) {
		syslog(LOG_ERR, "can't init sensor hub");
		exit(EXIT_FAILURE);
	}

	err = get_values();
	if (err < 0) {
		syslog(LOG_ERR, "can't get state");
		exit(EXIT_FAILURE);
	}

        /*
	 * do the sensor stuff
	 */

	syslog(LOG_INFO, "daemon is up and running");

	usleep(10000000);
	return EXIT_SUCCESS;
}
