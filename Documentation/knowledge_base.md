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

Link:

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


GPIO and raspi
--------------

Link:

	https://elinux.org/RPi_GPIO_Code_Samples


Build a raspi kernel
--------------------

Link:

	https://www.raspberrypi.org/documentation/linux/kernel/building.md


SQLite usage
------------

Link:

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

For valid temperature i add a BMP180 module. There\'re different versions outside. Choose the one you like. To activate it i added


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

To get the real values you should correct them depending on you location, see https://www.raspberry-pi-geek.com/Archive/2017/23/Using-the-BMP180-to-record-air-pressure-and-temperature for more informations about it.


RTC (real time clock) module
----------------------------

The Raspi does not have a RTC on the board. So we need a static network connection. In my images, the ntp client is already activated, but direct after a reboot it takes some time to get the actual time. During this period all date values are wrong (stand on UNIX time, see https://de.wikipedia.org/wiki/Unixzeit).

I add the module like https://www.elecrow.com/wiki/index.php?title=Tiny_RTC to the board. To use it with an raspberry you have to manually to some changes. There`re a lot of descriptions in the www, here`s an really good example: http://www.netzmafia.de/skripten/hardware/RasPi/Projekt-RTC/index.html .

A least there`re 2 changes needed:

	remove the pullups from SCL/SDA (remove R2/R3)
	remove the load circuit if you want to use a CR2032 batterie (remove D1/R4/R6 -> shortcut R6 pads)

![Alt text](../pics/RTC_before.jpg?raw=true "The original PCB")
![Alt text](../pics/RTC_after_2.jpg?raw=true "What changed?")
