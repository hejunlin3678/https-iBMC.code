chmod +x *
############################################
# bash script: BMC升级脚本
############################################
#把项目编码赋值好
project_code=$1
#命令失败时的重试次数 和间隔 
REPEAT=6
REPEAT_TIME=1 
PRODUCT_VERSION_NUM=03
FLAG_ADD_USER=0
IPMI_CMD_RETURN_FILE=temp.txt

SAVEFILE=$project_code"_save.txt"

G_WORK_DIR=`pwd`

source ./add_temp_user.sh

#打开DFT接口
DFT_enable()
{
    run_cmd  "f 0 30 90 20 db 07 00 01"  "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo "enable DFT fail!"
        echo "enable DFT fail!" >>$SAVEFILE
        exit 1
    else 
        echo "enable DFT ok!"
        return 0
    fi
}

#关闭DFT接口
DFT_disable()
{
    run_cmd  "f 0 30 90 20 db 07 00 00"  "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo "disable DFT fail!"
        echo "disable DFT fail!" >>$SAVEFILE
        return 1
    else 
        echo "disable DFT ok!"
        return 0
    fi
} 

error_quit()
{
    DFT_disable
    #若前面添加了临时用户，则要删除
    if [ $FLAG_ADD_USER -eq 1 ] ; then
        restore_test_admin_user 17
        echo "del the test user"
    fi
    exit 1
}

ok_quit()
{
    DFT_disable
    #若前面添加了临时用户，则要删除
    if [ $FLAG_ADD_USER -eq 1 ] ; then
        restore_test_admin_user 17
        echo "del the test user"
    fi
    exit 0
}

echo_and_save_fail()
{
    echo -e "\033[31;1m$1\033[0m" | tee -a $SAVEFILE
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

#命令执行函数  参数为：命令  正确返回值  保存的文件（$IPMI_CMD_RETURN_FILE）
run_cmd()
{
    for((j=0;j<$REPEAT;j++))
    do
        ipmicmd -k "$1" smi 0 >$IPMI_CMD_RETURN_FILE
        grep -wq "$2" $IPMI_CMD_RETURN_FILE
        if [ $? = 0 ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    
    return 1
}

#设置备份点
set_bak_point()
{
    local REPEAT_TIME_tmp=$REPEAT_TIME
    local REPEAT_tmp=$REPEAT
    REPEAT_TIME=30
    REPEAT=30
    sleep 30
    echo "Before set_bak_point : REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
    #此命令最后使用  把配置信息备份起来
    run_cmd "f 0 30 93 db 07 00 07 00 aa" "0f 31 00 93 00 db 07 00 01"
    if [ $? != 0 ] ; then
        echo_fail "Set bakpoint fail!"
        echo "Set bakpoint fail!" >>$SAVEFILE
        REPEAT_TIME=$REPEAT_TIME_tmp
        REPEAT=$REPEAT_tmp
        echo "After set_bak_point : REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
        error_quit
    else
        echo_success "Set bakpoint success!"
        echo "Set bakpoint success!" >>$SAVEFILE
        REPEAT_TIME=$REPEAT_TIME_tmp
        REPEAT=$REPEAT_tmp
        echo "After set_bak_point : REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
    fi
	

    SET_BAK_POINT_STATE=0
    SET_BAK_POINT_STATE_TRY_COUNT=0

    while [[ "$SET_BAK_POINT_STATE_TRY_COUNT" -lt 15 ]]
    do
    SET_BAK_POINT_STATE_TRY_COUNT=$(($SET_BAK_POINT_STATE_TRY_COUNT+1))

    run_cmd "f 0 30 93 db 07 00 07 00 55" ""
    if [ $? != 0 ] ; then
        echo "Get set_bak_point error: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
        error_quit
    else
        tmp_ret=`cat $IPMI_CMD_RETURN_FILE`
        echo $tmp_ret
        SET_BAK_POINT_STATE=${tmp_ret:24:2}
        echo $SET_BAK_POINT_STATE
        if [ "$SET_BAK_POINT_STATE" -eq 1 ];then
             echo "Get set_bak_point success: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
             return 0
        elif [ "$SET_BAK_POINT_STATE" -eq 2 ];then
             echo "Get set_bak_point failure 2: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
             error_quit	 
        fi
    fi
    sleep 2
    done

    echo "Get set_bak_point failure: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
    error_quit
}

get_product_version_num()
{
	ipmicmd -k "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 07 00" smi 0 > product_version_num.txt
	if [ $? -eq 0 ] ; then
		PRODUCT_VERSION_NUM=`cat product_version_num.txt | awk '{print $NF}'`
	fi
	#返回值ff表示获取产品版本的命令还不支持，属性还没有添加，这个肯定是V3的产品
	if [ "$PRODUCT_VERSION_NUM" == "ff"  -o "$PRODUCT_VERSION_NUM" == "FF" ] ; then
		PRODUCT_VERSION_NUM=03
	fi		
	echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM"
	echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM" >>$SAVEFILE
	rm -f product_version_num.txt
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
	cat /etc/*-release > verlog.txt	
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
	
	#匹配Kylin V10
	grep V10 verlog.txt > V10.txt
	if test -s V10.txt; then
		cat /etc/kylin-release
		edma_path=${G_WORK_DIR}/driver/kylinv10/host_edma_drv.ko 
		veth_path=${G_WORK_DIR}/driver/kylinv10/host_veth_drv.ko 
		cdev_path=${G_WORK_DIR}/driver/kylinv10/host_cdev_drv.ko
	fi
	#删除判断文件避免残留影响二次判断
	rm -rf verlog.txt
	rm -rf sp5.txt
	rm -rf sp8.txt
	rm -rf V10.txt
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

#传输文件
send_file()
{
	bmc_ip=$1
	src_file=$2
	des_path=$3
	user="root"
	pwd="Huawei12#$"

	if [ $PRODUCT_VERSION_NUM -gt 03 ];then
		user="Administrator"
		pwd="Admin@9000"
	fi
	if [ $# -lt 3 ] ; then
	   echo "Parameter error!"
	   echo "Parameter error!" >>$SAVEFILE
	   return 1
	fi
	#先校验默认用户名和密码是否可用，若不可用，添加临时用户名和密码
	check_user2_default_name_pwd
	if [ $? != 0 ] ; then
	    echo "need to add a test user"
	    add_test_admin_user
	    user="DFTUpgradeTest"
	    pwd="Admin@9000"
	    FLAG_ADD_USER=1
	fi
	sleep 10
	expect << EOF
	set timeout 300
	spawn scp -6r -l 4096 ${src_file} ${user}@${bmc_ip}:${des_path}
	expect {
	"*yes/no" { send "yes\r"; exp_continue }
	"*password:" { send "$pwd\r" }
	}

	expect eof
EOF

	return 0
}

#启动升级 如果发升级命令失败，先发送finsh命令，延时，然后再重发升级命令，重发3次
initiate_bmc_upgrade()
{
	for((i=0;i<3;i++))
	{
		run_cmd "f 0 30 91 db 07 00 06 AA 00 01 00 0E 2F 74 6D 70 2F 69 6D 61 67 65 2E 68 70 6D" "0f 31 00 91 00 db 07 00"
		if [ $? != 0 ] ; then
		   echo "The $i time initiate BMC upgrade fail"
		   
		   #finish upgrade
		   run_cmd "f 0 30 91 db 07 00 06 55" "0f 31 00 91 00 db 07 00"
		   if [ $? != 0 ] ; then
			   echo "The $i time finish BMC upgrade fail"
			   return 1
			else
			   echo "The $i time finish BMC upgrade ok"
			   sleep 1
		   fi
		else
		   echo "initiate BMC upgrade $getversion ok"
		   return 0
		fi
	}
	
	return 1
}

#升级进度查询 连续6次查询不到进度，返回错误
get_bmc_upgrade_status()
{
	error_counter=0
	
    for((i=0;i<600;i++))
	{
		run_cmd "f 0 30 91 db 07 00 06 00" "0f 31 00 91 00 db 07 00"
		if [ $? != 0 ] ; then
			((error_counter++))
			if [ $error_counter -eq 6 ] ; then
				echo "The $i time get BMC upgrade status fail cnt: $error_counter"
				echo "The $i time get BMC upgrade status fail cnt: $error_counter" >>$SAVEFILE
				return 1
			fi
		else
		    error_counter=0
		fi
		
		status=`cat $IPMI_CMD_RETURN_FILE |awk -F " " '{print $9}' `
		
		if [ $((16#$status)) -eq 228 ] ; then
		   echo "BMC upgrade status is 100%"
		   echo "BMC upgrade status is 100%" >>$SAVEFILE
		   return 0
		else
		   echo "The $i time get BMC upgrade status is $((16#$status))%"
		fi
		
		sleep 5
	}
	
	echo "get BMC upgrade status overtime" 
	echo "get BMC upgrade status overtime" >>$SAVEFILE
	return 1
}

#传输文件，升级，查询进度
start_upgrade()
{
    #查询升级文件是否存在
	hpm_file_path=${G_WORK_DIR}/image.hpm
	if [ ! -f "$hpm_file_path" ] ; then
	   echo "hpm file is not present!"
	   echo "hpm file is not present!" >>$SAVEFILE
	   error_quit
	fi
	
    #传输文件
	send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" image.hpm /tmp
	if [ $? != 0 ] ; then	 
	   echo "send hpm file fail!"
	   echo "send hpm file fail!" >>$SAVEFILE
	   return 1	
	fi
	
	#启动升级
	initiate_bmc_upgrade
	if [ $? != 0 ] ; then
       echo "init bmc upgrade fail" >>$SAVEFILE
	   return 1	
	fi
	
	#查询进度
	get_bmc_upgrade_status
	if [ $? != 0 ] ; then	   
	   return 1	
	fi
	
	return 0
}

#复位BMC
reset_bmc()
{
    echo "please wait for 6 second and reset bmc!"
    for((i=0;i<6;i++))
    do
	    echo -n -e "\b\b$i"
	    sleep 1
    done

    for((i=0;i<3;i++))
    do
        run_cmd "f 0 06 02" "0f 07 00 02 00"
        if [ $? -ne 0 ] ; then
            sleep 1
            continue
        fi
        break
    done

    if [ $i -eq 3 ] ; then
        echo_fail "BMC reset test Fail"
        echo "BMC reset test Fail" >>$SAVEFILE
        return 1
    fi

    echo "please wait for 90 second and reset bmc!"
    for((i=0;i<90;i++))
    do
	    echo -n -e "\b\b$i"
	    sleep 1
    done

    for((i=0;i<12;i++))
    {
        run_cmd "f 0 06 01" "0f 07 00 01 00"
        if [ $? -ne 0 ] ; then
            echo "please wait for 10 second!"
            sleep 10
        else 
            break
        fi
    }

    if [ $i -eq 12 ] ; then
        echo_fail "BMC reset test Fail"
        echo "BMC reset test Fail" >>$SAVEFILE
        return 1
    fi

    echo_success "BMC reset ok"
    echo "BMC reset ok" >>$SAVEFILE

    return 0
}

#hpm加载bmc软件
hpm_upgrade()
{
	#安装驱动
	load_driver

	for((k=0;k<$REPEAT;k++))
	{
		start_upgrade
		if [ $? != 0 ] ; then
			reset_bmc
			if [ $? != 0 ] ; then
				return 1
			fi
		else
			echo "hpm load BMC ok"
			return 0
		fi
	}
	echo "hpm rest BMC fail $k" 
	echo "hpm rest BMC fail $k" >>$SAVEFILE
	return 1
}

############################################################################
#########################程序开始运行#####################################
############################################################################
#先设置配置  后重启BMC  最后读取校验

#清空文件 写入当前时间
echo "" >$SAVEFILE
date >>$SAVEFILE

#获取版本信息
get_product_version_num
#开启dft验证
DFT_enable
sleep 1

hpm_upgrade
#升级白牌失败，直接退出
if [ $? != 0 ] ; then
    echo_and_save_fail "hpm upgrade fail"
    error_quit
fi

#升级白牌包后可能修改BMC配置，需要重新设置还原点
set_bak_point

ok_quit

