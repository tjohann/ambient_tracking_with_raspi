My ambient tracking modul with raspi
====================================

This is all content around my ambient tracking modul with a Raspi2. It collects ambient values like temperatur or humidity and shows it on a LCD2004 display.

[Related links](Documentation/links.md).

The basic user interface are make targets, which then start the corresponding scripts or directly actvivate something:

	+-----------------------------------------------------------+
	|                                                           |
	|                  Nothing to build                         |
	|                                                           |
	+-----------------------------------------------------------+
	| Example:                                                  |
	| make build              -> build all code subfolders      |
	| make clean              -> clean all dir/subdirs          |
	| make distclean          -> clean + kernel folder          |
	| make install            -> install all to $(HOME)/...     |
	| make uninstall          -> remove all from $(HOME)/...    |
	| make deploy             -> install bin's to /usr/bin      |
	+-----------------------------------------------------------+

WARNING: This is work in progress! Don't expect things to be complete in any dimension.

If you face a bug, then pls use https://github.com/tjohann/ambient_tracking_with_raspi/issues to create an ticket.


Requirement
-----------

The only software requirements are git (to clone/update runtimedir), rsync (to sync content below workdir and srcdir) and wget (to download the image tarballs from sourceforge).
To build the applications you also need i2c-tools, i2c-tools-devel and sqlite.


To use the my scripts, you need to source the pi-env_env file, wich sets some shell values:

	. ./pi-env_env


Additional mountpoints
----------------------

The scripts expect some mountpoints:

	# raspi
	LABEL=KERNEL_RPI /mnt/raspi/raspi_kernel  auto     noauto,user,rw     0       0
	LABEL=ROOTFS_RPI /mnt/raspi/raspi_rootfs  auto     noauto,user,rw     0       0
	LABEL=HOME_RPI   /mnt/raspi/raspi_home    auto     noauto,user,rw     0       0

If you create the images with my scripts, then the partitions will get an label, used to mount everthing.


Images
------

You can download the ready-to-use image from sourceforge (https://sourceforge.net/projects/pi-env/). All images are based on Void-Linux (https://voidlinux.org/) and use MUSL as libc (https://www.musl-libc.org/).

The sd-card needs 3 different partitions which are reflected by the images tarballs itself:

	1). kernel (fat32/64 meg) -> raspi2_kernel.tgz
	2). rootfs (ext4/8 gig) -> rasp2_rootfs.tgz
	3). home (ext4/the rest) -> raspi2_home.tgz


User
----

The user baalue is available on all images, you can use it to login via ssh and then use sudo or su -l for root tasks.

    root (password: root)
    baalue (password: baalue)


Network and distcc
------------------

For testing purpose i have a physical network (see also https://github.com/tjohann/a20_sdk), where all devices are conneted to. The easiest way to use it is to add a usb-ethernet adapter to your main machine and add your target device to it, otherwise you have to change the configuration by hand.

My cluster:

	192.168.178.80            baalue-80.my.domain             baalue_master
	192.168.178.81            baalue-81.my.domain             baalue-01
	192.168.178.82            baalue-82.my.domain             baalue-02
	192.168.178.83            baalue-83.my.domain             baalue-03
	192.168.178.84            baalue-84.my.domain             baalue-04
	192.168.178.85            baalue-85.my.domain             baalue-05
	192.168.178.86            baalue-86.my.domain             baalue-06
	192.168.178.87            baalue-87.my.domain             baalue-07
	192.168.178.88            baalue-88.my.domain             baalue-08
	192.168.178.89            baalue-89.my.domain             baalue-09
	192.168.178.90            baalue-90.my.domain             baalue-10
	192.168.178.91            baalue-91.my.domain             baalue-11
	192.168.178.92            baalue-92.my.domain             baalue-12
	192.168.178.93            baalue-93.my.domain             baalue-13
	192.168.178.94            baalue-94.my.domain             baalue-14
	192.168.178.95            baalue-95.my.domain             baalue-15
	192.168.178.96            baalue-96.my.domain             baalue-16


You can use the build cluster based with this device (to build a kernel or ...). Additional check https://github.com/tjohann/baalue_distcc . Here you should find all information and hints you need.

Note: The base configuration is already included in my image.


Versioninfo
-----------

I use a standard version scheme via git tags based on 3 numbers:

	PI-ENV_V0.0.1

The first number is the mayor number which reflect bigger changes. The second number (minor) will change because of

	- new scripts
	- image updates
	- new releases of sensor_pi/lcd2004_i2c

So a simple version update of the kernel will not increase the minor number, instead it will increase the third number (age number):

	- bugfixes
	- update kernel version
	- all smaller changes


Raspi 2
-------

Some technial data:

	900MHz quad-core ARM Cortex-A7 CPU
	1GB RAM

My development environment:

![Alt text](pics/development_environment.jpg?raw=true "Development environment with both supported LCD's")
![Alt text](pics/detailed_view_baseboard.jpg?raw=true "Detailed view of the baseboard")
![Alt text](pics/ambient_tracker_final.jpg?raw=true "Final version of the Ambient-Tracker")
![Alt text](pics/ambient_tracker_final_side_view.jpg?raw=true "Side view of the final version of the Ambient-Tracker")


Schematics
----------

See folder schematics for more info.


Build kernel
------------

I use a custom kernel, see [build custom kernel for the Raspi2](Documentation/howto_kernel.txt) for a simpel howto and [build kernel via Baalue-Cluster](Documentation/howto_kernel_cluster_build.txt) for the cluster one.

Actual kernel version:

	5.10.x (NO RT-PREEMPT)


Powermanagement
---------------

The default CPUFreq governor is **conservative**, which results in a frequency mostly of 600MHz instead of 900MHz.


Additional libraries/tools
--------------------------

To control the device, i use my daemon baalued (https://github.com/tjohann/baalued), which is based on libbalue (https://github.com/tjohann/libbaalue).


Additional scripts
------------------

In the scripts folder you find the used helper tools to setup an sd-card or to download and build a kernel.

Format an already partioned sd-card:

	+--------------------------------------------------------+
	|                                                        |
	| Usage: format_sdcard.sh
	|        [-d] -> sd-device /dev/sdd ... /dev/mmcblk ...  |
	|        [-v] -> print version info                      |
	|        [-h] -> this help                               |
	|                                                        |
	+--------------------------------------------------------+

Partition and format a sd-card:

	+--------------------------------------------------------+
	|                                                        |
	| Usage:  partition_sdcard.sh
	|        [-d] -> sd-device /dev/sdd ... /dev/mmcblk ...  |
	|        [-v] -> print version info                      |
	|        [-h] -> this help                               |
	|                                                        |
	+--------------------------------------------------------+

Mount/Unmount a sd-card:

	+--------------------------------------------------------+
	|                                                        |
	| Usage: umount_partitions.sh
	|        [-u] -> un-mount patitions (default)            |
	|        [-m] -> mount patitions                         |
	|        [-v] -> print version info                      |
	|        [-h] -> this help                               |
	|                                                        |
	+--------------------------------------------------------+

Download the tarballs from sourceforge:

	+--------------------------------------------------------+
	|                                                        |
	| Usage: get_image_tarballs.sh
	|        [-v] -> print version info                      |
	|        [-h] -> this help                               |
	|                                                        |
	+--------------------------------------------------------+

Untar the downloaded images to the sd-card:

	+--------------------------------------------------------+
	|                                                        |
	| Usage: untar_images_to_sdcard.sh
	|        [-v] -> print version info                      |
	|        [-h] -> this help                               |
	|                                                        |
	+--------------------------------------------------------+


Ruint service definition
------------------------

In folder ./etc you find the runit service definition. Note, that you (normaly) have to change the I2C configuration! See [Runit service files](etc/README.md)


DockerPi Sensor Hub
-------------------

I use the DockerPi Sensor shield (https://wiki.52pi.com/index.php/DockerPi_Sensor_Hub_Development_Board_SKU:%20_EP-0106), direct connected to the raspi. The resulting problem is, that the ECU has an effect on the measured temperatur. One way around is to make shure, that the ecu consumes as less power as possible. Actual measurements show, that all temperature values on the board are not usable (see ./Documentation/temperature_correction.pdf). Another efect with the external temperature module is, that the surface is reflecting. So when it is located outside and the sun shines on it, the measured results are not the air temperature.

Result: I have some correction values used in amtrack (and in the database). Also i use the external temperature sensor as the only temperature sensor and ignore the others. In addition i add the CPU temperature to the database, so an correction is possible.


Display
-------

I use one of the very common LCD2004 displays, connected to the raspi via I2C modul (see http://wiki.sunfounder.cc/index.php?title=I2C_LCD2004 as an example).


The code
--------

Common parts like become_damon() are located in a static library, see [Helper library](libhelper/README.md). For more details on the daemon code, see [Display handler](lcd2004_i2c/README.md) and [Sensor shield](sensor_pi/README.md).

The application amtrack connect both daemon to a useful system, wich shows the relevant informations like temperature on the display, see [ambient tracker](amtrack/README.md).

For further usage, a sqlite database is on the device (/home/ambient/ambient_data.db). It stores the values with the UNIX timestamp as primary key.


The database
------------

For more details see [the database](database/README.md).


The http server
---------------

Note: not really working yet

The local webserver provides a simple webpage which shows the content of the database with an update rate of ~10 minutes.
