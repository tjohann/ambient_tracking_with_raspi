My ambient tracking modul with raspi
====================================

This is all content around my ambient tracking modul with a Raspi2. It collects ambient values like temperatur or humidity and shows it on a LCD2004 display.

[Related links](Documentation/links.md).

The basic user interface are make targets, which then start the corresponding scripts or actvivate something:

    +-----------------------------------------------------------+
    |                                                           |
    |                  Nothing to build                         |
    |                                                           |
    +-----------------------------------------------------------+
    | Example:                                                  |
    | make build              -> build all code subfolders      |
    | make clean              -> clean all dir/subdirs          |
    | make install            -> install some scripts to        |
    |                            $(HOME)/bin                    |
    | make uninstall          -> remove scripts from            |
    |                            $(HOME)/bin                    |
    +-----------------------------------------------------------+

WARNING: This is work in progress! Don't expect things to be complete in any dimension.

If you face a bug then pls use https://github.com/tjohann/ambient_tracking_with_raspi/issues to create an issue.


Images
------

You can download the ready-to-use image from sourceforge (TODODODODODO). All images are based on Void-Linux (https://voidlinux.org/) and use MUSL as libc (https://www.musl-libc.org/).

The sd-card needs 3 different partitions which are reflected by the images tarballs itself:

	1). kernel (fat32/32 meg) -> raspi2_kernel.tgz
	2). rootfs (ext4/10 gig) -> rasp2_rootfs.tgz
	3). home (ext4/the rest) -> raspi2_home.tgz


User
----

    root (password: root)
    baalue (password: baalue)

The user baalue is available on all images, you can use it to login via ssh and then use sudo or su -l for root tasks.


Raspi 2
-------

Some technial data:

	900MHz quad-core ARM Cortex-A7 CPU
	1GB RAM


Build kernel
------------

I use a custom kernel [Build custom kernel for the Raspi2](Documentation/howto_kernel.txt).


DockerPi Sensor Hub
-------------------

I use the DockerPi Sensor shield, direct connected to the raspi. The resulting problem is, that the ECU could have an effect on the measured temperatur. Thats on of the reason, why the CPUFreq governor is **powersave**


Display
-------

I use a LCD2004 display connected to the raspi via I2C modul.


Powermanagment
--------------

The default CPUFreq governor is **powersave**, which results in a frequency of 600MHz instead of 900MHz.


Additional libraries/tools
--------------------------

To control the device i use my daemon baalued (https://github.com/tjohann/baalued) which is based on libbalue (https://github.com/tjohann/libbaalue).


Additonal scripts
-----------------

In the scripts folder you find the used helper tools to setup an sd-card or to download and build a kernel.

