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

WARNING: This is work in progress! Don't expect things to be complete in any dimension.

If you face a bug, then pls use https://github.com/tjohann/ambient_tracking_with_raspi/issues to create an ticket.


Requirement
-----------

No additional libaries are needed.


Start and usage
---------------

```
Usage: ./amtrack -[h]
       -t 2          -> LCD1602(:1)/LCD2004(:2)
       -h            -> show this help

Example: LCD size 20x4
        ./amtrack -t 2
```


Configuraion
------------

t.b.d.