#
# build libhelper.a
#

CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -D__DEBUG__
LDLIBS = -li2c

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

deploy:
	@echo "Nothing to do"

clean:
	rm -rf *~
	rm -rf .*~
	rm -rf *.o
	rm -rf $(LIB)

distclean: clean uninstall deploy

.PHONY: clean uninstall
