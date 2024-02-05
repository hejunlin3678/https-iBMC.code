chmod +x *
############################################
# bash script: BMC升级脚本
############################################
#把项目编码赋值好
project_code=$1
#命令失败时的重试次数 和间隔 
REPEAT=6
REPEAT_TIME=1 

SAVEFILE=$project_code"_save.txt"
G_WORK_DIR=`pwd`

error_quit()
{
    exit 1
}

ok_quit()
{
    exit 0
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

#命令执行函数  参数为：命令  正确返回值  保存的文件（temp.txt）
run_cmd()
{
    for((j=0;j<$REPEAT;j++))
    do
        ipmicmd -k "$1" smi 0 >temp.txt
        grep -wq "$2" temp.txt
        if [ $? = 0 ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    
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
	fi
}

#关闭黑匣子
close_black_box()
{
    #关闭黑匣子
	run_cmd "f 0 30 93 db 07 00 0x0f 0x0c 0x00 0x01 0x00 0x00 0x00" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
	   echo "close balck box fail"
	   echo "close balck box fail" >>$SAVEFILE 
	   error_quit
	else
	   echo "close balck box ok!"
	   echo "close balck box ok!" >>$SAVEFILE
	   return 0
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
	
	#删除判断文件避免残留影响二次判断
	rm -rf verlog.txt
	rm -rf sp5.txt
	rm -rf sp8.txt
	
	#查看BMC的black_box_state，看黑匣子是否打开
	run_cmd "f 0 30 93 db 07 00 0x10 0x0c" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
	   echo "get black box state fail"
	   echo "get black box state fail" >>$SAVEFILE
	   error_quit
	else
		status=`cat temp.txt |awk -F " " '{print $10}' `
		if [ $((16#$status)) -eq 1 ] ; then
		   echo "The black box state is open"
		   echo "The black box state is open" >>$SAVEFILE
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
	
	#避免其它模块驱动跟BMC驱动不一致导致host_cdev_drv.ko安装失败
	rmmod host_cdev_drv.ko >/dev/null 2>&1
	rmmod host_veth_drv.ko >/dev/null 2>&1
	rmmod host_edma_drv.ko >/dev/null 2>&1

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
	
	# 判断是否安装了cdev驱动
	str=`lsmod |grep host_cdev_drv`
	if [ -z "$str" ] ; then	
		# 判断host_cdev_drv文件是否存在
		if [ ! -f "$cdev_path" ] ; then
		   echo "host_cdev_drv file is not present!"
		   echo "host_cdev_drv file is not present!" >>$SAVEFILE
		   error_quit
		fi 
		
		cd `dirname ${cdev_path}`
		insmod host_cdev_drv.ko
		if [ $? != 0 ] ; then
			echo "insmod cdev driver failed!"
			echo "insmod cdev driver failed!" >>$SAVEFILE
			error_quit
		fi
	fi
	
	cd ${G_WORK_DIR}
	ifconfig veth up

	echo "load driver ok!"
	ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
	return 0
}

#发送06 01命令，测试通道
get_pcie_status_from_cdev()
{
	old_count=`cat /sys/module/host_edma_drv/parameters/statistics  |grep recv_bytes |awk  -F ':'  '{print $2}'`

	for((j=0;j<$REPEAT;j++))
	do
		`echo -e "\x01\x03\x18\x00\x01">/dev/hwibmc0`
		sleep 10
		new_count=`cat /sys/module/host_edma_drv/parameters/statistics  |grep recv_bytes |awk  -F ':'  '{print $2}'`
		if [ $new_count -gt $old_count ] ; then
			echo "get pcie status from cdev ok!"
			echo "get pcie status from cdev ok!" >>$SAVEFILE
			return 0
		fi
		sleep $REPEAT_TIME
		echo "get pcie status from cdev failed, retry."
		echo "get pcie status from cdev failed, retry." >>$SAVEFILE
	done
	echo "get pcie status from cdev failed!"
	echo "get pcie status from cdev failed!" >>$SAVEFILE
	return 1

}

#ping通道是否打开
get_pcie_status_from_network()
{
	for((j=0;j<$REPEAT;j++))
	do
		str=`ping6 fe80::9e7d:a3ff:fe28:6ffa%veth -c 5 | grep transmitted 2>/dev/null`
		send_data=`echo $str | awk -F " " '{print $1}'`
		recv_data=`echo $str | awk -F " " '{print $4}'`

		if [ -n "$send_data" -a $send_data = $recv_data ] ; then	
			echo "get pcie status from network ok!" 
			echo "get pcie status from network ok!" >>$SAVEFILE
			return 0
		fi
		sleep $REPEAT_TIME
		echo "get pcie status from network failed, retry." 
		echo "get pcie status from network failed, retry." >>$SAVEFILE
	done
	echo "get pcie status from network failed!" 
	echo "get pcie status from network failed!" >>$SAVEFILE
	return 1
}


############################################################################
#########################程序开始运行#####################################
############################################################################
#清空文件 写入当前时间
echo "" >$SAVEFILE
date >>$SAVEFILE

#加载驱动
load_driver

#获取网络状态
get_pcie_status_from_network
if [ $? -ne 0 ]; then
	error_quit	
fi

#获取cdev状态
get_pcie_status_from_cdev
if [ $? -ne 0 ]; then
	error_quit	
fi

ok_quit

