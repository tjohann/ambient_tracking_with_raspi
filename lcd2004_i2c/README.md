LCD 2004 I2C modul
==================

This is all content around the LCD handler.

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

No direct additional libaries are needed, but to build libhelper you need i2c-tools and i2c-tools-devel.


Display
-------

I use one of the very common LCD2004 displays, connected to the raspi via I2C modul (see http://wiki.sunfounder.cc/index.php?title=I2C_LCD2004 as an example).

![Alt text](../pics/development_environment_02.jpg?raw=true "LCD2004 and LCD1602 connected")


Start and usage
---------------

```
Usage: ./pcf8574_lcdd -[a:hi:t:]
       -i /dev/i2c-X -> I2C adapter
       -a 22         -> LCD address (in hex)
       -t 2          -> LCD1602(:1)/LCD2004(:2)
       -h            -> show this help

Example(s): LCD at first adapter with address 0x22
            from type LCD2004
           ./pcf8574_lcdd -i /dev/i2c-1 -a 22 -t 2
or
            LCD at first adapter with address 0x27
            from type LCD1602
            ./pcf8574_lcdd -i /dev/i2c-1 -a 27 -t 1
```

see also ./run_daemon.sh and [runit service descriptions](../etc/README.md)


Configuraion
------------

t.b.d.
