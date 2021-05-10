#
# my simple makefile act as something like a user interface
#

all::
	@echo Cheers

clean::
	rm -f *~
	rm -f .*~
ifneq ($(wildcard ./linux),)
	(cd linux && make clean)
endif

distclean: clean
	rm -rf modules_*

get_kernel::
	(git clone --depth=1 https://github.com/raspberrypi/linux)

update_all::
ifneq ($(wildcard ./linux),)
	(cd linux && git pull)
endif
