#
# my simple makefile act as something like a user interface
#

MODULES = pics configs scripts bin Documentation raspi2 schematics etc database
MODULES_BUILD = libhelper lcd2004_i2c sensor_pi amtrack database poweroff_handler
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
	@echo "| make install            -> install all to $(HOME)/... "
	@echo "| make uninstall          -> remove all from $(HOME)/..."
	@echo "| make deploy             -> install bin's to /usr/bin      |"
	@echo "+-----------------------------------------------------------+"

clean::
	rm -f *~ .*~
	for dir in $(MODULES); do (cd $$dir && $(MAKE) $@); done
	for dir in $(MODULES_BUILD); do (cd $$dir && $(MAKE) $@); done

distclean: clean
	for dir in $(MODULES_EXTRA); do (cd $$dir && $(MAKE) $@); done

install::
	for dir in $(MODULES_BUILD); do (cd $$dir && $(MAKE) $@); done

uninstall::
	for dir in $(MODULES_BUILD); do (cd $$dir && $(MAKE) $@); done

deploy::
	for dir in $(MODULES_BUILD); do (cd $$dir && $(MAKE) $@); done

#
# to make things easier
#
update: clean
	(git pull)

lazy: update build

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
