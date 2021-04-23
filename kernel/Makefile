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

clone_kernel::
	(git clone --depth=1 https://github.com/raspberrypi/linux)
