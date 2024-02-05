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

UserName="Administrator"
Password="Admin@9000"
IPV6_ADDR="fe80::9e7d:a3ff:fe28:6ffa%veth"
produce_name=""
machine_uid=""

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

#传输文件
file_trans() {
    bmc_ip=$1
    src_file=$2
    des_path=$3
    operate=$4

    if [ $# -lt 4 ] ; then
       echo "Parameter error!"
       echo "Parameter error!" >>$SAVEFILE
       return 1
    fi

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
    
    if [ $i == $SCP_MAX_RETRY_TIME ] ;then
        echo "scp failed" | tee -a >>$SAVEFILE
        return 1
    else
        echo "scp successfully"
        return 0
    fi
}
#若前面添加了临时用户，则要删除
del_tmp_user(){
    
    if [ $FLAG_ADD_USER==1 ] ; then
        restore_test_admin_user 17
        echo " del the test user "
    fi
}
import_ssl_cert() {
    #安装驱动
    load_driver

    user="Administrator"
    pwd="Admin@9000"
    
    #先校验默认用户名和密码是否可用，若不可用，添加临时用户名和密码
    check_user2_default_name_pwd
    if [ $? != 0 ] ; then
        echo "need to add a test user"
        add_test_admin_user
        user="DFTUpgradeTest"
        pwd="Admin@9000"
        UserName="DFTUpgradeTest"
        Password="Admin@9000"
        FLAG_ADD_USER=1
    fi
    sleep 10
    
    repeat_times=20
    repeat_error_times=0
    #获取managers_id
    for((j=0;j<${repeat_times};j++))
    do
        #判断redfish是否是通路，如果不通，则重复发送，每次间隔1秒。
        slot_code=$(curl -k -s -u ${UserName}:${Password} "https://[${IPV6_ADDR}]/redfish/v1/Managers" -o /dev/null -s -w %{http_code})
        if [ $? -ne 0 -o ${slot_code} -ne 200 ];
        then
            echo "slot_code=${slot_code}"
            repeat_error_times=$((repeat_error_times+=1))
            echo "redfish link is break, then try again,the try times is ${repeat_error_times}"
            if [ ${repeat_error_times} -gt 19 ]; then
                echo "The link is blocked "
                del_tmp_user
                return 1
            fi

            sleep 1
            continue
        else
            echo "slot_code=${slot_code}"
            managers_id=$(curl -k -s -u ${UserName}:${Password} "https://[${IPV6_ADDR}]/redfish/v1/Managers" | awk -F "\"Members\":" '{print $2}'| awk -F "\"" '{print $4}'| awk -F "/" '{print $NF}')
            if [ $? -ne 0 -o -z ${managers_id} ];
            then
                echo "managers_id=${managers_id}"
                echo "Failed to get managers id."
                echo "Failed to get managers id." >>$SAVEFILE
                del_tmp_user
                return 1
            else
                echo "managers_id=${managers_id}"
                echo "Get managers id successfully." >>$SAVEFILE
                break
            fi
        fi
    done
    # SSL证书命名为：Server_certificate.p12
    for((i=0;i<3;i++))
    {
        file_trans "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" "Server_certificate.p12" "/tmp/Server_certificate.p12" upload
        if [ $? != 0 ] ; then
            echo "The $i time upload ssl server cert $1 failed"
        else
            http_code=$(curl -k -s -u ${UserName}:${Password} -H "Content-Type:application/json" -X POST -d '{"Type":"URI","Password":"Admin@9000","Certificate":"/tmp/Server_certificate.p12"}' "https://[${IPV6_ADDR}]/redfish/v1/Managers/1/SecurityService/HttpsCert/Actions/HttpsCert.ImportCustomCertificate" -o /dev/null -s -w %{http_code})
            echo "http_code=${http_code}"
            if [ $? -ne 0 -o ${http_code} -ne 200 ]; 
            then
                echo "ImportCustomCertificate fail"
                echo "ImportCustomCertificate fail" >>$SAVEFILE
            else
                echo "ImportCustomCertificate success"
                echo "ImportCustomCertificate success" >>$SAVEFILE
                del_tmp_user
                return 0
            fi	            
        fi
        sleep 1
    }
    
    del_tmp_user

    echo "import ssl server cert fail" 
    echo "import ssl server cert fail" >>$SAVEFILE
    return 1
}

############################################################################
#########################程序开始运行#####################################
############################################################################

#清空文件 写入当前时间
echo "" >$SAVEFILE
date >> $SAVEFILE

source ./add_temp_user.sh

import_ssl_cert

ok_quit
