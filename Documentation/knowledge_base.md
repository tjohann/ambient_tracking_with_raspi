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
https://cdn-shop.adafruit.com/datasheets/HD44780.pdf
https://de.wikipedia.org/wiki/HD44780



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


Connect a 10k pullup and a 1k resistor in line to the port:

![Alt text](../pics/connect_poweroff_button.png?raw=true "schematic poweroff button")

![Alt text](../pics/GPIO-Pinout-Diagram.png?raw=true "GPIO pinout")

Sources of the Rasp-GPIO-Pinout-Diagram: https://www.raspberrypi.org/documentation/usage/gpio/


CPU temperature
---------------

Relevant folder:

	/sys/class/thermal/thermal_zone0

Results@Raspi2:

	baalue@pi-env:/sys/class/thermal/thermal_zone0$ cat type
	cpu-thermal

	baalue@pi-env:/sys/class/thermal/thermal_zone0$ cat temp
	33090

Possible script:

	paste <(cat /sys/class/thermal/thermal_zone*/type) <(cat /sys/class/thermal/thermal_zone*/temp) | column -s $'\t' -t | sed 's/\(.\)..$/.\1°C/'

Results@Raspi2:

	baalue@pi-env:/sys/class/thermal/thermal_zone0$ paste <(cat /sys/class/thermal/thermal_zone*/type) <(cat /sys/class/thermal/thermal_zone*/temp) | column -s $'\t' -t | sed 's/\(.\)..$/.\1°C/'
	cpu-thermal  33.6°C


For more informations see https://www.cyberciti.biz/faq/linux-find-out-raspberry-pi-gpu-and-arm-cpu-temperature-command/

	cpu=$(</sys/class/thermal/thermal_zone0/temp)
	echo "$((cpu/1000))°C"


BMP180 modul
------------

For valid temperature i add a BMP180 module. There're different versions outside. Choose the one you like. To activate it i added


Path via i2c or iio:

	/sys/bus/i2c/devices/1-0077/iio:device0
	or
	/sys/bus/iio/devices/iio:device0

Output:

	baalue@pi-env:/sys/bus/i2c/devices/1-0077/iio:device0$ pwd
	/sys/bus/i2c/devices/1-0077/iio:device0
	baalue@pi-env:/sys/bus/i2c/devices/1-0077/iio:device0$ tree .
	.
	├── dev
	├── in_pressure_input
	├── in_pressure_oversampling_ratio
	├── in_temp_input
	├── in_temp_oversampling_ratio
	├── name
	├── power
	│   ├── autosuspend_delay_ms
	│   ├── control
	│   ├── runtime_active_time
	│   ├── runtime_status
	│   └── runtime_suspended_time
	├── subsystem -> ../../../../../../../bus/iio
	└── uevent

	2 directories, 12 files

	baalue@pi-env:/sys/bus/i2c/devices/1-0077/iio:device0$ cat in_temp_input
	27700

	baalue@pi-env:/sys/bus/i2c/devices/1-0077/iio:device0$ cat in_pressure_input
	100.040000000

