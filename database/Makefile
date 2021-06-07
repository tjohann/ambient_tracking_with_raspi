#
# my simple makefile act as something like a user interface
#

all::
	@echo Cheers

deploy::
	(cp ambient_data.db /var/run/ambient_data.db)

init_db::
	(sqlite3 ambient_data.db < ambient_data.sql)

init_clean_db::
	(sqlite3 ambient_data.db < ambient_data_clean.sql)

clean::
	rm -f *~
	rm -f .*~

distclean: clean
