#
# build sensor_pid.c
#

CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra
LDLIBS = -I../libhelper -L../libhelper -lhelper

PROG = sensor_pid
OBJ = sensor_pid.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LDLIBS)

$(PROG): $(OBJ)
	$(CC) -o $@ $^ -lc $(LDLIBS)

install: $(PROG)
	install $(PROG) $(HOME)/bin/$(PROG)

uninstall:
	rm -rf $(HOME)/bin/$(PROG)

clean:
	rm -rf *~ .*~
	rm -rf *.o
	rm -rf $(PROG)

distclean: clean uninstall

.PHONY: clean uninstall
