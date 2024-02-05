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
SCP_MAX_RETRY_TIME=10
SCP_RETRY_INTERVAL=10
IPMI_CMD_RETURN_FILE=temp.txt

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

echo_success()
{
	echo -e "\033[32;1m$1\033[0m"
}

echo_fail()
{
	echo -e "\033[31;1m$1\033[0m"
}

echo_and_save_fail()
{
    echo -e "\033[31;1m$1\033[0m" | tee -a $SAVEFILE
}

echo_and_save_success()
{
    echo -e "\033[32;1m$1\033[0m" | tee -a $SAVEFILE
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
#打开DFT接口
DFT_enable()
{
    run_cmd  "f 0 30 90 20 db 07 00 01"  "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo "enable DFT fail!"
        echo "enable DFT fail!" >>$SAVEFILE
        error_quit
    else 
        echo "enable DFT ok!"
        return 0
    fi
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
	
	#删除判断文件避免残留影响二次判断
	rm -rf verlog.txt
	rm -rf sp5.txt
	rm -rf sp8.txt
	
	#查看BMC的black_box_state，看黑匣子是否打开
	run_cmd "f 0 30 93 db 07 00 0x10 0x0c" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
	   echo_and_save_fail "get black box state fail"
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
		echo_and_save_fail "pcie device is not present!"
		open_black_box
	else
		echo "pcie device is present!"
	fi
	
	#查看driver.tar.gz文件是否存在
	if [ ! -f "$driver_path" ] ; then
		echo_and_save_fail "driver.tar.gz file is not present!"
		error_quit
	else
		rm -rf driver
		tar zxf driver.tar.gz 
	fi  
	
	rmmod host_cdev_drv.ko
	rmmod host_veth_drv.ko
	rmmod host_edma_drv.ko
	cd `dirname ${edma_path}`
	insmod host_edma_drv.ko
	cd `dirname ${veth_path}`
	insmod host_veth_drv.ko
	cd `dirname ${cdev_path}`
	insmod host_cdev_drv.ko
	
	# 判断是否安装了edma驱动
	str=`lsmod |grep host_edma_drv`
	if [ -z "$str" ] ; then
		# 判断host_edma_drv文件是否存在
		if [ ! -f "$edma_path" ] ; then
		   echo_and_save_fail "host_edma_drv file is not present!"
		   error_quit
		fi  
		
		cd `dirname ${edma_path}`
		insmod host_edma_drv.ko
		if [ $? != 0 ] ; then
			echo_and_save_fail "insmod edma driver failed!"
			error_quit
		fi
	fi
	
	# 判断是否安装了veth驱动
	str=`lsmod |grep host_veth_drv`
	if [ -z "$str" ] ; then	
		# 判断host_veth_drv文件是否存在
		if [ ! -f "$veth_path" ] ; then
		   echo_and_save_fail "host_veth_drv file is not present!"
		   error_quit
		fi 
		
		cd `dirname ${veth_path}`
		insmod host_veth_drv.ko
		if [ $? != 0 ] ; then
			echo_and_save_fail "insmod veth driver failed!"
			error_quit
		fi
	fi
	
	cd ${G_WORK_DIR}
	ifconfig veth up

	echo "load driver ok!"
	ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
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
	   echo_and_save_fail "Parameter error!"
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
	# 增加重试等待sshd服务完全起来以及新增的临时用户生效，同时解决密码错误导致再次输入卡死300s的问题
	for((i=0;i<$SCP_MAX_RETRY_TIME;i++)) {
		expect << EOF
		set timeout 300
		spawn scp -6r -l 4096 ${src_file} ${user}@${bmc_ip}:${des_path}
		expect {
		"*yes/no" { send "yes\r"; exp_continue }
		"*password:" { send "$pwd\r" }
		"*Password:" { send "$pwd\r" }
		}
	
		expect {
		"*password:" {exit 2}
		"*Password:" {exit 2}
		}

EOF
		if [ $? -ne 0 ] ; then
			echo "sshd was not ready" | tee -a >>$SAVEFILE
			sleep $SCP_RETRY_INTERVAL
			continue
		else 
			break
		fi
	}

	#若前面添加了临时用户，则要删除
	if [ $FLAG_ADD_USER==1 ] ; then
	    restore_test_admin_user 17
	    echo " del the test user "
	fi

	if [ $i == $SCP_MAX_RETRY_TIME ] ;then
		echo "scp failed" | tee -a >>$SAVEFILE
		return 1
	else
		echo "scp successfully"
		return 0
	fi
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
    file_name="$1"
    #查询升级文件是否存在
	hpm_file_path=${G_WORK_DIR}/$file_name
	if [ ! -f "$hpm_file_path" ] ; then
	   echo_and_save_fail "hpm file is not present!"
	   error_quit
	fi
	
    #传输文件
	send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" "$file_name" /tmp/image.hpm
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

    echo "please wait for 120 second and reset bmc!"
    for((i=0;i<120;i++))
    do
	    echo -n -e "\b\b$i"
	    sleep 1
    done

    for((i=0;i<20;i++))
    {
        run_cmd "f 0 06 01" "0f 07 00 01 00"
        if [ $? -ne 0 ] ; then
            echo "please wait for 10 second!"
            sleep 10
        else 
            break
        fi
    }

    if [ $i -eq 20 ] ; then
        echo_fail "BMC reset test Fail"
        echo "BMC reset test Fail" >>$SAVEFILE
        return 1
    fi

    echo_success "BMC reset ok"
    echo "BMC reset ok" >>$SAVEFILE

    return 0
}

#使用bt通道升级bmc
hpm_upgrade_hpmfwupg()
{
    local i
    file_name="$1"

	for((i=0;i<$REPEAT;i++))
	{
		inputfile=in 
		rm -fr $inputfile
		mknod $inputfile p 

		exec 8<>$inputfile 

		echo "0" >$inputfile 
		echo "y" >$inputfile
		./hpmfwupg upgrade "$file_name" activate <&8
		if [ $? != 0 ] ; then
			sleep 30
			run_cmd "f 0 06 02" "0f 07 00 02 00"
			if [ $? != 0 ] ; then
				echo "hpm reset BMC fail $i" 
				echo "hpm reset BMC fail $i" >>$SAVEFILE
				error_quit
			fi
			sleep 60
		else
			echo "hpm load BMC ok $i"
			echo "hpm load BMC ok $i"  >>$SAVEFILE
			return 0
		fi
	}
	echo "hpm rest BMC fail $i" 
	echo "hpm rest BMC fail $i" >>$SAVEFILE
	error_quit
}

#hpm加载bmc软件
hpm_upgrade()
{
	file_name="$1"
	#安装驱动
	load_driver 

	for((k=0;k<${upgrade_repeat_times};k++))
	{
		start_upgrade "$file_name"
		if [ $? != 0 ]; then
			if [ ${upgrade_fail_reset_flag} == 1 ]; then
				reset_bmc
				if [ $? != 0 ] ; then
					return 1
				fi
			fi
		else
            # 如果需要重启传入参数2为1（即升级后需要重启BMC），则进行重启操作
            if [ $bmc_reset_flag == 1 ]; then
                echo_success "after upgrade this file,need to reset bmc"
                reset_bmc
                if [ $? != 0 ]; then
                    echo_fail "reset bmc fail"
                    echo "reset bmc fail" >$SAVEFILE
                    error_quit
                fi
            else
                echo_success "the upgrade file need't reset BMC"
            fi         
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

upgrade_file=""
bmc_reset_flag=0
upgrade_fail_reset_flag=1
upgrade_repeat_times=6

if [ $1 == "" ]; then
    echo_fail "there is no upgrade file name,please input upgrade file name"
    error_quit
else
    upgrade_file="$1"
fi

if [ $2 == "" ]; then
    echo_fail "there is no reset bmc flag, please add para 2 to add bmc reset flag"
    error_quit
elif [ $2 == 0 ] || [ $2 == 1 ]; then
    bmc_reset_flag=$2
else
    echo_fail "the input para 2 is wrong"
    error_quit
fi    

if [[ $3 == 0 ]]; then
    upgrade_fail_reset_flag=0
else
    upgrade_fail_reset_flag=1
fi

if [[ $4 == 2 ]]; then
    upgrade_repeat_times=2
else
    upgrade_repeat_times=6
fi

#获取版本信息
get_product_version_num
#开启dft验证
DFT_enable
sleep 1
chmod 777 hpmfwupg 
source ./add_temp_user.sh

hpm_upgrade "$upgrade_file"
if [ $? != 0 ]; then
    if [ ${upgrade_fail_reset_flag} == 1 ]; then
        hpm_upgrade_hpmfwupg "$upgrade_file"
    else
        echo "upgrade $upgrade_file fail"
        error_quit
    fi
else
    echo_success "upgrade hpm file $upgrade_file success"   
fi  
    
ok_quit

