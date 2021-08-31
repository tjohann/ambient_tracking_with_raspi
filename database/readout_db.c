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

/* the database */
sqlite3 *db;

#define SQL_SELECT_STRING "SELECT * from AmbientValues"
#define SQL_SEL_STR_LEN 30

extern char *__progname;
bool run_as_daemon = false;

static void
__attribute__((noreturn)) usage(void)
{
	putchar('\n');
	fprintf(stdout, "Usage: ./%s -[h]                                  \n",
		__progname);
	fprintf(stdout, "       -h            -> show this help            \n");
	putchar('\n');
	fprintf(stdout, "Example: readout all values of database           \n");
	fprintf(stdout, "        ./%s                                      \n",
		__progname);

	exit(EXIT_FAILURE);
}

static void cleanup(void)
{
	sqlite3_close(db);

	fprintf(stdout, "application is down -> bye\n");
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


int print_db(__attribute__((__unused__)) void *dummy,
	int argc, char **argv,
	char **azColName)
{
    dummy = 0;

    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    return 0;
}


int main(int argc, char *argv[])
{

	int c;
	while ((c = getopt(argc, argv, "h")) != -1) {
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

	err = init_database();
	if (err < 0) {
		eprintf("can't init database\n");
		exit(EXIT_FAILURE);
	}

	char *err_msg = NULL;
	err = sqlite3_exec(db, SQL_SELECT_STRING, print_db, 0, &err_msg);
	if (err != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
