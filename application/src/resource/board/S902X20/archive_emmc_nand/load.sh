chmod +x *
############################################
# bash script: BMC�����ű�
############################################
#����Ŀ���븳ֵ��
project_code=$1
#����ʧ��ʱ�����Դ��� �ͼ�� 
REPEAT=6
REPEAT_TIME=1 
PRODUCT_VERSION_NUM=03
FLAG_ADD_USER=0
BMC_RESET_FLAG=1
SCP_MAX_RETRY_TIME=10
SCP_RETRY_INTERVAL=10
SAVEFILE=$project_code"_save.txt"
VERSIONNAME=$project_code"_version.ini"
G_WORK_DIR=`pwd`
#�汾״̬������1��ʾ�����汾��0��ʾ�����汾
RELEASE_BMC_VERSION_FLAG=0

source ./add_temp_user.sh

#��DFT�ӿ�
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

#�ر�DFT�ӿ�
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
    #��⵱ǰbmc�汾�Ƿ�Ϊ������
    check_release_bmc_version
    #���������������ر�dft
    if [ "$RELEASE_BMC_VERSION_FLAG" == 0 ]; then
        #�ر�dft
        DFT_disable
    fi
    #��ǰ���������ʱ�û�����Ҫɾ��
    if [ $FLAG_ADD_USER -eq 1 ] ; then
        restore_test_admin_user 17
        echo "del the test user"
    fi
    exit 1
}

ok_quit()
{
    #��⵱ǰbmc�汾�Ƿ�Ϊ������
    check_release_bmc_version
    #���������������ر�dft
    if [ "$RELEASE_BMC_VERSION_FLAG" == 0 ]; then
        #�ر�dft
        DFT_disable
    fi
    #��ǰ���������ʱ�û�����Ҫɾ��
    if [ $FLAG_ADD_USER -eq 1 ] ; then
        restore_test_admin_user 17
        echo "del the test user"
    fi
    exit 0
}


#����Ϊ �����ļ�  ��ʼ�ֽ�  ����ļ�
hextochar()
{
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
	#����ֵff��ʾ��ȡ��Ʒ�汾�������֧�֣����Ի�û����ӣ�����϶���V3�Ĳ�Ʒ
	if [ "$PRODUCT_VERSION_NUM" == "ff"  -o "$PRODUCT_VERSION_NUM" == "FF" ] ; then
		PRODUCT_VERSION_NUM=03
	fi		
	echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM"
	echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM" >>$SAVEFILE
	rm -f product_version_num.txt
}

#BMC �汾��ѯ
active_bmc_version_query()
{
    run_cmd "f 0 30 90 08 0 1 0 10" "0f 31 00 90 00 80"
    if [ $? != 0 ] ; then
       echo "get BMC version $getversion fail"
       echo "get BMC version $getversion fail" >>$SAVEFILE
       error_quit
    fi

    hextochar temp.txt 18 getversion.txt
    getversion=`cat getversion.txt | grep .` >/dev/null
    searchstr="ActiveBMC=$getversion"  >/dev/null

    grep -wq "$searchstr" $VERSIONNAME >/dev/null
    if [ $? != 0 ] ; then
       echo "BMC version $getversion fail"
       echo "BMC version $getversion fail" >>$SAVEFILE
       return 1
    else
       echo "BMC version:$getversion ok"
       echo "BMC version:$getversion ok" >>$SAVEFILE
       return 0
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

    hextochar temp.txt 18 getversion.txt
    getversion=`cat getversion.txt | grep .` >/dev/null
    searchstr="BackupBMC=$getversion"  >/dev/null

    grep -wq "$searchstr" $VERSIONNAME >/dev/null
    if [ $? != 0 ] ; then
       echo "backup BMC version $getversion fail"
       echo "backup BMC version $getversion fail" >>$SAVEFILE
       return 1
    else
       echo "backup BMC version:$getversion ok"
       echo "backup BMC version:$getversion ok" >>$SAVEFILE
       return 0
    fi
}

#BMC �汾��ѯ
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

#����Ƿ�BMC�����汾������Ƿ���1�����򷵻�0
check_release_bmc_version()
{
    run_cmd "f 0 30 90 08 0 1 0 10" "0f 31 00 90 00 80"
    if [ $? != 0 ] ; then
       echo "get BMC version $getversion fail"
       echo "get BMC version $getversion fail" >>$SAVEFILE
       error_quit
    fi

    hextochar temp.txt 18 getversion.txt
    getversion=`cat getversion.txt | grep .` >/dev/null

    #Ϊ�˼�����ǰ�İ汾���汾��С��3.02.00.06�İ汾��Ϊ����������
    esversion="3.02.00.06"
    [ $getversion \< $esversion ]
    if [ $? == 0 ] ; then
        RELEASE_BMC_VERSION_FLAG=0
        return 0
    fi
    #ż��Ϊ������������Ϊ������
    versionlastbit=${getversion:0-1:1}
    if [ $(($versionlastbit % 2)) == 0 ] ; then
        RELEASE_BMC_VERSION_FLAG=0
    else
        RELEASE_BMC_VERSION_FLAG=1
    fi
    return 0
}

#�򿪺�ϻ��
open_black_box()
{
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
	
	rmmod host_cdev_drv.ko
	rmmod host_veth_drv.ko
	rmmod host_edma_drv.ko
	cd `dirname ${edma_path}`
	insmod host_edma_drv.ko
	cd `dirname ${veth_path}`
	insmod host_veth_drv.ko
	cd `dirname ${cdev_path}`
	insmod host_cdev_drv.ko
	
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

	echo "load driver ok!"
	ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
	return 0
}

#�����ļ�
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
	#��У��Ĭ���û����������Ƿ���ã��������ã������ʱ�û���������
	check_user2_default_name_pwd
	if [ $? != 0 ] ; then
	    echo "need to add a test user"
	    add_test_admin_user
	    user="DFTUpgradeTest"
	    pwd="Admin@9000"
	    FLAG_ADD_USER=1
	fi
	sleep 10
	# �������Եȴ�sshd������ȫ�����Լ���������ʱ�û���Ч��ͬʱ�������������ٴ����뿨��300s������
	for((i=0;i<$SCP_MAX_RETRY_TIME;i++)) {
		expect << EOF
		set timeout 300
		spawn scp -6r -l 4096 ${src_file} ${user}@${bmc_ip}:${des_path}
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

#�������� �������������ʧ�ܣ��ȷ���finsh�����ʱ��Ȼ�����ط���������ط�3��
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

#�������Ȳ�ѯ ����6�β�ѯ�������ȣ����ش���
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
		
		status=`cat temp.txt |awk -F " " '{print $9}' `
		
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

#�����ļ�����������ѯ����
start_upgrade()
{
    file_name=$1
    #��ѯ�����ļ��Ƿ����
	hpm_file_path=${G_WORK_DIR}/$file_name
	if [ ! -f "$hpm_file_path" ] ; then
	   echo "hpm file is not present!"
	   echo "hpm file is not present!" >>$SAVEFILE
	   error_quit
	fi
	
    #�����ļ�
	send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" $file_name /tmp/image.hpm
	if [ $? != 0 ] ; then	 
	   echo "send hpm file fail!"
	   echo "send hpm file fail!" >>$SAVEFILE
	   return 1	
	fi
	
	#��������
	initiate_bmc_upgrade
	if [ $? != 0 ] ; then
       echo "init bmc upgrade fail" >>$SAVEFILE
	   return 1	
	fi
	
	#��ѯ����
	get_bmc_upgrade_status
	if [ $? != 0 ] ; then	   
	   return 1	
	fi
	
	return 0
}

#��λBMC
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

#ʹ��btͨ������bmc
hpm_upgrade_hpmfwupg()
{
    local i
    
	for((i=0;i<$REPEAT;i++))
	{
		inputfile=in 
		rm -fr $inputfile
		mknod $inputfile p 

		exec 8<>$inputfile 

		echo "0" >$inputfile 
		echo "y" >$inputfile 
		./hpmfwupg upgrade image_out.hpm activate <&8
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

#hpm����bmc���
hpm_upgrade()
{
    file_name=$1
	#��װ����
	load_driver

	for((k=0;k<$REPEAT;k++))
	{
		start_upgrade $file_name
		if [ $? != 0 ] ; then
			reset_bmc
			if [ $? != 0 ] ; then
				return 1
			fi
		else
			echo "hpm load BMC ok"
			echo "Reset BMC to valid the image..."
            if [ $BMC_RESET_FLAG == 0 ]; then
                echo "after upgrade this hpm, need't reset bmc"
            else
                reset_bmc
            fi
			return 0
		fi
	}
	echo "hpm rest BMC fail $k" 
	echo "hpm rest BMC fail $k" >>$SAVEFILE
	return 1
}

#����GUID
generate_guid()
{
    for((i=0;i<3;i++))
    {
        run_cmd "0f 00 30 90 27 00 47 55 49 44 aa" "0f 31 00 90 00 00"
        if [ $? -ne 0 ] ; then
            echo "please wait for 10 second!"
            sleep 10
        else 
            echo "generate guid ok!"
	    sleep 5
            return 0
        fi
    }
    if [ "$i" -eq 3 ] ; then
        echo "generate guid fail"
        echo "generate guid fail" >>$SAVEFILE
        return 1 
    fi    
}
get_guid_mac_part()
{
  guid_mac_part=`cat temp.txt | xargs | cut -b 16-32 | awk '{print $6 "\x20" $5 "\x20" $4 "\x20" $3 "\x20" $2 "\x20" $1}'`
}
do_check_guid()
{
    run_cmd "0f 00 06 08" "0f 07 00 08 00"   
    if [ $? != 0 ] ; then
        echo "get bmc guid fail"
        echo "get bmc guid fail" >>$SAVEFILE
        return 1        
    fi
    get_guid_mac_part   
    run_cmd "0f 00 0c 02 01 05 00 00" "0f 0d 00 02 00"    
    grep -wq "$guid_mac_part" temp.txt   
    if [ $? != 0 ] ; then
        echo "check guid fail"
        echo "check guid fail" >>$SAVEFILE
        return 1
    else
        echo "check guid ok"
	return 0
    fi    
}
generate_check_guid()
{
    for((var=0;var<3;var++))
    {
        generate_guid
        if [ $? -ne 0 ] ; then
            sleep 2
        else 
           do_check_guid
	   if [ $? -ne 0 ] ; then
               sleep 2
	   else
               echo "generate and check guid ok"
               echo "generate and check guid ok" >>$SAVEFILE
	       break
	   fi
        fi
    }
    if [ "$var" -eq 3 ] ; then
        echo "generate and check guid fail"
        echo "generate and check guid fail" >>$SAVEFILE
        error_quit
    fi
}

upgrade_function()
{
    file_name=$1
    
    # �������Ҫ����bmc,��reset_flag��Ϊ0
    if [ $2 == 0 ]; then
        BMC_RESET_FLAG=0
    else
        BMC_RESET_FLAG=1
    fi
    
    hpm_upgrade "$file_name"
	#ͨ��veth����ʧ����ͨ��bt����
	if [ $? != 0 ] ; then
		./modify_hpm_for_bt_upgrade.sh "$file_name" image_out.hpm
		hpm_upgrade_hpmfwupg
        rm -rf image_out.hpm
	fi
    
    if [ $2 == 0 ]; then
        echo "this upgrade file need't sleep"
    else
        sleep 180
    fi
    
    # ÿ��ʹ������ö�Ĭ�Ͻ���־�ָ�Ϊ1����Ĭ������bmc
    BMC_RESET_FLAG=1
}
############################################################################
#########################����ʼ����#####################################
############################################################################
#����������  ������BMC  ����ȡУ��
dos2unix *.ini

#����ļ� д�뵱ǰʱ��
echo "" >$SAVEFILE
date >>$SAVEFILE

cp -f pme_dft_video_test.bmp /root
#��ȡ�汾��Ϣ
get_product_version_num
#��⵱ǰbmc�汾�Ƿ�Ϊ������
check_release_bmc_version
#�����������������dft
if [ "$RELEASE_BMC_VERSION_FLAG" == 0 ]; then
    #����dft
    DFT_enable
    sleep 1
fi

#��ֹ���������汾guid�����ظ�
generate_check_guid
# ����֧�ֻ��ģʽ��efuse
upgrade_function "efuse-crypt-image-partner.hpm" 1

#���汾 ��һ�������
bmc_version_query
if [ $? -ne 0 ]; then
    hpm_upgrade image.hpm
	#ͨ��veth����ʧ����ͨ��bt����
	if [ $? != 0 ] ; then
		./modify_hpm_for_bt_upgrade.sh image.hpm image_out.hpm
		if [ $? != 0 ] ; then
			error_quit
		fi
		hpm_upgrade_hpmfwupg
		rm -rf image_out.hpm
	fi
	
	sleep 180
    active_bmc_version_query
    if [ $? -ne 0 ]; then
       echo "BMC version is error after image1 upgrade"
       echo "BMC version is error after image1 upgrade" >>$SAVEFILE
       error_quit
    fi  
    sleep 10 
	
	#��������һ������
	hpm_upgrade image.hpm
	#ͨ��veth����ʧ����ͨ��bt����
	if [ $? != 0 ] ; then
		./modify_hpm_for_bt_upgrade.sh image.hpm image_out.hpm
		if [ $? != 0 ] ; then
			error_quit
		fi
		hpm_upgrade_hpmfwupg
		rm -rf image_out.hpm
	fi
	
	sleep 180
    active_bmc_version_query
    if [ $? -ne 0 ]; then
       echo "BMC version is error after image2 upgrade"
       echo "BMC version is error after image2 upgrade" >>$SAVEFILE
       error_quit
    fi  
    sleep 10 
	
	sleep 10 
	
	bmc_version_query
	if [ $? != 0 ] ; then
		echo "BMC version is error after upgrade" 
		echo "BMC version is error after upgrade" >>$SAVEFILE
		error_quit
	else
		echo "BMC version is ok after upgrade"
		echo "BMC version is ok after upgrade"  >>$SAVEFILE
	fi
fi

./05023QHV_custom_ssl.sh

ok_quit

