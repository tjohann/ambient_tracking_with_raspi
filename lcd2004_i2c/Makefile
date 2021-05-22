#
# build pcf8574_lcdd.c
#

CC = gcc
CFLAGS =  -std=gnu11 -Wall -Wextra -D__DEBUG__
LDLIBS = -I../libhelper -L../libhelper -lhelper -pthread

PROG = pcf8574_lcdd
OBJ = pcf8574_lcdd.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LDLIBS)

$(PROG): $(OBJ)
	$(CC) -o $@ $^ -lc $(LDLIBS)

install: $(PROG)
	install $(PROG) $(HOME)/bin/$(PROG)

deploy: $(PROG)
	install $(PROG) /usr/bin/$(PROG)

uninstall:
	rm -rf $(HOME)/bin/$(PROG)

clean:
	rm -rf *~ .*~
	rm -rf *.o
	rm -rf $(PROG)

distclean: clean uninstall

.PHONY: clean uninstall
