My ambient tracking modul with raspi
====================================

This is all content around my ambient tracking modul with raspi. Its collects ambient values like temperatur or humidity and shows it on a LCD2004 display.

[Related links](Documentation/links.md).

The basic user interface are make targets, which then start the corresponding scripts:

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

You can download the ready-to-use image from sourceforge (TODODODODODO).

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

....

900MHz quad-core ARM Cortex-A7 CPU
1GB RAM


Build kernel
------------

I use a custom kernel [Build custom kernel for the Raspi2](Documentation/howto_kernel.txt).


DockerPi Sensor Hub
-------------------

I use the DockerPi Sensor shield, direct connected to the raspi


Display
-------

I use a LCD2004 display connected to the raspi via I2C modul.


Powermanagment
--------------

tbd


