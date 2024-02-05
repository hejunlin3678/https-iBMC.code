#!/bin/sh

REPEAT=6
REPEAT_TIME=1
MAX_STRING_NUM=24
IPMI_CMD_LOG_FILE=custom.log
IPMI_CMD_RETURN_FILE=ipmi_cmd_return_string.txt
SAVEFILE=05024EBS_save.txt
FLAG_ADD_USER=0
ENVIRONMENT_TYPE=0
EQUIPMENT_SEPARATION_SUPPORT_STATE=0
G_WORK_DIR=`pwd`


source ./add_temp_user.sh

function clear_operation()
{
    if [ ${FLAG_ADD_USER} -eq 1 ];
    then
        restore_test_admin_user 17
        echo "Delete the test user successfully."

    fi
}

function error_quit()
{
    clear_operation
    exit 1
}

function ok_quit()
{
    clear_operation
    exit 0
}

echo_fail()
{
	echo -e "\033[31;1m$1\033[0m"
}

echo_success()
{
	echo -e "\033[32;1m$1\033[0m"
}


#命令执行函数  参数为：命令  正确返回值  保存的文件（$IPMI_CMD_RETURN_FILE）
run_cmd()
{
    local i
    echo "ipmicmd -k \"$1\" smi 0 : $2" >>$IPMI_CMD_LOG_FILE
    for((i=0;i<$REPEAT;i++))
    do
        ipmicmd -k "$1" smi 0 > $IPMI_CMD_RETURN_FILE
        grep -wq "$2" $IPMI_CMD_RETURN_FILE
        if [ $? = 0 ] ; then
		    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
            return 0
        fi
        sleep $REPEAT_TIME
    done

    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
    return 1
}

#打开黑匣子
open_black_box()
{
	#打开黑匣子
	run_cmd "f 0 30 93 db 07 00 0x0f 0x0c 0x00 0x00 0x00 0x00 0x01" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo "open balck box fail"
		echo "open balck box fail" >>$SAVEFILE
		error_quit
	else
		echo "open black box restart OS now!"
		echo "open black box restart OS now!" >>$SAVEFILE
		reboot
		ok_quit
	fi
}

#加载驱动
load_driver()
{
	driver_path=${G_WORK_DIR}/driver.tar.gz
	cat /etc/euleros-release > verlog.txt
	grep SP5 verlog.txt > sp5.txt
	if test -s sp5.txt; then
		cat /etc/euleros-release
		edma_path=${G_WORK_DIR}/driver/euler2sp5/host_edma_drv.ko
		veth_path=${G_WORK_DIR}/driver/euler2sp5/host_veth_drv.ko
		cdev_path=${G_WORK_DIR}/driver/euler2sp5/host_cdev_drv.ko
	fi

	grep SP8 verlog.txt > sp8.txt
	if test -s sp8.txt; then
		cat /etc/euleros-release
		edma_path=${G_WORK_DIR}/driver/euler2sp8/host_edma_drv.ko
		veth_path=${G_WORK_DIR}/driver/euler2sp8/host_veth_drv.ko
		cdev_path=${G_WORK_DIR}/driver/euler2sp8/host_cdev_drv.ko
	fi

    cat /etc/centos-release > verlog.txt
	grep "8.2" verlog.txt > cent82.txt
	if test -s cent82.txt; then
		cat /etc/centos-release
		edma_path=${G_WORK_DIR}/driver/centos82/host_edma_drv.ko
		veth_path=${G_WORK_DIR}/driver/centos82/host_veth_drv.ko
		cdev_path=${G_WORK_DIR}/driver/centos82/host_cdev_drv.ko
	fi

	#删除判断文件避免残留影响二次判断
	rm -rf verlog.txt
	rm -rf sp5.txt
	rm -rf sp8.txt
	rm -rf cent82.txt

	#查看BMC的black_box_state，看黑匣子是否打开
	run_cmd "f 0 30 93 db 07 00 0x10 0x0c" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
	   echo "get black box state fail"
	   echo "get black box state fail" >>$SAVEFILE
	   error_quit
	else
		status=`cat $IPMI_CMD_RETURN_FILE |awk -F " " '{print $10}' `
		if [ $((16#$status)) -eq 1 ] ; then
		   echo "The black box state is open"
		else
		   open_black_box
		fi
	fi

	#查看OS测黑匣子是否打开
	str=`lspci | grep 1710`
	if [ -z "$str" ] ; then
		echo "pcie device is not present!"
		echo "pcie device is not present!" >>$SAVEFILE
		open_black_box
	else
		echo "pcie device is present!"
	fi

	#查看driver.tar.gz文件是否存在
	if [ ! -f "$driver_path" ] ; then
		echo "driver.tar.gz file is not present!"
		echo "driver.tar.gz file is not present!" >>$SAVEFILE
		error_quit
	else
		rm -rf driver
		tar zxf driver.tar.gz
	fi

	# 判断是否安装了edma驱动
	str=`lsmod |grep host_edma_drv`
	if [ -z "$str" ] ; then
		# 判断host_edma_drv文件是否存在
		if [ ! -f "$edma_path" ] ; then
		   echo "host_edma_drv file is not present!"
		   echo "host_edma_drv file is not present!" >>$SAVEFILE
		   error_quit
		fi

		cd `dirname ${edma_path}`
		insmod host_edma_drv.ko
		if [ $? != 0 ] ; then
			echo "insmod edma driver failed!"
			echo "insmod edma driver failed!" >>$SAVEFILE
			error_quit
		fi
	fi

	# 判断是否安装了veth驱动
	str=`lsmod |grep host_veth_drv`
	if [ -z "$str" ] ; then
		# 判断host_veth_drv文件是否存在
		if [ ! -f "$veth_path" ] ; then
		   echo "host_veth_drv file is not present!"
		   echo "host_veth_drv file is not present!" >>$SAVEFILE
		   error_quit
		fi

		cd `dirname ${veth_path}`
		insmod host_veth_drv.ko
		if [ $? != 0 ] ; then
			echo "insmod veth driver failed!"
			echo "insmod veth driver failed!" >>$SAVEFILE
			error_quit
		fi
	fi

	cd ${G_WORK_DIR}
	ifconfig veth up

	echo "load driver ok!"
	return 0
}

# 升级带TCM使能的伙伴根efuse
upgrade_efuse_function()
{
    ./upgrade_hpm.sh "efuse-crypt-image-partner-tcm.hpm" 0
    if [ $? -ne 0 ]; then
        echo_fail "upgrade new efuse fail"
        echo "upgrade new efuse fail" >>$SAVEFILE
        error_quit
    else
        echo_success "upgrade new efuse success"
        echo "upgrade new efuse success" >>$SAVEFILE
        # 退出后ac
        ipmitool raw 0x30 0x90 0x20 0xdb 0x07 0x00 0x01
        ipmitool raw 0x30 0x90 0x31 0x00 0x03
    fi
}

# 判断是否有efuse，如果无efuse则进行升级
upgrade_efuse()
{
    echo "start get efuse state"
    # 如果返回值为00，则代表不含有efuse
    run_cmd "0f 00 30 93 db 07 00 5b 2c 00" "0f 31 00 93 00 db 07 00 00"
    if [ $? == 0 ]; then
        upgrade_efuse_function
    else
        echo "efuse has already existed"
        echo "efuse has already existed" >>$SAVEFILE
    fi
}

chmod +x conver
chmod +x *

UserName="Administrator"
Password="Admin@9000"
IPV6_ADDR="fe80::9e7d:a3ff:fe28:6ffa%veth"
produce_name=""
machine_uid=""

load_driver
# 判断是否有efuse，如果无efuse则升级带TCM使能的伙伴根efuse升级包
upgrade_efuse

./common_factory_restore.sh
if [ $? != 0 ] ; then
    error_quit
fi

ok_quit