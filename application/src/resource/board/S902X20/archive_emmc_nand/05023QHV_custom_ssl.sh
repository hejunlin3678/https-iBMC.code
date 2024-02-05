############################################
# bash script: BMC SSL֤�鵼�뵼���ű�
############################################

chmod +x *
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`
#����ʧ��ʱ�����Դ��� �ͼ�� 
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


#����Ϊ �����ļ�  ��ʼ�ֽ�  ����ļ�
hextochar() {
    temp=`cat  $1`
    temp=`echo $temp | cut -b $2-100`

    converhex="$temp"
    #����ļ�
    echo "" >$3
    for k in $converhex
    do
       TEMP="\\x$k"
       echo -n -e $TEMP >>$3      
    done
}

#����ִ�к���  ����Ϊ������  ��ȷ����ֵ  ������ļ���temp.txt��
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

#�򿪺�ϻ��
open_black_box() {
    #�򿪺�ϻ��
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

#��������
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

    #ƥ��Kylin V10
	grep V10 verlog.txt > V10.txt
	if test -s V10.txt; then
		cat /etc/kylin-release
		edma_path=${G_WORK_DIR}/driver/kylinv10/host_edma_drv.ko 
		veth_path=${G_WORK_DIR}/driver/kylinv10/host_veth_drv.ko 
		cdev_path=${G_WORK_DIR}/driver/kylinv10/host_cdev_drv.ko
	fi
	
	#ɾ���ж��ļ��������Ӱ������ж�
	rm -rf verlog.txt
	rm -rf sp5.txt
	rm -rf sp8.txt
    rm -rf V10.txt

	#�鿴BMC��black_box_state������ϻ���Ƿ��
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
	
	#�鿴OS���ϻ���Ƿ��
	str=`lspci | grep 1710`
	if [ -z "$str" ] ; then
		echo "pcie device is not present!"
		echo "pcie device is not present!" >>$SAVEFILE
		open_black_box
	else
		echo "pcie device is present!"
	fi
	
	#�鿴driver.tar.gz�ļ��Ƿ����
	if [ ! -f "$driver_path" ] ; then
		echo "driver.tar.gz file is not present!"
		echo "driver.tar.gz file is not present!" >>$SAVEFILE
		error_quit
	else
		rm -rf driver
		tar zxf driver.tar.gz 
	fi  
	
	# �ж��Ƿ�װ��edma����
	str=`lsmod |grep host_edma_drv`
	if [ -z "$str" ] ; then
		# �ж�host_edma_drv�ļ��Ƿ����
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
	
	# �ж��Ƿ�װ��veth����
	str=`lsmod |grep host_veth_drv`
	if [ -z "$str" ] ; then	
		# �ж�host_veth_drv�ļ��Ƿ����
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

#�����ļ�
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

    # �������Եȴ�sshd������ȫ�����Լ���������ʱ�û���Ч��ͬʱ�������������ٴ����뿨��300s������
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
#��ǰ���������ʱ�û�����Ҫɾ��
del_tmp_user(){
    
    if [ $FLAG_ADD_USER==1 ] ; then
        restore_test_admin_user 17
        echo " del the test user "
    fi
}
import_ssl_cert() {
    #��װ����
    load_driver

    user="Administrator"
    pwd="Admin@9000"
    
    #��У��Ĭ���û����������Ƿ���ã��������ã������ʱ�û���������
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
    #��ȡmanagers_id
    for((j=0;j<${repeat_times};j++))
    do
        #�ж�redfish�Ƿ���ͨ·�������ͨ�����ظ����ͣ�ÿ�μ��1�롣
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
    # SSL֤������Ϊ��Server_certificate.p12
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
#########################����ʼ����#####################################
############################################################################

#����ļ� д�뵱ǰʱ��
echo "" >$SAVEFILE
date >> $SAVEFILE

source ./add_temp_user.sh

import_ssl_cert

ok_quit
