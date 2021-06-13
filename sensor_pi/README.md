LCD 2004 I2C modul
==================

This is all content around the sensor handler.

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

The i2c-tools and i2c-tools-devel are needed. As an indirect requirement you need sqlite to build.


DockerPi-Shield
---------------

I use one of this DockerPi Shield: https://wiki.52pi.com/index.php/DockerPi_Sensor_Hub_Development_Board_SKU:%20_EP-0106


Start and usage
---------------

```
Usage: ./sensor_pid -[hi:a:]
       -i /dev/i2c-X -> I2C adapter
       -a 17         -> I2C address (in hex)
       -h            -> show this help

Example:
        ./sensor_pid -i /dev/i2c-1 -a 17
```

see also ./run_daemon.sh and [runit service descriptions](../etc/README.md)
