#!/usr/bin/env bash
################################################################################
#
# Title       :    prepare_image_tarballs.sh
#
# License:
#
# GPL
# (c) 2021, thorsten.johannvorderbrueggen@t-online.de
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
################################################################################
#
# Date/Beginn :    24.04.2021/24.04.2021
#
# Version     :    V0.01
#
# Milestones  :    V0.01 (apr 2021) -> first working version
#
# Requires    :
#
#
################################################################################
# Description
#
#   A simple tool to prepare the tarballs for upload to sourceforge
#
################################################################################
#

# VERSION-NUMBER
VER='0.01'

# if env is sourced
MISSING_ENV='false'

# mountpoints
SD_KERNEL='none'
SD_ROOTFS='none'
SD_HOME='none'

# ...
BRAND="raspi2"

# program name
PROGRAM_NAME=${0##*/}

# my usage method
my_usage()
{
    echo " "
    echo "+--------------------------------------------------------+"
    echo "| Usage: ${PROGRAM_NAME} "
    echo "|        [-v] -> print version info                      |"
    echo "|        [-h] -> this help                               |"
    echo "|                                                        |"
    echo "+--------------------------------------------------------+"
    echo " "
    exit
}

# my cleanup
cleanup() {
    rm $_temp 2>/dev/null
    rm $_log 2>/dev/null
}

# my exit method
my_exit()
{
    # if something is still mounted
    umount_partitions

    echo "+-----------------------------------+"
    echo "|          Cheers $USER            |"
    echo "+-----------------------------------+"
    cleanup
    # http://tldp.org/LDP/abs/html/exitcodes.html
    exit 3
}

# print version info
print_version()
{
    echo "+------------------------------------------------------------+"
    echo "| You are using ${PROGRAM_NAME} with version ${VER} "
    echo "+------------------------------------------------------------+"
    cleanup
    exit
}

# --- Some values for internal use
_temp="/tmp/${PROGRAM_NAME}.$$"
_log="/tmp/${PROGRAM_NAME}.$$.log"


# check the args
while getopts 'hv' opts 2>$_log
do
    case $opts in
        h) my_usage ;;
        v) print_version ;;
        ?) my_usage ;;
    esac
done


# ******************************************************************************
# ***             Error handling for missing shell values                    ***
# ******************************************************************************

if [[ ! ${RASPI_HOME} ]]; then
    MISSING_ENV='true'
fi

if [[ ! ${RASPI_SDCARD_KERNEL} ]]; then
    MISSING_ENV='true'
fi

if [[ ! ${RASPI_SDCARD_ROOTFS} ]]; then
    MISSING_ENV='true'
fi

if [[ ! ${RASPI_SDCARD_HOME} ]]; then
    MISSING_ENV='true'
fi

# show a usage screen and exit
if [ "$MISSING_ENV" = 'true' ]; then
    cleanup
    echo " "
    echo "+--------------------------------------+"
    echo "|                                      |"
    echo "|  ERROR: missing env                  |"
    echo "|         have you sourced env-file?   |"
    echo "|                                      |"
    echo "|          Cheers $USER               |"
    echo "|                                      |"
    echo "+--------------------------------------+"
    echo " "
    exit
fi


# ******************************************************************************
# ***                      The functions for main_menu                       ***
# ******************************************************************************

mount_partitions()
{
    mount $SD_KERNEL
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not mount ${SD_KERNEL}" >&2
	my_exit
    fi

    mount $SD_ROOTFS
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not mount ${SD_ROOTFS}" >&2
	my_exit
    fi

    mount $SD_HOME
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not mount ${SD_HOME}" >&2
	my_exit
    fi
}

umount_partitions()
{
    umount $SD_KERNEL
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not umount ${SD_KERNEL}" >&2
	# do not exit -> will try to umount the others
    fi

    umount $SD_ROOTFS
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not umount ${SD_ROOTFS}" >&2
	# do not exit -> will try to umount the others
    fi

    umount $SD_HOME
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not umount ${SD_HOME}" >&2
	# do not exit -> will try to umount the others
    fi
}



# ******************************************************************************
# ***                         Main Loop                                      ***
# ******************************************************************************

# sudo handling up-front
echo " "
echo "+------------------------------------------+"
echo "| prepare image tarballs for upload to sf  |"
echo "| --> need sudo for some parts             |"
echo "+------------------------------------------+"
echo " "

sudo -v
# keep-alive
while true; do sudo -n true; sleep 60; kill -0 "$$" || exit; done 2>/dev/null &

SD_KERNEL=$RASPI_SDCARD_KERNEL
SD_ROOTFS=$RASPI_SDCARD_ROOTFS
SD_HOME=$RASPI_SDCARD_HOME

mount_partitions

cd $SD_ROOTFS
sudo tar czpvf ${RASPI_HOME}/images/${BRAND}_rootfs.tgz .
if [ $? -ne 0 ] ; then
    echo "ERROR -> could not tar ${RASPI_HOME}/images/${BRAND}_rootfs.tgz" >&2
    my_exit
fi

sync

cd $SD_KERNEL
tar czvf ${RASPI_HOME}/images/${BRAND}_kernel.tgz .
if [ $? -ne 0 ] ; then
    echo "ERROR -> could not tar ${RASPI_HOME}/images/${BRAND}_kernel.tgz" >&2
    my_exit
fi

sync

cd $SD_HOME
sudo tar czpvf ${RASPI_HOME}/images/${BRAND}_home.tgz .
if [ $? -ne 0 ] ; then
    echo "ERROR -> could not tar ${RASPI_HOME}/images/${BRAND}_home.tgz" >&2
    my_exit
fi

sync

cd ${RASPI_HOME}
umount_partitions
cleanup

echo " "
echo "+----------------------------------------+"
echo "|            Cheers $USER "
echo "+----------------------------------------+"
echo " "
