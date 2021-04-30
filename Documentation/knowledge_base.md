Some infos about all relevant topics
====================================


Raspi 2
-------

https://www.raspberrypi.org/products/raspberry-pi-2-model-b


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
