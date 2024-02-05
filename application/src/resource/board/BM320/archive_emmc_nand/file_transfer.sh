chmod +x *
############################################
# bash script: 传输文件到BMC
############################################
#把项目编码赋值
project_code=$1
#命令失败时的重试次数 和间隔 
REPEAT=6
REPEAT_TIME=1 
PRODUCT_VERSION_NUM=03
FLAG_ADD_USER=0
SAVEFILE=$project_code"_save.txt"
CONFIG_FILE=$project_code"_product_strategy_config.ini"
G_WORK_DIR=`pwd`

error_quit()
{
    exit 1
}

ok_quit()
{
    exit 0
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
        echo_fail "BMC reset fail"
        echo "BMC reset fail" >>$SAVEFILE
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
        echo_fail "BMC reset fail"
        echo "BMC reset fail" >>$SAVEFILE
        return 1
    fi

    echo_success "BMC reset sucesss"
    echo "BMC reset ok" >>$SAVEFILE

    return 0
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
		status=`cat temp.txt |awk -F " " '{print $10}' `
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

	echo "load driver sucesss!"
	ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
	return 0
}

#传输文件
send_file()
{
	SRC_FILE=$1

	BMC_IP="\["`cat $CONFIG_FILE | grep ^BMCIP | awk -F '=' '{print $2}' | cut -f1 -d";"`"\]"
	USER=`cat $CONFIG_FILE | grep ^User | awk -F '=' '{print $2}' | cut -f1 -d";"`
	PWD=`cat $CONFIG_FILE | grep ^Password | awk -F '=' '{print $2}' | cut -f1 -d";"`

	if [ "$BMC_IP" == "\[\]" ];then
		BMC_IP="\[fe80::9e7d:a3ff:fe28:6ffa%veth\]"
	fi
	
	if [ "$USER" == "" ];then
		USER="Administrator"
	fi
	
	if [ "$PWD" == "" ];then
		PWD="Admin@9000"
	fi
	
	#先校验默认用户名和密码是否可用，若不可用，添加临时用户名和密码
	check_user2_default_name_pwd
	if [ $? != 0 ] ; then
	    echo "need to add a test user"
	    add_test_admin_user
	    USER="DFTUpgradeTest"
	    PWD="Admin@9000"
	    FLAG_ADD_USER=1
	fi
	sleep 10
	expect << EOF
	set timeout 300
	spawn scp -6r -l 4096 ${SRC_FILE} ${USER}@${BMC_IP}:/tmp
	expect {
	"*yes/no" { send "yes\r"; exp_continue }
	"*password:" { send "$PWD\r" }
	}

	expect eof
EOF

	#若前面添加了临时用户，则要删除
	if [ $FLAG_ADD_USER -eq 1 ] ; then
	    restore_test_admin_user 17
	    echo " del the test user "
	fi
	
	echo "/tmp" > $2
	return 0
}



############################################################################
#########################程序开始运行#####################################
############################################################################

if [ $# != 3 ]; then
	echo "$0 <project_code> <file> <destination_path_saved>"
	error_quit
fi

if [ ! -f $2 ];then
	echo "$2 is not exist"
	echo "$2 is not exist" >>$SAVEFILE
	error_quit
fi

if [ ! -f $CONFIG_FILE ];then
	echo "$CONFIG_FILE is not exist"
fi


dos2unix *.ini

#开启dft验证
DFT_enable
sleep 1

source ./add_temp_user.sh

load_driver

for((k=0;k<$REPEAT;k++))
{
	send_file $2 $3
	if [ $? != 0 ] ; then
		reset_bmc
		if [ $? != 0 ] ; then
			echo "reset BMC fail" >>$SAVEFILE
			error_quit
		fi
	else
		echo "transfer file $2 to BMC success"
		break
	fi
}

ok_quit

