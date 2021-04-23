#
# my simple makefile act as something like a user interface
#

MODULES_BUILD = lcd2004_i2c sensor_pi
MODULES = pics configs scripts kernel Documentation

all::
	@echo "+-----------------------------------------------------------+"
	@echo "|                                                           |"
	@echo "|                  Nothing to build                         |"
	@echo "|                                                           |"
	@echo "+-----------------------------------------------------------+"
	@echo "| Example:                                                  |"
	@echo "| make build              -> build all code subfolders      |"
	@echo "| make clean              -> clean all dir/subdirs          |"
	@echo "| make install            -> install some scripts to        |"
	@echo "|                            $(HOME)/bin              |"
	@echo "| make uninstall          -> remove scripts from            |"
	@echo "|                            $(HOME)/bin              |"
	@echo "+-----------------------------------------------------------+"

clean::
	rm -f *~ .*~
	for dir in $(MODULES); do (cd $$dir && $(MAKE) $@); done
	for dir in $(MODULES_BUILD); do (cd $$dir && $(MAKE) $@); done

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
