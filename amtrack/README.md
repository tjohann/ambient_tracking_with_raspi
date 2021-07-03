The ambient tracker modul
=========================

This modul connect the sensor_pid and the pcf8475_lcdd modul. It implements the logic to bring the different informations together.

[Background infos](../Documentation/knowledge_base.md).

The basic interface are make targets:

    +-----------------------------------------------------------+
    |                                                           |
    |                  Nothing to build                         |
    |                                                           |
    +-----------------------------------------------------------+
    | Example:                                                  |
    | make                    -> build all                      |
    | make clean              -> clean all                      |
    | make distclean          -> clean and uninstall            |
    | make install            -> install bin to $(HOME)/bin     |
    | make uninstall          -> remove bin from $(HOME)/bin    |
    +-----------------------------------------------------------+

If you face a bug, then pls use https://github.com/tjohann/ambient_tracking_with_raspi/issues to create an ticket.


Requirement
-----------

No direct additional libaries are needed, but to build libhelper you need i2c-tools, i2c-tools-devel and sqlite.


Start and usage
---------------

```
Usage: ./amtrack -[h]
       -t 2          -> LCD1602(:1)/LCD2004(:2)
       -h            -> show this help

Example: LCD size 20x4
        ./amtrack -t 2
```


Database
--------

For further working, a sqlite database is on the device (/home/ambient/ambient_data.db). It stores the values for future use like a webserver with the a dataplot.
