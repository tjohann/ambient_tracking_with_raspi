Some infos about all relevant topics
====================================


Raspi 2
-------

https://www.raspberrypi.org/products/raspberry-pi-2-model-b

Raspi I2C topics:

	https://www.waveshare.com/wiki/Raspberry_Pi_Tutorial_Series:_I2C


Display
-------

https://tutorials-raspberrypi.de/hd44780-lcd-display-per-i2c-mit-dem-raspberry-pi-ansteuern
https://github.com/tjohann/mydriver/tree/master/userspace_examples/pcf8574_lcd1602


DockerPi Sensor Hub
-------------------

https://wiki.52pi.com/index.php/DockerPi_Sensor_Hub_Development_Board_SKU:%20_EP-0106


I2C topics and linux
--------------------

I2C tools -> i2cdetect -l

Links:

	https://www.kernel.org/doc/Documentation/i2c/i2c-protocol
	http://www.netzmafia.de/skripten/hardware/RasPi/RasPi_I2C.html

Youtube:
	https://www.youtube.com/watch?v=BvIQ0b2gUFs  (Raspberry PI 2 - Fun with I2C DACs and ADC's)


Using i2c-dev driver
--------------------

Sysfs entry -> /sys/class/i2c-dev/
Character device i2c-%d -> (i2c-0, i2c-1, ...)

Links:

	https://www.kernel.org/doc/Documentation/i2c/dev-interface
	https://www.kernel.org/doc/Documentation/i2c/fault-codes


Build a raspi kernel
--------------------

https://www.raspberrypi.org/documentation/linux/kernel/building.md


SQLite usage
------------

https://zetcode.com/db/sqlitec/


Poweroff button
---------------

To shutdown the device via a button (pinout connector 1 and 40), add

	dtoverlay=gpio-shutdown,gpio-pin=21

to config.txt. I use the "other" end of the PIN header.

Connect a 10k pullup and a 1k resistor in line to the port:

![Alt text](pics/connect_poweroff_button.png?raw=true "schematic poweroff button")

![Alt text](pics/GPIO-Pinout-Diagram.png?raw=true "GPIO pinout")

Sources of pics above: https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/robot/buttons_and_switches/ and https://www.raspberrypi.org/documentation/usage/gpio/

For more info see also: https://www.raspberrypi.org/forums/viewtopic.php?t=217442#p1565264 and https://www.embeddedpi.com/documentation/gpio/mypi-industrial-raspberry-pi-psu-shutdown-gpio-line
