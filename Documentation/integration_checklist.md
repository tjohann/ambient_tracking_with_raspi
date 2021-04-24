Integration - checklist for Raspi-PI-ENV image
==============================================

This is a simple integration checklist for my ambient_tracking_with_raspi
Project. The purpose is to provide a repreducable delivery for the
main versions (PI-ENV_Vx.x.x).

Below you find the common checks and the special tasks for PI-ENV_Vx.x.x.


Common checks
-------------

installation checks:

	- check raspi installation

build/config updates on device:

	- xbps-install -Su
	- reboot
	- xbps-remove -Oo
	- date > UPDATE_ROOTFS_DATE
	- update, build and install libbaalue/baalued
	- update, build and install lcd2004_i2c
	- update, build and install sensor_pi
	- mupdatedb

tool checks on device:

	- is network (ip) config correct
	- is baalued working

sdk checks:

	- update checksum.sha256



 (25.04.2021)
---------------------------

	Common checks                                           [2021-04-25 -> done]

	Config updates (on the target):
		- ...

	Build updates:
		- libbaalue
		  - ...
		- baalued
		  - ...
		- lcd2004_i2c
		  - ...
	    - sensor_pi
		  - ...

	Script updates:
		- ...

	Others:
		- ...

