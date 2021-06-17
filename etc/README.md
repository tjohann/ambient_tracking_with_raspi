runit service definition
========================

Note: you have to change the I2C config!

lcd_daemon
----------

The LCD daemon -> pcf8574_lcdd. It's a link to lcd_daemon_lcd2004, the default in my example.

My first (default) configuration:

	type -> LCD2004 (20x4)
	devnode -> /dev/i2c-1
	address -> 0x22

My second configuration:

	type -> LCD1602 (16x2)
	devnode -> /dev/i2c-1
	address -> 0x27

See ![Alt text](pics/development_environment_02.jpg?raw=true "Both LCD's")

To use one of the configuration, copy the one you want to /etc/sv and change, what is different in your environment. After that create a link from /etc/sv/lcd_daemon to /var/service . After some seconds you should see a hello-screen on the display. If not, check the output at /var/log/messages .


sensor_daemon
-------------

The sensor daemon -> sensor_pid.

My configuration (Raspi2):

	devnode -> /dev/i2c-1
	address -> 0x17


ambient tracker
---------------

The ambient tracker tool -> amtrack.

My configuration (Raspi2):

	lcd -> LCD2004


poweroff daemon
---------------

The poweroff daemon to make shutdown easy.

My configuration (Raspi2):

	switch -> GPIO26 (P1-37)
	led -> GPIO19 (P1-35)
