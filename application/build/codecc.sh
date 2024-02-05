#!/bin/bash
# codecc
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

export TARGET_PATH=/home/$USER/coverityandfortify
export BUILD_PATH=$(pwd)
export USER=$(whoami)

helpinfo()
{
    echo
    echo  "Invalid Parameters. Please input as following :"
    echo
    echo "    $0 [-c][-f][-fc] [app][all]"
    echo
    echo "app support list :"
    echo "            $(ls -l ../apps/ | awk  '{print "\t\t"$9}' |grep -v txt$)"
    
    echo "		user_interface"
    echo "		libs"
    echo "              kvm"
    echo "              player"
}
if [ "$USER" == "root" ]; then
	echo "Please do not run with root."
    exit;
fi
if [ $# == 2 ]; then

	if [ "$2" == "user_interface" ] || [ "$2" == "libs" ] || [ "$2" == "kvm" ] || [ "$2" == "player" ]; then
        echo
    	elif [ "$2" != "all" ]; then

		ls -l ../apps/ | awk  '{print "\t\t"$9}' |grep -w $2 > /dev/null
		if [ $? != 0 ];then		
			echo "unsupported module name."
			helpinfo
			exit 1
		fi
	fi


	if [ "$1" == "-f" ]; then

	    echo "bash -x ./codecc_fortify.sh $2"

	elif [ "$1" == "-c" ]; then

	    echo "bash -x ./codecc_coverity.sh $2"
    
	elif [ "$1" == "-fc" ]; then
	    echo "bash -x ./codecc_fortify.sh $2"
	    echo "bash -x ./codecc_coverity.sh $2"
        fi
else
	helpinfo

	exit
fi
    
