#!/bin/sh
# This script is only intend to obtain the XML value of IsCustomBrandFW for backup(1710) or avail(1711) partion
BMC_MOUNT_PATH=/dev/shm/bmc_mount
BMC_CB_FLAG_PATH=/var/bmc_ver_info
BMC_IS_CB_FW_FILE=$BMC_CB_FLAG_PATH/bmc_is_cb_fw
BMC_CB_RESULT=$BMC_CB_FLAG_PATH/bmc_cb_result

get_cb_flag()
{
    local cur_part=mmcblk0p5
    if [ `uname -m` != "aarch64" ]; then 
        if [ $(/bin/mount | grep ${cur_part} | wc -l) -gt 0 ] ; then
            cur_part=mmcblk0p6
        fi
    else
        cur_part=`ls -lR /dev | grep -w nvm_temp | cut -d '>' -f 2 | tr -d ' '`
    fi
    dft_ver_file=${BMC_MOUNT_PATH}/opt/pme/conf/profile/platform.xml
    mount -t ext4 -o ro /dev/${cur_part} ${BMC_MOUNT_PATH}
    if [ $? -ne 0 ];then
        date >> $BMC_CB_RESULT
        echo "mount $1 fail" > $BMC_CB_RESULT
        return 1
    fi
    CB_FW_FLAG=` cat ${dft_ver_file} |grep -w IsCustomBrandFW | awk -F '>' '{print $3}' | awk -F '<' '{print $1}'`
    echo "${CB_FW_FLAG}" >> $BMC_IS_CB_FW_FILE
    umount $BMC_MOUNT_PATH >> $BMC_CB_RESULT
    if [ $? -ne 0 ];then
        date >> $BMC_CB_RESULT
        echo "umount $cur_part fail" > $BMC_CB_RESULT
        return 2
    fi
    date >> $BMC_CB_RESULT
    echo "get bmc custom flag successfully" >> $BMC_CB_RESULT
}

if [ ! -e $BMC_MOUNT_PATH ];then
    mkdir -p $BMC_MOUNT_PATH
fi

if [ ! -e $BMC_CB_FLAG_PATH ];then
    mkdir -p $BMC_CB_FLAG_PATH
fi

if [ -e $BMC_CB_RESULT ];then
    rm -rf $BMC_CB_RESULT
fi

if [ -e $BMC_IS_CB_FW_FILE ];then
    rm -rf $BMC_IS_CB_FW_FILE
fi


get_cb_flag


rm -rf $BMC_MOUNT_PATH
