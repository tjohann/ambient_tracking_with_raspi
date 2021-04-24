#!/usr/bin/env bash
################################################################################
#
# Title       :    partition_sdcard.sh
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
# Milestones  :    V0.01 (jul 2016) -> initial version
#
# Requires    :
#
#
################################################################################
# Description
#
#   A simple tool to create a partition table on a sdcard
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

# which devnode?
DEVNODE='none'

# minimal size of a SD-Card
MIN_SD_SIZE=15000000

# addition to patitionlabel
SD_PART_NAME_POST_LABEL='none'

# program name
PROGRAM_NAME=${0##*/}

# my usage method
my_usage()
{
    echo " "
    echo "+--------------------------------------------------------+"
    echo "|                                                        |"
    echo "| Usage:  ${PROGRAM_NAME} "
    echo "|        [-d] -> sd-device /dev/sdd ... /dev/mmcblk ...  |"
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
while getopts 'hvd:' opts 2>$_log
do
    case $opts in
        h) my_usage ;;
        v) print_version ;;
	d) DEVNODE=$OPTARG ;;
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
    exit 3
fi


# ******************************************************************************
# ***                      The functions for main_menu                       ***
# ******************************************************************************

check_devnode()
{
    local mounted=`grep ${DEVNODE} /proc/mounts | sort | cut -d ' ' -f 1`
    if [[ "${mounted}" ]]; then
	echo "ERROR: ${DEVNODE} has already mounted partitions" >&2
	my_exit
    fi

    mounted=`echo ${DEVNODE} | awk -F '[/]' '{print $3}'`
    grep 1 /sys/block/${mounted}/removable 1>$_log
    if [ $? -ne 0 ] ; then
	echo "ERROR: ${DEVNODE} has is not removeable device" >&2
	my_exit
    fi

    grep 0 /sys/block/${mounted}/ro 1>$_log
    if [ $? -ne 0 ] ; then
	echo "ERROR: ${DEVNODE} is only readable" >&2
	my_exit
    fi

    local size=$(< /sys/block/${mounted}/size)
    if [[ "$size" -lt "$MIN_SD_SIZE" ]]; then
	echo "ERROR: ${DEVNODE} is to small with ${size} sectors" >&2
	my_exit
    fi
}

check_directories()
{
    if [[ ! -d "${SD_KERNEL}" ]]; then
	echo "ERROR -> ${SD_KERNEL} not available!" >&2
	echo "         have you added them to your fstab? (see README.md)" >&2
	my_exit
    fi

    if [[ ! -d "${SD_ROOTFS}" ]]; then
	echo "ERROR -> ${SD_ROOTFS} not available!" >&2
	echo "         have you added them to your fstab? (see README.md)" >&2
	my_exit
    fi

    if [[ ! -d "${SD_HOME}" ]]; then
	echo "ERROR -> ${SD_HOME} not available!" >&2
	echo "         have you added them to your fstab? (see README.md)" >&2
	my_exit
    fi
}

clean_sdcard()
{
    # keep old partition table
    # echo "sudo dd if=/dev/zero of=${DEVNODE} bs=1k count=1023 seek=1"
    # sudo dd if=/dev/zero of=${DEVNODE} bs=1k count=1023 seek=1

    # clear also partition table
    echo "sudo dd if=/dev/zero of=${DEVNODE} bs=1M count=1"
    sudo dd if=/dev/zero of=${DEVNODE} bs=1M count=1
    if [ $? -ne 0 ] ; then
	echo "ERROR: could not clear ${DEVNODE}" >&2
	my_exit
    fi

    sudo partprobe ${DEVNODE}
}

partition_sdcard()
{
    sudo blockdev --rereadpt ${DEVNODE}
    cat <<EOT | sudo sfdisk ${DEVNODE}
1M,64M,c
,8G,L
,,L
EOT
    if [ $? -ne 0 ] ; then
	echo "ERROR: could not create partitions" >&2
	my_exit
    fi

    sudo partprobe ${DEVNODE}
}

format_partitions()
{
    if [[ -b ${DEVNODE}1 ]]; then
	echo "sudo mkfs.vfat -F 32 -n KERNEL_${SD_PART_NAME_POST_LABEL} ${DEVNODE}1"
	sudo mkfs.vfat -F 32 -n KERNEL_${SD_PART_NAME_POST_LABEL} ${DEVNODE}1
	if [ $? -ne 0 ] ; then
	    echo "ERROR: could not format parition ${DEVNODE}1" >&2
	    my_exit
	fi
    else
	echo "ERROR -> ${DEVNODE}1 not available" >&2
    fi

    if [[ -b ${DEVNODE}2 ]]; then
	echo "sudo mkfs.ext4 -O ^has_journal -L ROOTFS_${SD_PART_NAME_POST_LABEL} ${DEVNODE}2"
	sudo mkfs.ext4 -O ^has_journal -L ROOTFS_${SD_PART_NAME_POST_LABEL} ${DEVNODE}2
	if [ $? -ne 0 ] ; then
	    echo "ERROR: could not format parition ${DEVNODE}2" >&2
	    my_exit
	fi
    else
	echo "ERROR -> ${DEVNODE}2 not available" >&2
    fi

    if [[ -b ${DEVNODE}3 ]]; then
	echo "sudo mkfs.ext4 -O ^has_journal -L HOME_${SD_PART_NAME_POST_LABEL} ${DEVNODE}3"
	sudo mkfs.ext4 -O ^has_journal -L HOME_${SD_PART_NAME_POST_LABEL} ${DEVNODE}3
	if [ $? -ne 0 ] ; then
	    echo "ERROR: could not format parition ${DEVNODE}3" >&2
	    my_exit
	fi
    else
	echo "ERROR -> ${DEVNODE}3 not available" >&2
    fi
}

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
	my_exit
    fi

    umount $SD_ROOTFS
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not umount ${SD_ROOTFS}" >&2
	my_exit
    fi

    umount $SD_HOME
    if [ $? -ne 0 ] ; then
	echo "ERROR -> could not umount ${SD_HOME}" >&2
	my_exit
    fi
}


# ******************************************************************************
# ***                         Main Loop                                      ***
# ******************************************************************************

# sudo handling up-front
echo " "
echo "+------------------------------------------+"
echo "| partition sd-card                        |"
echo "| --> need sudo for some parts             |"
echo "+------------------------------------------+"
echo " "

sudo -v
# keep-alive
while true; do sudo -n true; sleep 60; kill -0 "$$" || exit; done 2>/dev/null &

echo " "
echo "+------------------------------------------+"
echo "| do some testing on $DEVNODE ...           "
echo "+------------------------------------------+"
check_devnode

SD_KERNEL=$RASPI_SDCARD_KERNEL
SD_ROOTFS=$RASPI_SDCARD_ROOTFS
SD_HOME=$RASPI_SDCARD_HOME
SD_PART_NAME_POST_LABEL="RPI"

echo " "
echo "+------------------------------------------+"
echo "| check needed directories                 |"
echo "+------------------------------------------+"
check_directories

echo " "
echo "+------------------------------------------+"
echo "| clean $DEVNODE ...                        "
echo "+------------------------------------------+"
clean_sdcard

echo " "
echo "+------------------------------------------+"
echo "| start paritioning $DEVNODE                "
echo "+------------------------------------------+"
partition_sdcard

echo " "
echo "+------------------------------------------+"
echo "| start formating the partitions           |"
echo "+------------------------------------------+"
format_partitions

echo " "
echo "+------------------------------------------+"
echo "| check if we can mount the partitions     |"
echo "+------------------------------------------+"
mount_partitions

echo " "
echo "+------------------------------------------+"
echo "| check if we can umount the partitions    |"
echo "+------------------------------------------+"
umount_partitions

echo " "
echo "+------------------------------------------+"
echo "| $DEVNODE is ready to use                  "
echo "+------------------------------------------+"

cleanup
echo " "
echo "+------------------------------------------+"
echo "|             Cheers $USER                  "
echo "+------------------------------------------+"
echo " "
