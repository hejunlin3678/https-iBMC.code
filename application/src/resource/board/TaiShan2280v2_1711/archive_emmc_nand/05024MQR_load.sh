#!/bin/sh

REPEAT=6
REPEAT_TIME=1 
MAX_STRING_NUM=24
IPMI_CMD_LOG_FILE=temp.txt
IPMI_CMD_RETURN_FILE=ipmi_cmd_return_string.txt
SAVEFILE=05024MQR_save.txt
FLAG_ADD_USER=0
ENVIRONMENT_TYPE=0
EQUIPMENT_SEPARATION_SUPPORT_STATE=0
G_WORK_DIR=`pwd`

VERSIONNAME=$project_code"_version.ini"

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

# 使能dft
enable_dft_mode()
{
    # 升级dft使能hpm包
    ./upgrade_hpm.sh "dftimage.hpm" 0
    if [ $? -ne 0 ]; then
        echo_fail "upgrade dftimage hpm fail"
        echo "upgrade dftimage hpm fail" >>$SAVEFILE
        error_quit
    else
        echo_success "upgrade dftimage hpm success"
    fi
}

# 升级支持伙伴模式的efuse
upgrade_efuse_function()
{
    ./upgrade_hpm.sh "efuse-crypt-image-partner.hpm" 0
    if [ $? -ne 0 ]; then
        echo_fail "upgrade new efuse fail"
        echo "upgrade new efuse fail" >>$SAVEFILE
        error_quit
    else
        echo_success "upgrade new efuse success"
        echo "upgrade new efuse success" >>$SAVEFILE
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

# 检查清除日志的文件是否存在
check_clear_log_file()
{
    if [ ! -f ./upgrade_hpm.sh ]; then
        echo_fail "there is no upgrade_hpm.sh"
        echo "there is no upgrade_hpm.sh" >>$SAVEFILE
        error_quit
    fi

    if [ ! -f ./dftimage.hpm ]; then
        echo_fail "there is no enable dftimage.hpm"
        echo "there is no enable dftimage.hpm" >>$SAVEFILE
        error_quit
    fi
    
    if [ ! -f ./file_restore.sh ]; then
        echo_fail "there is no file_restore.sh"
        echo "there is no file_restore.sh" >>$SAVEFILE
        error_quit
    fi
}

# 清除所有的日志
clear_log()
{
    # 查看需要文件是否都存在
    check_clear_log_file
    # 使能dft
    enable_dft_mode
	clear_log_times=5
	for ((k=0; k<${clear_log_times}; k++))
	do
		# 清除日志
		source ./file_restore.sh
		if [ $? -ne 0 ]; then
			echo_fail "clear log fail"
			echo "clear log fail" >>$SAVEFILE

            sleep 30
		else
			echo_success "clear log success"
			echo "clear log success" >>$SAVEFILE
			ok_quit
		fi
	done
	error_quit
}

#参数为 输入文件  开始字节  输出文件
hextochar()
{
    temp=`cat  $1`
    temp=`echo $temp | cut -b $2-100`

    converhex="$temp"
    #清空文件
    echo "" >$3
    for k in $converhex
    do
       TEMP="\\x$k"
       echo -n -e $TEMP >>$3      
    done
}

#BMC 版本查询
active_bmc_version_query()
{
    run_cmd "f 0 30 90 08 0 1 0 10" "0f 31 00 90 00 80"
    if [ $? != 0 ] ; then
       echo "get BMC version $getversion fail"
       echo "get BMC version $getversion fail" >>$SAVEFILE
       error_quit
    fi

    hextochar $IPMI_CMD_RETURN_FILE 18 getversion.txt
    getversion=`cat getversion.txt | grep .` >/dev/null

    if [[ "$getversion" > "3.11" ]] ; then
       echo "BMC version:$getversion ok"
       echo "BMC version:$getversion ok" >>$SAVEFILE
       return 0
    else
       echo "BMC version $getversion fail"
       echo "BMC version $getversion fail" >>$SAVEFILE
       return 1

    fi
}

backup_bmc_version_query()
{
    run_cmd "f 0 30 90 08 0 0c 0 10" "0f 31 00 90 00 80"
    if [ $? != 0 ] ; then
       echo "get backup BMC version $getversion fail"
       echo "get backup BMC version $getversion fail" >>$SAVEFILE
       error_quit
    fi

    hextochar $IPMI_CMD_RETURN_FILE 18 getversion.txt
    getversion=`cat getversion.txt | grep .` >/dev/null

    if [[ "$getversion" > "3.11" ]] ; then
       echo "backup BMC version:$getversion ok"
       echo "backup BMC version:$getversion ok" >>$SAVEFILE
       return 0
    else
       echo "backup BMC version $getversion fail"
       echo "backup BMC version $getversion fail" >>$SAVEFILE
       return 1
    fi
}

#BMC 版本查询
bmc_version_query()
{
    #Active BMC version query
	active_bmc_version_query
	if [ $? -ne 0 ] ; then		
		return 1
	fi
	
	#Backup BMC version query
	backup_bmc_version_query
	if [ $? -ne 0 ] ; then		
		return 1
	fi
	
	return 0
}

chmod +x conver
chmod +x *

UserName="Administrator"
Password="Admin@9000"
IPV6_ADDR="fe80::9e7d:a3ff:fe28:6ffa%veth"
produce_name=""
machine_uid=""

load_driver

#检测版本 不一致则加载
bmc_version_query
if [ $? -ne 0 ]; then
    # 加载主区版本
    ./upgrade_hpm.sh "image.hpm" 1
    active_bmc_version_query
    if [ $? -ne 0 ]; then
       echo "BMC version is error after image1 upgrade"
       echo "BMC version is error after image1 upgrade" >>$SAVEFILE
       error_quit
    fi  
    sleep 10

    # 加载主区版本
    ./upgrade_hpm.sh "image.hpm" 1
    active_bmc_version_query
    if [ $? -ne 0 ]; then
       echo "BMC version is error after image1 upgrade"
       echo "BMC version is error after image1 upgrade" >>$SAVEFILE
       error_quit
    fi  
    sleep 10
fi

# 判断是否有efuse，如果无efuse则升级支持伙伴模式efuse
upgrade_efuse

ok_quit