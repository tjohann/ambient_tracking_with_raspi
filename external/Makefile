#
# my simple makefile act as something like a user interface
#

all::
	@echo Cheers

clean::
	rm -f *~
	rm -f .*~
ifneq ($(wildcard ./libbaalue),)
	(cd libbaalue && make clean)
endif

distclean: clean


get_libbaalue::
	(git clone https://github.com/tjohann/libbaalue.git)
