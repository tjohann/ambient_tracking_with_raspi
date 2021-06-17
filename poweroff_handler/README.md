Poweroff handler
================

This is all content around the poweroff handling via switch and LED control indication.

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

t.b.d.


schematic
---------

The switch and the LED are connected to:

	switch -> GPIO26 (P1-37)
	led -> GPIO19 (P1-35)

See:
![Alt text](../schematics/shutdown.jpg?raw=true "Schematic for shutdown and LED control indication")


Start and usage
---------------

```
Usage: ./poweroff_d -[h]
       -h            -> show this help

Example:
        ./poweroff_d
```

see also ./run_daemon.sh and [runit service descriptions](../etc/README.md)
