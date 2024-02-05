#!/bin/sh
UPGRAD_LOG=/var/log/bmc_upgrade_log
MMCBLK_MOUNT_TMP=/dev/shm/upgrade/mount_tmp_dir
CGROUP_DIR=/dev/shm/upgrade/cgroup

echo "" > $UPGRAD_LOG
chmod 640 $UPGRAD_LOG
date >> $UPGRAD_LOG
echo "begin to extract img file" >> $UPGRAD_LOG

mkdir $CGROUP_DIR >> $UPGRAD_LOG
/bin/mount -t cgroup -o memory memcfg $CGROUP_DIR >> $UPGRAD_LOG
if [ $? -ne 0 ];then
    date >> $UPGRAD_LOG
    echo "mount cgroup fail" >> $UPGRAD_LOG
    return 1
fi
mkdir $CGROUP_DIR/upgrade_mem >> $UPGRAD_LOG
echo 10M > $CGROUP_DIR/upgrade_mem/memory.limit_in_bytes >> $UPGRAD_LOG
path=$CGROUP_DIR/upgrade_mem/tasks
if [[ $(readlink -f $path) != $path ]];then
        echo "$path is soft link."
        exit 1
fi
echo $$ > $path >> $UPGRAD_LOG
/bin/tar xvf /dev/shm/upgrade/rootfs_img.tar.gz -O rootfs_iBMC.img > /dev/nvm_temp
if [ $? -ne 0 ];then
    date >> $UPGRAD_LOG
    echo "tar rootfs img to flash fail" >> $UPGRAD_LOG
    umount $CGROUP_DIR >> $UPGRAD_LOG
    rm -rf $CGROUP_DIR >> $UPGRAD_LOG
    return 1
fi
umount $CGROUP_DIR >> $UPGRAD_LOG
rm -rf $CGROUP_DIR >> $UPGRAD_LOG
mkdir $MMCBLK_MOUNT_TMP >> $UPGRAD_LOG
chmod 750 $MMCBLK_MOUNT_TMP >> $UPGRAD_LOG
/bin/mount -t ext4 -o ro  /dev/nvm_temp $MMCBLK_MOUNT_TMP >> $UPGRAD_LOG
if [ $? -ne 0 ];then
    date >> $UPGRAD_LOG
    echo "mount /dev/nvm_temp fail" >> $UPGRAD_LOG
    return 1
fi
umount $MMCBLK_MOUNT_TMP >> $UPGRAD_LOG
date >> $UPGRAD_LOG
echo "extract img file sucessfully" >> $UPGRAD_LOG
return 0
