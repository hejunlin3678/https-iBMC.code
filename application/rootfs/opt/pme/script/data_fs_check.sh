#!/bash/bin

TMP_DIR=/dev/shm/datafs_bk
DF_DATA_FS=/usr/upgrade/datafs.tar.gz
DATA_DIR=/data
LIST_FILE=/usr/upgrade/bkFileList
LOG=/data/var/log/datafs_log

recover_datafs() {
    mkdir $TMP_DIR
    while read line
    do
        cp -a --parent $line $TMP_DIR
    done < $LIST_FILE
    tar -zxvf $DF_DATA_FS -C $TMP_DIR 1>/dev/null 2>&1

    /bin/umount /data
    if [ $? -ne 0 ]; then
    	echo "Recover datafs failed(umount datafs failed)"
	exit 0
    fi
    echo y | /sbin/mkfs.ext4 /dev/$1
    if [ $? -ne 0 ]; then
    	echo "Recover datafs failed(format datafs failed)"
	exit 0
    fi
    /bin/mount -t ext4  -rw /dev/$1 /data
    if [ $? -ne 0 ]; then
    	echo "Recover datafs failed(mount datafs failed)"
	exit 0
    fi
    cp -arf $TMP_DIR/datafs/* $DATA_DIR/
    cp -arf $TMP_DIR/data/* $DATA_DIR/
    rm -rf $TMP_DIR
}

echo "Recover datafs begin"
grep 'extroot=/dev/mtdblock' /proc/cmdline
if [ $? == 0 ] ;then
	exit 0
else
	UBOOT_RECOVER_FLAG=0
	DATAFS_DEV="mmcblk0p7"
	if [ `uname -m`x == "aarch64x" ]; then 
		DATAFS_DEV="mmcblk0p1"
	fi
	UBOOT_RECOVER_FLAG=`cat /proc/sys_info/datafs_flag | tr -d "\r\n"`
	if [ 1 == $UBOOT_RECOVER_FLAG ]; then
		recover_datafs $DATAFS_DEV
		echo "Recover datafs success"
		out_t=$(date +'%Y-%m-%d %H:%M:%S')
		echo "$out_t Recover datafs: success" >> $LOG
	else
		echo "Recover datafs no need ubootrecoverflag=$UBOOT_RECOVER_FLAG"
	fi

fi

