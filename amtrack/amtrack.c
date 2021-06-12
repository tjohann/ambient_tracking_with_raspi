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
#include <sqlite3.h>
#include <time.h>

/* the write to lcd_daemon fifo */
static int lcd_fd = -1;

/* the lcd type info */
static unsigned char lcd_type = 0;
static unsigned char lcd_max_line = 0;
static unsigned char lcd_max_col = 0;
typedef void * (*func_lcd_handler_t) (void *);
func_lcd_handler_t lcd_handling;

/* the fifo, to read the sensor data from */
static char sensor_client_fifo[MAX_LEN_FIFO_NAME];
static int sensor_fd = -1;

static char *values[VAL_MAX_LEN];

/* the database */
sqlite3 *db;

#define SQL_INSERT_STRING "INSERT INTO AmbientValues VALUES(%ld, %d, %d, %d, %d, %d, %d);"
#define SQL_INS_STR_LEN 100

extern char *__progname;

#define LCD_WRITE() do {						\
		if (write(lcd_fd, &req, len) != (int) len) {		\
			perror("len of data request not valid -> ignore it"); \
			return -1;					\
		}							\
	} while(0)

#define LCD_WRITE_2() do {						\
		if (write(lcd_fd, &req, len) != (int) len)		\
			perror("len of data request not valid -> ignore it"); \
	} while(0)


/* common functions/threads */
void * lcd2004_handling(void *arg);
void * lcd1602_handling(void *arg);

static void
__attribute__((noreturn)) usage(void)
{
	putchar('\n');
	fprintf(stdout, "Usage: ./%s -[h]                                  \n",
		__progname);
	fprintf(stdout, "       -t 2          -> LCD1602(:1)/LCD2004(:2)   \n");
	fprintf(stdout, "       -h            -> show this help            \n");
	putchar('\n');
	fprintf(stdout, "Example: LCD size 20x4                            \n");
	fprintf(stdout, "        ./%s -t 2                                 \n",
		__progname);

	exit(EXIT_FAILURE);
}

static void cleanup(void)
{
	sqlite3_close(db);

	if (lcd_fd > 0)
		close(lcd_fd);

	unlink(sensor_client_fifo);

	fprintf(stdout, "application is down -> bye\n");
}

static int init_lcd(int type)
{
	lcd_fd = open(LCD_FIFO, O_WRONLY);
	if (lcd_fd < 0) {
		perror("open in main()");
		return -1;;
	}

	struct lcd_request req;
	size_t len = sizeof(struct lcd_request);
	memset(&req, 0, len);

	/* clear display */
	req.line = -1 * LCD_CLEAR;
	LCD_WRITE();

	switch(type) {
	case 1:
		lcd_type = LCD1602;
		lcd_max_line = LCD1602_MAX_LINE;
		lcd_max_col = LCD1602_MAX_COL;

		lcd_handling = lcd1602_handling;

		memset(&req, 0, len);
		req.line = 1;
		strncpy(req.str, "MY AMBIENT", lcd_max_col);
		LCD_WRITE();

		memset(&req, 0, len);
		req.line = 2;
		strncpy(req.str, "       - TRACKER", lcd_max_col);
		LCD_WRITE();

		break;
	case 2:
		lcd_type = LCD2004;
		lcd_max_line = LCD2004_MAX_LINE;
		lcd_max_col = LCD2004_MAX_COL;

		lcd_handling = lcd2004_handling;

		memset(&req, 0, len);
		req.line = 2;
		strncpy(req.str, "MY AMBIENT", lcd_max_col);
		LCD_WRITE();

		memset(&req, 0, len);
		req.line = 3;
		strncpy(req.str, "           - TRACKER", lcd_max_col);
		LCD_WRITE();

		break;
	default:
		eprintf("LCD is not supported!\n");
		return -1;
	}

	for(int i = 0; i < VAL_MAX_LEN; i++)
		values[i] = alloc_string_2(lcd_max_col);

	return 0;
}

static int lcd_write_string(unsigned char line, unsigned char pos)
{
	struct lcd_request req;
	size_t len = sizeof(struct lcd_request);
	memset(&req, 0, len);

	req.line = line;
	strncpy(req.str, values[pos], lcd_max_col);
	LCD_WRITE();

	return 0;
}

static int lcd_clear(void)
{
	struct lcd_request req;
	size_t len = sizeof(struct lcd_request);
	memset(&req, 0, len);

	memset(&req, 0, len);
	req.line = -1 * LCD_CLEAR;
	LCD_WRITE();

	return 0;
}

static int init_database(void)
{
	sqlite3_stmt *res;

	int rc = sqlite3_open(AMBIENT_DATABASE, &db);
	if (rc != SQLITE_OK)
		goto error;

#ifdef __DEBUG__
	rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);
	if (rc != SQLITE_OK)
		goto error;

	rc = sqlite3_step(res);
	if (rc == SQLITE_ROW)
		fprintf(stdout, "use SQLite version: %s\n",
			sqlite3_column_text(res, 0));

	sqlite3_finalize(res);
#endif

	return 0;
error:
	fprintf(stderr, "error in database handling: %s\n",
		sqlite3_errmsg(db));

	sqlite3_close(db);
	return 1;
}

static int init_sensor(void)
{
	struct sensor_fifo_req fifo_req;
	size_t len = sizeof(struct sensor_fifo_req);
	memset(&fifo_req, 0, len);

	fifo_req.interval = 1;
	fifo_req.pid = getpid();

	/* open the sensor server fd to setup my client fifo */
	int fd = open(SENSOR_FIFO, O_WRONLY);
	if (fd < 0)
		goto error;

	if (write(fd, &fifo_req, len) != (int) len)
		goto error;

	sleep(1);

	snprintf(sensor_client_fifo, MAX_LEN_FIFO_NAME, SENSOR_CLIENT_FIFO, getpid());
#ifdef __DEBUG__
	fprintf(stdout, "sensor_client_fifo name: %s\n", sensor_client_fifo);
#endif

	sensor_fd = open(sensor_client_fifo, O_RDONLY);
	if (fd < 0)
		goto error;

	return 0;
error:
	perror("error in init_sensor()");

	close(fd);
	return -1;
}

/* the lcd2004 print thread */
void * lcd2004_handling(void *arg)
{
	struct lcd_request req;
	size_t len = sizeof(struct lcd_request);
	memset(&req, 0, len);

	sleep(10);

	for (;;) {
		/* first circle */
		(void) lcd_clear();
		(void) lcd_write_string(1, BARO_TEMP);
		(void) lcd_write_string(2, PRESSURE);
		(void) lcd_write_string(3, BRIGHTNESS);
		(void) lcd_write_string(4, HUMINITY);
		sleep(10);

                /* second circle */
		(void) lcd_clear();
		(void) lcd_write_string(1, ONBOARD_TEMP);
		(void) lcd_write_string(2, BARO_TEMP);
		(void) lcd_write_string(3, EXT_TEMP);
		(void) lcd_write_string(4, BODY_DETECT);
		sleep(10);
	}

	return NULL;
}

/* the lcd1602 print thread */
void * lcd1602_handling(void *arg)
{
	struct lcd_request req;
	size_t len = sizeof(struct lcd_request);
	memset(&req, 0, len);

	sleep(10);

	for (;;) {
		/* first circle */
		(void) lcd_clear();
		(void) lcd_write_string(1, BARO_TEMP);
		(void) lcd_write_string(2, PRESSURE);
		sleep(10);

		 /* second circle */
		(void) lcd_clear();
		(void) lcd_write_string(1, BRIGHTNESS);
		(void) lcd_write_string(2, HUMINITY);
		sleep(10);

                /* third circle */
		(void) lcd_clear();
		(void) lcd_write_string(1, ONBOARD_TEMP);
		(void) lcd_write_string(2, BARO_TEMP);
		sleep(10);

                /* fourth circle */
		(void) lcd_clear();
		(void) lcd_write_string(1, EXT_TEMP);
		(void) lcd_write_string(2, BODY_DETECT);
		sleep(10);
	}

	return NULL;
}


/* the main thread */
void * ambient_handling(void *arg)
{
	struct sensor_data data;
	size_t len = sizeof(struct sensor_data);
	memset(&data, 0, len);

	char *err_msg = NULL;
	time_t t;

	/*
	 * example string to put into the database:
	 * "INSERT INTO AmbientValues VALUES(1623141708, 20, 21, 22, 123456, 12, 34);"
	 *
	 * format of database:
	 * DATE INTEGER PRIMARY KEY
	 * EXT_TEMP INTEGER
	 * BARO_TEMP INTEGER
	 * ONBOARD_TEMP INTEGER
	 * PRESSURE INTEGER
	 * BRIGHTNESS INTEGER
	 * HUMINITY INTEGER
	 *
	 * the length of sql[] should be with 100 large enough
	 */
	char sql[SQL_INS_STR_LEN];
	memset(&sql, 0, SQL_INS_STR_LEN);

	int err = -1;
	for (;;) {
		err = read(sensor_fd, &data, len);
		if (err != (int) len) {
			if (err == -1 && errno == EBADF) {
				perror("can`t read from sensor fifo!");
				exit(EXIT_FAILURE); /* the only useful action */
			}

			perror("len of data request not valid -> ignore it");
			continue;
 		}

		for(int i = 0; i < VAL_MAX_LEN; i++)
			memset(values[i], 0, lcd_max_col);

		if (lcd_type == LCD2004) {
			snprintf(values[EXT_TEMP], lcd_max_col + 1,
				"Ext. Temp: %d°C", data.ext_temp);

			snprintf(values[BARO_TEMP], lcd_max_col + 1,
				"Baro Temp: %d°C", data.baro_temp);

			snprintf(values[ONBOARD_TEMP], lcd_max_col + 1,
				"Onboard: %d°C", data.onboard_temp);

			snprintf(values[PRESSURE], lcd_max_col + 1,
				"Pressure: %dPa", data.pressure);

			snprintf(values[BRIGHTNESS], lcd_max_col + 1,
				"Brightness: %dLux", data.brightness);

			snprintf(values[HUMINITY], lcd_max_col + 1,
				"Huminity: %d%%", data.huminity);

			snprintf(values[BODY_DETECT], lcd_max_col + 1,
				"Body detect?: %s",
				data.body_detect ? "yes" : "no");
		} else if (lcd_type == LCD1602) {
			snprintf(values[EXT_TEMP], lcd_max_col + 1,
				"Ext. T.: %d degC", data.ext_temp);

			snprintf(values[BARO_TEMP], lcd_max_col + 1,
				"Baro T.: %d degC", data.baro_temp);

			snprintf(values[ONBOARD_TEMP], lcd_max_col + 1,
				"Onb. T.: %d degC", data.onboard_temp);

			snprintf(values[PRESSURE], lcd_max_col + 1,
				"Pres.: %d Pa", data.pressure);

			snprintf(values[BRIGHTNESS], lcd_max_col + 1,
				"Bright. : %d Lux", data.brightness);

			snprintf(values[HUMINITY], lcd_max_col + 1,
				"Huminity: %d%%", data.huminity);

			snprintf(values[BODY_DETECT], lcd_max_col + 1,
				"Body det. ? : %s",
				data.body_detect ? "yes" : "no");
		} else {
			eprintf("LCD is not supported!\n");
			continue;
		}

		t = time(NULL);
		snprintf(sql, SQL_INS_STR_LEN, SQL_INSERT_STRING,
			t,
			data.ext_temp,
			data.baro_temp,
			data.onboard_temp,
			data.pressure,
			data.brightness,
			data.huminity);

		err = sqlite3_exec(db, sql, 0, 0, &err_msg);
		if (err != SQLITE_OK ) {
			fprintf(stderr, "SQL error: %s\n", err_msg);
			sqlite3_free(err_msg);

			continue;
		}

#ifdef __DEBUG__
		printf("external temp: %d\n", data.ext_temp);
		printf("onboard temp: %d\n", data.onboard_temp);
		printf("baro temp: %d\n", data.baro_temp);
		printf("huminity: %d\n", data.huminity);
		printf("brightness: %d\n", data.brightness);
		printf("pressure: %d\n", data.pressure);
		printf("body_detect: %d\n", data.body_detect);

		printf("%s\n", values[EXT_TEMP]);
		printf("%s\n", values[BARO_TEMP]);
		printf("%s\n", values[ONBOARD_TEMP]);
		printf("%s\n", values[PRESSURE]);
		printf("%s\n", values[BRIGHTNESS]);
		printf("%s\n", values[HUMINITY]);
		printf("%s\n", values[BODY_DETECT]);

		printf("%s\n", sql);
#endif
		memset(&data, 0, len);
		memset(&sql, 0, SQL_INS_STR_LEN);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	int type = -1;

	int c;
	while ((c = getopt(argc, argv, "ht:")) != -1) {
		switch (c) {
		case 't':
			type = atoi(optarg);
			break;
		case 'h':
			usage();
			break;
		default:
			eprintf("ERROR -> no valid argument\n");
			usage();
		}
	}

	if (type == -1)
		usage();

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	err = init_database();
	if (err < 0) {
		eprintf("can't init database\n");
		exit(EXIT_FAILURE);
	}

	err = init_lcd(type);
	if (err < 0) {
		eprintf("can't init LCD\n");
		exit(EXIT_FAILURE);
	}

	err = init_sensor();
	if (err < 0) {
		eprintf("can't init sensor hub\n");
		exit(EXIT_FAILURE);
	}

	pthread_t tid;
	err = pthread_create(&tid, NULL, lcd_handling, NULL);
	if (err != 0) {
		eprintf("can't create thread\n");
		exit(EXIT_FAILURE);
	}
	err = pthread_detach(tid);
	if (err != 0)
		eprintf("can't detach thread -> ignore it\n");

	err = pthread_create(&tid, NULL, ambient_handling, NULL);
	if (err != 0) {
		eprintf("can't create thread\n");
		exit(EXIT_FAILURE);
	}

	puts("applications is up and running");

	(void) pthread_join(tid, NULL);
	return EXIT_SUCCESS;
}
