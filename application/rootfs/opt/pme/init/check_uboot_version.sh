#!/bin/sh

# This script is used to check uboot version and upgrade uboot.
export LD_LIBRARY_PATH=/opt/pme/lib:/opt/pme/plugins/chip

main()
{
	/opt/pme/bin/uboot_tool -u /usr/upgrade/u-boot.bin
	sleep 1
}

main

