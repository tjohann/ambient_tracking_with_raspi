#
# my simple makefile act as something like a user interface
#

all::
	@echo Cheers

clean::
	rm -f *~
	rm -f .*~
	rm -rf modules_*
	(cd linux && make clean)

distclean: clean
	(cd linux && make clean)
	rm -rf modules_*

clone_kernel::
	(git clone --depth=1 https://github.com/raspberrypi/linux)
