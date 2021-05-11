#
# build libhelper.a
#

CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra
LDLIBS =

LIB = libhelper.a
OBJ = helper.o

%.o: %.c
	$(CC) -g -c -o $@ $< $(CFLAGS) $(LDLIBS)

$(LIB): $(OBJ)
	ar rcs $@ $(OBJ)

install: $(LIB)
	install $(LIB) $(HOME)/lib/$(LIB)

uninstall:
	rm -rf $(HOME)/lib/$(LIB)

clean:
	rm -rf *~
	rm -rf .*~
	rm -rf *.o
	rm -rf $(LIB)

distclean: clean uninstall

.PHONY: clean uninstall
