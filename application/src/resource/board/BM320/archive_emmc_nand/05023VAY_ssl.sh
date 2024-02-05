############################################
# bash script: BMC SSL证书导入导出脚本
############################################

chmod +x *
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`
#命令失败时的重试次数 和间隔 
REPEAT=6
REPEAT_TIME=1 
PRODUCT_VERSION_NUM=03
FLAG_ADD_USER=0
VRD_VERSION_FLAG=0
SCP_MAX_RETRY_TIME=10
SCP_RETRY_INTERVAL=10

SAVEFILE=$project_code"_save.txt"
VERSIONNAME=$project_code"_version.ini"
G_WORK_DIR=`pwd`

error_quit() {
    exit 1
}

ok_quit() {
    exit 0
}


#参数为 输入文件  开始字节  输出文件
hextochar() {
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
run_cmd() {
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
open_black_box() {
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
	ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
	echo "load driver ok!"
	return 0
}

# load_driver() {
    # # suse 15  版本信息配置到了 /etc/os-release
    # if [ ! -f "/etc/SuSE-release" ] ; then
        # driver_version=`cat /etc/os-release |grep VERSION_ID|awk -F "=" '{print $2}' | sed -e 's/"//g' | sed -e 's/\./sp/'`
        # echo "SUSE Linux Version : ${driver_version}"
        # driver_path=${G_WORK_DIR}/driver.tar.gz
        # edma_path=${G_WORK_DIR}/driver/suse${driver_version}/host_edma_drv.ko 
        # veth_path=${G_WORK_DIR}/driver/suse${driver_version}/host_veth_drv.ko 
    # else
        # suse_version=`cat /etc/SuSE-release |grep VERSION|awk -F " " '{print $3}' `
        # suse_patch=`cat /etc/SuSE-release |grep PATCHLEVEL|awk -F " " '{print $3}'`
        # driver_version=suse${suse_version}sp${suse_patch}
        # echo "SUSE Linux Version : ${driver_version}"
        
        # driver_path=${G_WORK_DIR}/driver.tar.gz
        # edma_path=${G_WORK_DIR}/driver/suse${suse_version}sp${suse_patch}/host_edma_drv.ko 
        # veth_path=${G_WORK_DIR}/driver/suse${suse_version}sp${suse_patch}/host_veth_drv.ko 
    # fi

    # #查看BMC的black_box_state，看黑匣子是否打开
    # run_cmd "f 0 30 93 db 07 00 0x10 0x0c" "0f 31 00 93 00 db 07 00"
    # if [ $? != 0 ] ; then
        # echo "get black box state fail"
        # echo "get black box state fail" >>$SAVEFILE
        # error_quit
    # else
        # status=`cat temp.txt |awk -F " " '{print $10}' `
        # if [ $((16#$status)) -eq 1 ] ; then
            # echo "The black box state is open"
        # else
            # open_black_box
        # fi
    # fi
    
    # #查看OS测黑匣子是否打开
    # str=`lspci | grep 1710`
    # if [ -z "$str" ] ; then
        # echo "pcie device is not present!"
        # echo "pcie device is not present!" >>$SAVEFILE
        # open_black_box
    # else
        # echo "pcie device is present!"
    # fi
    
    # #查看driver.tar.gz文件是否存在
    # if [ ! -f "$driver_path" ] ; then
        # echo "driver.tar.gz file is not present!"
        # echo "driver.tar.gz file is not present!" >>$SAVEFILE
        # error_quit
    # else
        # rm -rf driver
        # tar zxf driver.tar.gz 
    # fi  
    
    # # 判断是否安装了edma驱动
    # str=`lsmod |grep host_edma_drv`
    # if [ -z "$str" ] ; then
        # # 判断host_edma_drv文件是否存在
        # if [ ! -f "$edma_path" ] ; then
            # echo "host_edma_drv file is not present!"
            # echo "host_edma_drv file is not present!" >>$SAVEFILE
            # error_quit
        # fi  
        
        # cd `dirname ${edma_path}`
        # insmod host_edma_drv.ko
        # if [ $? != 0 ] ; then
            # echo "insmod edma driver failed!"
            # echo "insmod edma driver failed!" >>$SAVEFILE
            # error_quit
        # fi
    # fi
    
    # # 判断是否安装了veth驱动
    # str=`lsmod |grep host_veth_drv`
    # if [ -z "$str" ] ; then    
        # # 判断host_veth_drv文件是否存在
        # if [ ! -f "$veth_path" ] ; then
           # echo "host_veth_drv file is not present!"
           # echo "host_veth_drv file is not present!" >>$SAVEFILE
           # error_quit
        # fi 
        
        # cd `dirname ${veth_path}`
        # insmod host_veth_drv.ko
        # if [ $? != 0 ] ; then
            # echo "insmod veth driver failed!"
            # echo "insmod veth driver failed!" >>$SAVEFILE
            # error_quit
        # fi
    # fi
    
    # cd ${G_WORK_DIR}
    # ifconfig veth up

    # echo "load driver ok!"
    # ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
    # return 0
# }

#传输文件
file_trans() {
    bmc_ip=$1
    src_file=$2
    des_path=$3
    operate=$4

    user="Administrator"
    pwd="Admin@9000"

    if [ $# -lt 4 ] ; then
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
    if [ $operate == 'download' ] ; then
        para1=${user}@${bmc_ip}:${des_path}
        para2=${src_file}
    else
        para1=${src_file}
        para2=${user}@${bmc_ip}:${des_path}
    fi

    # 增加重试等待sshd服务完全起来以及新增的临时用户生效，同时解决密码错误导致再次输入卡死300s的问题
    for((i=0;i<$SCP_MAX_RETRY_TIME;i++)) {
        expect << EOF
        set timeout 300
        spawn scp -6r -l 4096 ${para1} ${para2}
        expect {
        "*yes/no" { send "yes\r"; exp_continue }
        "*password:" { send "$pwd\r" }
        }
    
        expect {
        "*password:" {exit 2}
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

export_ssl_csr() {
    #安装驱动
    load_driver

    for((i=0;i<3;i++))
    {
        # 导出SSL的CSR文件，文件名称：/tmp/server.csr
        run_cmd "f 0 30 93 db 07 00 57 00 01 06 00 00 0f 2f 74 6d 70 2f 73 65 72 76 65 72 2e 63 73 72" "0f 31 00 93 00 db 07 00"
        if [ $? != 0 ] ; then
           echo "The $i time export ssl server csr $1 failed"
        else
            file_trans "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" "$1" "/tmp/server.csr" download

            if [ $? != 0 ] ; then     
                echo "The $i time export ssl server csr $1 fail!"
                echo "The $i time export ssl server csr $1 fail!" >>$SAVEFILE
            else
                echo "export ssl server csr $1 successfully" 
                echo "export ssl server csr $1 successfully" >>$SAVEFILE
                return 0
            fi
        fi
        sleep 1
    }
    
    echo "export ssl server csr $1 fail" 
    echo "export ssl server csr $1 fail" >>$SAVEFILE
    return 1
}

import_ssl_cert() {
    #安装驱动
    load_driver
    
    # SSL证书命名为：server.cer
    for((i=0;i<3;i++))
    {
        file_trans "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" "$1" "/tmp/server.cer" upload
        if [ $? != 0 ] ; then
            echo "The $i time upload ssl server cert $1 failed"
        else
            run_cmd "f 0 30 93 db 07 00 57 00 02 07 00 00 0f 2f 74 6d 70 2f 73 65 72 76 65 72 2e 63 65 72" "0f 31 00 93 00 db 07 00"
            if [ $? != 0 ] ; then     
                echo "The $i time import ssl server cert $1 fail!"
                echo "The $i time import ssl server cert $1 fail!" >>$SAVEFILE
            else
                echo "import ssl server cert $1 successfully" 
                echo "import ssl server cert $1 successfully" >>$SAVEFILE
                return 0
            fi
        fi
        sleep 1
    }

    echo "import ssl server cert $1 fail" 
    echo "import ssl server cert $1 fail" >>$SAVEFILE
    return 1
}

############################################################################
#########################程序开始运行#####################################
############################################################################

#清空文件 写入当前时间
echo "" >$SAVEFILE
date >> $SAVEFILE

# 更新证书有效期时间, 2041-01-01 00:00:00 为证书有效期截止时间
# 计算当前时间到证书有效期截止时间的天数
expiryDays=$(( ($(date -d "2041-04-01 00:00:00" '+%s') - $(date '+%s')) / 86400 ))
# 最大天数不能超过6519天
if [ $expiryDays -ge "6519" ]; then
	echo "invalid date"
	exit
fi
# 将天数更新到配置文件中
sed -i "/CertValidity/c\CertValidity=${expiryDays}" ssl_parameter.ini

source ./add_temp_user.sh

if [ $# -ne 2 ]; then
    echo "$0 <operation> <file_name>"
    echo "  operation:"
    echo "    export: export the ssl certification csr file."
    echo "    import: import the ssl certification."
    error_quit
fi

if [ $1 == 'export' ]; then
    export_ssl_csr $2
elif [ $1 == 'import' ]; then
    import_ssl_cert $2
else
    echo "unsupport operation $1" 
    echo "unsupport operation $1"  >>$SAVEFILE
fi
 
ok_quit
