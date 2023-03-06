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
	- update, build and install libbaalue/baalued (if needed)
	- update, build and install libhelper (if needed)
	- update, build and install lcd2004_i2c (if needed)
	- update, build and install sensor_pi (if needed)
	- update, build and install readout_db (if needed)
	- update, build and install poweroff_handler (if needed)
	- mupdatedb

tool checks on device:

	- is network (ip) config correct?
	- is baalued working?
	- does the hello-world screen come up?
	- are the shown values plausible?
	- does the poweroff button works?

sdk checks:

	- update checksum.sha256


PI-ENV_V0.0.3 (06.03.2023)
--------------------------

	Common checks                                           [2023-03-06 -> partly]

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


PI-ENV_V0.0.2 (XX.07.2021)
--------------------------

	Common checks                                           [2021-07-XX -> partly]

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


PI-ENV_V0.0.1 (25.04.2021)
--------------------------

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

