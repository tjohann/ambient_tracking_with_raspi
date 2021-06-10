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


Readout database
----------------

Readout_db is a simple tool which prints the complete content of the database to stdout.

	Usage: ./readout_db -[h]
       -h            -> show this help

	Example: readout all values of database
        ./readout_db
