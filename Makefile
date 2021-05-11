#
# my simple makefile act as something like a user interface
#

MODULES = pics configs scripts bin Documentation raspi2 schematics
MODULES_BUILD = libhelper lcd2004_i2c sensor_pi
MODULES_EXTRA = kernel external

all::
	@echo "+-----------------------------------------------------------+"
	@echo "|                                                           |"
	@echo "|                  Nothing to build                         |"
	@echo "|                                                           |"
	@echo "+-----------------------------------------------------------+"
	@echo "| Example:                                                  |"
	@echo "| make build              -> build all code subfolders      |"
	@echo "| make clean              -> clean all dir/subdirs          |"
	@echo "| make distclean          -> clean + kernel folder          |"
	@echo "| make install            -> install some scripts to        |"
	@echo "|                            $(HOME)/bin              |"
	@echo "| make uninstall          -> remove scripts from            |"
	@echo "|                            $(HOME)/bin              |"
	@echo "+-----------------------------------------------------------+"

clean::
	rm -f *~ .*~
	for dir in $(MODULES); do (cd $$dir && $(MAKE) $@); done
	for dir in $(MODULES_BUILD); do (cd $$dir && $(MAKE) $@); done

distclean: clean
	for dir in $(MODULES_EXTRA); do (cd $$dir && $(MAKE) $@); done

install::
	@echo "Nothing to install"

uninstall::
	@echo "Nothing to uninstall"


#
# build code subfolders
#
build::
	@echo "+----------------------------------------------------------+"
	@echo "|                                                          |"
	@echo "|              Build all code subfolder                    |"
	@echo "|                                                          |"
	@echo "+----------------------------------------------------------+"
	for dir in $(MODULES_BUILD); do (cd $$dir && $(MAKE)); done
