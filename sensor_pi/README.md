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

No additional libaries are needed.


DockerPi-Shield
---------------

I use one of this DockerPi Shield: https://wiki.52pi.com/index.php/DockerPi_Sensor_Hub_Development_Board_SKU:%20_EP-0106


Start and usage
---------------

```
sage: ./sensor_pid -[h]
       -h            -> show this help

Example:
        ./sensor_pid
```

see also ./run_daemon.sh and [runit service descriptions](../etc/README.md)


Configuraion
------------

t.b.d.
