#
# build readout_db.c and admin database
#

CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -D__DEBUG__
LDLIBS = -I../libhelper -L../libhelper -lhelper -lsqlite3

PROG = readout_db
OBJ = readout_db.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LDLIBS)

$(PROG): $(OBJ)
	$(CC) -o $@ $^ -lc $(LDLIBS)

install: $(PROG)
	install $(PROG) $(HOME)/bin/$(PROG)

uninstall:
	rm -rf $(HOME)/bin/$(PROG)

deploy: $(PROG)
	install $(PROG) /usr/bin/$(PROG)
	(cp ambient_data.db /var/run/ambient_data.db)

# database specific parts
deploy_db:
	(cp ambient_data.db /var/run/ambient_data.db)

init_db:
	(sqlite3 ambient_data.db < ambient_data.sql)

init_clean_db:
	(sqlite3 ambient_data.db < ambient_data_clean.sql)

# common parts
clean::
	rm -f *~
	rm -f .*~
	rm -rf *.o
	rm -rf $(PROG)

distclean: clean uninstall

.PHONY: clean uninstall init_db init_clean_db deploy_db
