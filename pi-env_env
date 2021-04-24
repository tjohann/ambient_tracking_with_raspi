# +-------------------- setup the pi-env dev environment ----------------------+
# |                               pi-env_env                                   |
# +----------------------------------------------------------------------------+

# set MY_HOST_ARCH
export MY_HOST_ARCH=$(uname -m)

# home of the git repo
export RASPI_HOME=${HOME}/Projects/ambient_tracking_with_raspi

# extend PATH for our PI-Env stuff
export PATH=$PATH:${RASPI_HOME}/bin

# set mount points for the sdcard -> raspi2
export RASPI_SDCARD_KERNEL=/mnt/raspi/raspi_kernel
export RASPI_SDCARD_ROOTFS=/mnt/raspi/raspi_rootfs
export RASPI_SDCARD_HOME=/mnt/raspi/raspi_home

echo "Setup env for host \"${MY_HOST_ARCH}\" with project root dir \"${RASPI_HOME}\""

#EOF
