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
#define FIRST_REG             0x01 /* the address of the first register       */
#define LAST_REG              0x0D /* the address of the last register        */
#define TEMP_REG              0x01 /* ext. temperature [unit:deg celsius]     */
#define LIGHT_REG_L           0x02 /* light brightness low 8 bit [unit:lux]   */
#define LIGHT_REG_H           0x03 /* light brightness high 8 bit [unit:lux]  */
#define STATUS_REG            0x04 /* status function                         */
#define ON_BOARD_TEMP_REG     0x05 /* onboard temperature [unit:deg celsius]  */
#define ON_BOARD_HUMIDITY_REG 0x06 /* onboard humidity [unit:%]               */
#define ON_BOARD_SENSOR_ERROR 0x07 /* 0 -> ok -- 1 -> error                   */
#define BMP280_TEMP_REG       0x08 /* temperature [unit:deg celsius]          */
#define BMP280_PRESSURE_REG_L 0x09 /* pressure low 8 bit [unit:pascal]        */
#define BMP280_PRESSURE_REG_M 0x0A /* pressure mid 8 bit [unit:pascal]        */
#define BMP280_PRESSURE_REG_H 0x0B /* pressure high 8 bit [unit:pascal]       */
#define BMP280_STATUS 	      0x0C /* 0 -> ok -- 1 -> error                   */
#define HUMAN_DETECT          0x0D /* 0 -> no nctive body -- 1 -> active body */

/* position within the value array */
#define VAL_MAX_LEN  7
#define EXT_TEMP     0x00
#define ONBOARD_TEMP 0x01
#define BARO_TEMP    0x02
#define HUMINITY     0x03
#define BRIGHTNESS   0x04
#define PRESSURE     0x05
#define BODY_DETECT  0x06

/* store of the sensor module values */
static int values[VAL_MAX_LEN];

/* status bits */
#define STATE_EXT_TEMP     BIT0
#define STATE_BRIGHTNESS   BIT1
#define STATE_ONBOARD_TEMP BIT2
#define STATE_HUMINITY     BIT3
#define STATE_BARO_TEMP    BIT4
#define STATE_PRESSURE     BIT5

/* the status byte */
static unsigned char sensor_state = 0x3F;

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
	__s32 buf[LAST_REG + 1];  /* ignore buf[0] */
	memset(buf, 0, (LAST_REG + 1) * sizeof(__s32));

	/* ignore buf[0] -> easier to handle */
	for (int i = FIRST_REG; i <= LAST_REG; i++) {
		buf[i] = i2c_smbus_read_byte_data(sensor, i);
		if (buf[i] < 0) {
			syslog(LOG_ERR, "can't read register value");
			return -1;
		}
	}

	/* handle external temp sensor */
	if (buf[STATUS_REG] & 0x01) {
		syslog(LOG_ERR, "external temperature sensor overrange");
		values[EXT_TEMP] = - 0xFF;
		sensor_state |= STATE_EXT_TEMP;
	} else if (buf[STATUS_REG] & 0x02) {
		syslog(LOG_ERR, "no external temperature sensor");
		values[EXT_TEMP] = - 0xFF;
		sensor_state |= STATE_EXT_TEMP;
	} else {
		values[EXT_TEMP] = buf[TEMP_REG];
		sensor_state &= ~STATE_EXT_TEMP;
#ifdef __DEBUG__
		syslog(LOG_INFO, "current external sensor temperature: %d °C",
			buf[TEMP_REG]);
		syslog(LOG_INFO, "current external sensor temperature: %d °C",
			values[EXT_TEMP]);
#endif
	}

	/* handle onboard temp sensor */
	if (buf[ON_BOARD_SENSOR_ERROR] != 0) {
		syslog(LOG_ERR, "onboard temperature sensor out-of-date error");
		values[ONBOARD_TEMP] = - 0xFF;
		sensor_state |= STATE_ONBOARD_TEMP;
	} else {
		values[ONBOARD_TEMP] = buf[ON_BOARD_TEMP_REG];
		sensor_state &= ~STATE_ONBOARD_TEMP;
#ifdef __DEBUG__
		syslog(LOG_INFO, "current onboard sensor temperature: %d °C",
			buf[ON_BOARD_TEMP_REG]);
		syslog(LOG_INFO, "current onboard sensor temperature: %d °C",
			values[ONBOARD_TEMP]);
#endif
	}

	/* handle onboard humidity sensor */
	if (buf[ON_BOARD_SENSOR_ERROR] != 0) {
		syslog(LOG_ERR, "onboard humidity sensor out-of-date error");
		values[HUMINITY] = - 0xFF;
		sensor_state |= STATE_HUMINITY;
	} else {
		values[HUMINITY] = buf[ON_BOARD_HUMIDITY_REG];
		sensor_state &= ~STATE_HUMINITY;
#ifdef __DEBUG__
		syslog(LOG_INFO, "current onboard sensor humidity: %d %%",
			buf[ON_BOARD_HUMIDITY_REG]);
		syslog(LOG_INFO, "current onboard sensor humidity: %d %%",
			values[HUMINITY]);
#endif
	}

        /* handle brighness sensor */
	if (buf[STATUS_REG] & 0x04) {
		syslog(LOG_ERR, "onboard brightness sensor overrange");
		values[BRIGHTNESS] = - 0xFF;
		sensor_state |= STATE_BRIGHTNESS;
	} else if (buf[STATUS_REG] & 0x08) {
		syslog(LOG_ERR, "onboard brightness sensor failure");
		values[BRIGHTNESS] = - 0xFF;
		sensor_state |= STATE_BRIGHTNESS;
	} else {
		values[BRIGHTNESS] = (buf[LIGHT_REG_H] << 8) | buf[LIGHT_REG_L];
		sensor_state &= ~STATE_BRIGHTNESS;
#ifdef __DEBUG__
		syslog(LOG_INFO, "current onboard sensor brightness: %d lux",
			((buf[LIGHT_REG_H] << 8) | buf[LIGHT_REG_L]));
		syslog(LOG_INFO, "current onboard sensor brightness: %d lux",
			values[BRIGHTNESS]);
#endif

	}

	/* handle barometer temp */
	if (buf[BMP280_STATUS] != 0) {
		syslog(LOG_ERR, "onboard barometer sensor error");
		values[BARO_TEMP] = - 0xFF;
		sensor_state |= STATE_BARO_TEMP;
	} else {
		values[BARO_TEMP] = buf[BMP280_TEMP_REG];
		sensor_state &= ~STATE_BARO_TEMP;
#ifdef __DEBUG__
		syslog(LOG_INFO, "current barometer sensor temperature: %d °C",
			buf[BMP280_TEMP_REG]);
		syslog(LOG_INFO, "current barometer sensor temperature: %d °C",
			values[BARO_TEMP]);
#endif
	}

	/* handle barometer pressure */
	if (buf[BMP280_STATUS] != 0) {
		syslog(LOG_ERR, "onboard barometer sensor error");
		values[PRESSURE] = - 0xFF;
		sensor_state |= STATE_PRESSURE;
	} else {
		values[PRESSURE] = (buf[BMP280_PRESSURE_REG_L]
				| buf[BMP280_PRESSURE_REG_M] << 8
				| buf[BMP280_PRESSURE_REG_H] << 16);
		sensor_state &= ~STATE_PRESSURE;
#ifdef __DEBUG__
		syslog(LOG_INFO, "current barometer sensor pressure: %d pascal",
			(buf[BMP280_PRESSURE_REG_L]
				| buf[BMP280_PRESSURE_REG_M] << 8
				| buf[BMP280_PRESSURE_REG_H] << 16 ));
		syslog(LOG_INFO, "current barometer sensor pressure: %d pascal",
			values[PRESSURE]);
#endif
	}

	/* handle body detection */
	if (buf[HUMAN_DETECT] == 1) {
		values[BODY_DETECT] = 1;
#ifdef __DEBUG__
		syslog(LOG_INFO, "body detected within 5 seconds");
#endif
	} else {
		values[BODY_DETECT] = 0;
#ifdef __DEBUG__
		syslog(LOG_INFO, "no body detected");
#endif
	}

#ifdef __DEBUG__
	syslog(LOG_INFO, "the actual sensor state: 0x%x ", sensor_state);
#endif

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
