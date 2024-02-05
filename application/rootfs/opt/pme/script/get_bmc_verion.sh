#!/bin/sh
BMC_MOUNT_PATH=/dev/shm/bmc_mount
BMC_VER_PATH=/var/bmc_ver_info
BMC_BAK_VER_FILE=$BMC_VER_PATH/bmc_bak_verion
BMC_TEMP_VER_FILE=$BMC_VER_PATH/bmc_temp_verion
BMC_TEMP_BUILD_NO=$BMC_VER_PATH/bmc_temp_build_num
BMC_VER_RESULT=$BMC_VER_PATH/bmc_verion_result

get_bmc_verion()
{
    mount -t ext4 -o ro $1 $BMC_MOUNT_PATH
    if [ $? -ne 0 ];then
        date >> $BMC_VER_RESULT
        echo "mount $1 fail" >> $BMC_VER_RESULT
        return 1
    fi
    if [ "$3" == "0" ];then
        VER_STR=`cat $BMC_MOUNT_PATH/opt/pme/conf/profile/platform.xml | grep PMEVer`
        else
        VER_STR=`cat $BMC_MOUNT_PATH/opt/pme/conf/profile/platform.xml | grep \"BuildNumber\"`
    fi

    if [ $? -ne 0 ];then
        date >> $BMC_VER_RESULT
        echo "get bmc $1 ver fail" >> $BMC_VER_RESULT
        echo "null" > $2
        umount $BMC_MOUNT_PATH >> $BMC_VER_RESULT
        if [ $? -ne 0 ];then
            date >> $BMC_VER_RESULT
            echo "umount $1 fail" >> $BMC_VER_RESULT
            return 2
        fi
        return 1
    fi
    VER_STR=${VER_STR#*<V>}
    VER_STR=${VER_STR%</V>*}
    echo $VER_STR > $2
    umount $BMC_MOUNT_PATH >> $BMC_VER_RESULT
    if [ $? -ne 0 ];then
        date >> $BMC_VER_RESULT
        echo "umount $1 fail" >> $BMC_VER_RESULT
        return 2
    fi
    date >> $BMC_VER_RESULT
    echo "get bmc $1 ver successful" >> $BMC_VER_RESULT
}

if [ ! -e $BMC_MOUNT_PATH ];then
    mkdir -p $BMC_MOUNT_PATH
fi

if [ ! -e $BMC_VER_PATH ];then
    mkdir -p $BMC_VER_PATH
fi

if [ -e $BMC_BAK_VER_FILE ];then
    rm -rf $BMC_BAK_VER_FILE
fi

if [ -e $BMC_TEMP_VER_FILE ];then
    rm -rf $BMC_TEMP_VER_FILE
fi

if [ -e $BMC_VER_RESULT ];then
    rm -rf $BMC_VER_RESULT
fi

get_bmc_verion /dev/nvm_gold $BMC_BAK_VER_FILE 0
if [ $? -eq 2 ];then
    return 1
fi
chmod 640 $BMC_VER_RESULT
chmod 640 $BMC_BAK_VER_FILE

get_bmc_verion /dev/nvm_temp $BMC_TEMP_VER_FILE 0
if [ $? -eq 2 ];then
    return 1
fi
chmod 640 $BMC_TEMP_VER_FILE

get_bmc_verion /dev/nvm_temp $BMC_TEMP_BUILD_NO 1
if [ $? -eq 2 ];then
    return 1
fi
chmod 640 $BMC_TEMP_BUILD_NO
rm -rf $BMC_MOUNT_PATH

chmod -R 640 $BMC_VER_PATH
