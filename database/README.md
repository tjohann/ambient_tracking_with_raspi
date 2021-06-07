The skeleton database
=====================

This modul contains a skeleton database for installation on the device.


Init database
-------------

To do a init of the database, use

	make init_db

Note: this creates a new database with on init value for testing. If you don't want that, use

	make init_clean_db


Useful commands
---------------

	.quit
	.databases
	.open
	.read ambient_data.sql
	.tables
	SELECT * FROM AmbientValues;  <- don`t forget the semicolon
