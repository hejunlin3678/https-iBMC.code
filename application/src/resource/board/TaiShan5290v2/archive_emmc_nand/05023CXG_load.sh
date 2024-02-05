#!/bin/sh

REPEAT=6
REPEAT_TIME=1 
MAX_STRING_NUM=24
IPMI_CMD_LOG_FILE=custom.log
IPMI_CMD_RETURN_FILE=ipmi_cmd_return_string.txt
SAVEFILE=05023CXG_save.txt
FLAG_ADD_USER=0
EQUIPMENT_SEPARATION_SUPPORT_STATE=0
G_WORK_DIR=`pwd`
PCERT_JUDGE_FLAG=1

source ./add_temp_user.sh

if [[ $1 != "" ]]; then
    if [ $1 == 0 ]; then
        PCERT_JUDGE_FLAG="$1"
    else
        PCERT_JUDGE_FLAG=1
    fi    
else
    PCERT_JUDGE_FLAG=1
fi

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

#去掉前后空格
remove_space()
{
	local covert_string="$1"
	covert_string=`echo "$covert_string" | sed 's/^[ ]*//g' | sed 's/[ ]*$//g'`
	echo "$covert_string"
}

#将字符串装换成十六进制
string2hex()
{
	STRING=$1
	echo $STRING > temp.txt
	#对于STRING的最大长度为255字节做判断
	if [ "${#STRING}" -gt 255 ] ; then
		echo_fail "Surpasses the greatest length limit 255!"
		echo "Surpasses the greatest length limit 255!" >> $SAVEFILE
		error_quit
	fi
	#转换字符串为16进制字符串
	./conver temp.txt 0 1 > temp_tmp.txt
	if [ $? != 0 ] ; then
		echo_fail  "conver $STRING Fail"
		echo  "conver $STRING Fail"  >> $SAVEFILE
		error_quit
	fi
	
	TMP_STRING=`cat temp_tmp.txt`
	TMP_STRING=$(remove_space "$TMP_STRING")
	
	echo "$TMP_STRING"
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

#判断并分割字符串
string2substring()
{
	STRING=$1
	local max_string_len=$2
	if [ "$max_string_len" = "" ] ; then
		max_string_len=48
	fi
	echo $STRING > temp.txt
	#对于STRING的最大长度为48位做判断
	if [ "${#STRING}" -gt $max_string_len ] ; then
		echo_fail "Surpasses the greatest length limit $max_string_len! write e_label $STRING failed!"
		echo "Surpasses the length limit!" >> $SAVEFILE
		error_quit
	fi
	#转换字符串为16进制字符串
	./conver temp.txt 0 1 > temp_tmp.txt
	if [ $? != 0 ] ; then
		echo_fail  "conver $STRING Fail"
		echo  "conver $STRING Fail"  >> $SAVEFILE
		error_quit
	fi
	
	TMP_STRING=`cat temp_tmp.txt`
	#整个字符串长度
	strlen=`echo ${#TMP_STRING}`
	#计算子字符长度
	len=`expr $MAX_STRING_NUM \* 3`
	#下个字符串开始位置
	offset=`expr $len + 1 `
	if [ "${#STRING}" -gt "$MAX_STRING_NUM" ] ; then
		TMP_STRING1=`echo $TMP_STRING | cut -b -$len`
		TMP_STRING2=`echo $TMP_STRING | cut -b $offset-$strlen`
	fi
}

#写电子标签
write_elabel()
{
	string2substring "$1"
	strlen=`echo ${#1}`
	#判断是否使用子字符串
	if [ "$strlen" -gt "$MAX_STRING_NUM" ] ; then
		first_len=`printf %2x $MAX_STRING_NUM`
		last_len=`expr $strlen - $MAX_STRING_NUM `
		last_len=`printf %2x $last_len`
		#写入第一个字符串
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 80 $first_len $TMP_STRING1" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Set e_label:$1 fail!"
			echo "Set e_label:$1 fail!" >>$SAVEFILE
			error_quit
		fi
		
		#写入剩余字符串
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER $first_len $last_len $TMP_STRING2" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Set e_label:$1 fail!"
			echo "Set e_label:$1 fail!" >>$SAVEFILE
			error_quit
		fi

		echo_success "Set e_label:$1 success!"
		echo "Set e_label:$1 success!" >>$SAVEFILE
	
	elif [ "$strlen" -gt "0" ] ; then
		#直接写入整串字符
		strlen=`printf %2x $strlen`
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 00 $strlen $TMP_STRING" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Set e_label:$1 fail!"
			echo "Set e_label:$1 fail!" >>$SAVEFILE
			error_quit
		else
			echo_success "Set e_label:$1 success!"
			echo "Set e_label:$1 success!" >>$SAVEFILE
		fi
	else
		#写电子标签时如果长度为0则数据data域不用填写
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 00 00" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Clear e_label fail!"
			echo "Clear e_label fail!" >>$SAVEFILE
			error_quit
		else
			echo_success "Clear e_label success!"
			echo "Clear e_label success!" >>$SAVEFILE
		fi
	fi
}

set_machine_name()
{
    #定制化标志位
    run_cmd "f 00 30 90 21 04 03" "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo_fail "Set support_customize_dft fail!"
        echo "Set support_customize_dft fail!" >>$SAVEFILE
        error_quit
    fi

    FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=1

	echo "set machine_name:$1" >>$SAVEFILE

	write_elabel "$1"

	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=1

	write_elabel "$1"

    #update elable
    run_cmd "f 0 30 90 06 00 aa" "0f 31 00 90 00 00"
    if [ $? -ne 0 ]; then
        echo "update fru produce_name fail"
        echo "update fru produce_name fail" >>$SAVEFILE
        error_quit
    fi
}

#参数为 输入文件  开始字节  输出文件
hextochar()
{
    temp=`cat  $1`
    temp=`echo $temp | cut -b $2- `

    converhex="$temp"

    #清空文件
    >$3

    for k in $converhex
    do
       TEMP="\\x$k"
       echo -n -e $TEMP >>$3      
    done
}

#读电子标签
read_elabel()
{
	max_str_len=`printf %2x $MAX_STRING_NUM`
	#读取22个字符
	run_cmd "f 0 30 90 05 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 00 $max_str_len" "0f 31 00 90 00"
	if [ $? != 0 ]; then
		echo_fail "read e_label fail!"
		echo "read e_label fail!" >> $SAVEFILE
		error_quit
	fi
	
	hextochar $IPMI_CMD_RETURN_FILE 19 temp_tmp.txt
	READ_ELABEL_STRING=`cat temp_tmp.txt`

	#根据返回值判断是否还有字符未读取完
	grep -wq "0f 31 00 90 00 80" $IPMI_CMD_RETURN_FILE
	if [ $? != 0 ]; then
		run_cmd "f 0 30 90 05 $FRU_ID $AREA_NUMBER $FIELD_NUMBER $max_str_len $max_str_len" "0f 31 00 90 00"
		if [ $? != 0 ]; then
			echo_success "read e_label:$READ_ELABEL_STRING"
			echo "read e_label:$READ_ELABEL_STRING" >> $SAVEFILE
		else
			hextochar $IPMI_CMD_RETURN_FILE 19 temp_tmp.txt
			READ_ELABEL_STRING=$READ_ELABEL_STRING`cat temp_tmp.txt`
			echo_success "read e_label:$READ_ELABEL_STRING"
			echo "read e_label:$READ_ELABEL_STRING" >> $SAVEFILE
		fi
	else
		echo_success "read e_label:$READ_ELABEL_STRING"
		echo "read e_label:$READ_ELABEL_STRING" >> $SAVEFILE
	fi
}

#校验机器名称是否写入
check_machine_name()
{    
	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=1
	READ_ELABEL_STRING=""

	read_elabel

	echo $READ_ELABEL_STRING > temp_tmp.txt

	if [ "$1" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "verify Machine name:$1 fail!"
		echo "verify Machine name:$1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "verify Machine name:$1 success"
		echo "verify Machine name:$1 success" >>$SAVEFILE
	fi

	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=1
	READ_ELABEL_STRING=""

	read_elabel

	echo $READ_ELABEL_STRING > temp_tmp.txt

	if [ "$1" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "verify product name:$1 fail!"
		echo "verify product name:$1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "verify product name:$1 success"
		echo "verify product name:$1 success" >>$SAVEFILE
		return 0
	fi
}

# 设置机器别名函数
set_machine_alias_function()
{
    MACHINEALIAS="$1"
    
    strlen=`echo ${#MACHINEALIAS}`
	info_val=$(string2hex "$MACHINEALIAS")
	info_len=`printf "%02x" $strlen`
    
    if [ ${strlen} -gt 16 ]; then
        echo_fail "the machine_alias is too long"
        echo "the machine_alias is too long" >>$SAVEFILE
        error_quit
    fi
    
    run_cmd "f 0 30 93 db 07 00 35 30 00 1 0 00 00 ff ff 00 01 00 08 00 73 ${info_len} ${info_val}" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ]; then
        echo_fail "set_machine_alias fail"
        echo "set_machine_alias fail" >>$SAVEFILE
        error_quit
    fi
    
    echo_success "set_machine_alias $MACHINEALIAS success"
}

# 设置机器别名
set_machine_alias()
{
    machine_alias=""
	
 	declare -A dir
	
    dir=([0x02070400]="4U")

	for key in $(echo ${!dir[*]})
	do
		if [ ${key} = ${ProductUniqueID} ];
		then
			machine_alias=${dir[${key}]}
			echo_success "get the machine_alias :${machine_alias}"
			echo "the uid ${ProductUniqueID} match machine_alias and alias is ${machine_alias}" >>$SAVEFILE
			break
		fi
	done

	if [ "${machine_alias}" = "" ];
	then
		echo "this machine needn't set machine_alias"
        return
	fi
	
    # 设置机器别名函数
    set_machine_alias_function "${machine_alias}"
}

check_userid_null()
{
	userid=`printf %2x $1`
	#判断该用户id是否已经创建了，如果已经创建了，就删除该用户
	run_cmd "f 00 06 46 $userid" "0f 07 00 46 00 00 00 00 00 00 00 00 00 00"
	if [ $? != 0 ] ; then
		 echo "user$1 is already have,delete user$1"
		 echo "user$1 is already have,delete user$1" >>$SAVEFILE
		 restore_test_admin_user $1
		echo "Delete the user$1 successfully."
		FLAG_ADD_USER=0
	else
		 return 0
	fi	
}

function check_default_name_pswd()
{
	#判断该用户id是否是默认的账号密码，如果不是默认的就添加临时账号和密码
	check_user2_default_name_pwd
	if [ $? -ne 0 ] ; then
		echo "Need to add a test user" >>$SAVEFILE
		check_userid_null 17
		add_test_admin_user
		UserName="DFTUpgradeTest"
		Password="Admin@9000"
		FLAG_ADD_USER=1
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
	ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
	echo "load driver ok!"
	return 0
}

function delete_bma_function()
{	
	#判断默认的用户名密码是否被修改，如果被修改就创建临时账号密码
	check_default_name_pswd

	#加载驱动
    load_driver

    sleep 6

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
                echo_fail "The link is blocked "
                echo "The link is blocked " >>$SAVEFILE
                error_quit
            fi

            sleep 1
            continue
        else
            echo "slot_code=${slot_code}"
            managers_id=$(curl -k -s -u ${UserName}:${Password} "https://[${IPV6_ADDR}]/redfish/v1/Managers" | awk -F "\"Members\":" '{print $2}'| awk -F "\"" '{print $4}'| awk -F "/" '{print $NF}')
            if [ $? -ne 0 -o -z ${managers_id} ];
            then
                echo "managers_id=${managers_id}"
                echo_fail "Failed to get managers id."
                echo "Failed to get managers id." >>$SAVEFILE
                error_quit
            else
                echo "managers_id=${managers_id}"
                echo "Get managers id successfully." >>$SAVEFILE
                break
            fi
        fi
    done

    http_code=$(curl -k -s -u ${UserName}:${Password} -H "Content-Type:application/json" -X POST -d '{"Mode":"Delete"}' "https://[${IPV6_ADDR}]/redfish/v1/Managers/${managers_id}/SmsService/Actions/SmsService.RefreshInstallableBma" -o /dev/null -s -w %{http_code})
    echo "http_code=${http_code}"
    if [ $? -ne 0 -o ${http_code} -ne 200 ]; 
    then
        echo_fail "Delete bma file fail"
        echo "Delete bma file fail" >>$SAVEFILE
        error_quit
    else
        echo_success "Delete bma file success"
        echo "Delete bma file success" >>$SAVEFILE
    fi

    #判断是否创建了user17，如果创建了就删除，防止影响其他功能
    check_userid_null 17
}

function query_produce_name()
{
	#判断默认的用户名密码是否被修改，如果被修改就创建临时账号密码
	check_default_name_pswd

	declare -A dir
	#创建一个字典进行uid和产品名称的对照
	dir=([0x02070500]="S920X03" [0x02070400]="S920X03")

	ProductUniqueID=$(curl -k -s -u ${UserName}:${Password} -g "https://[${IPV6_ADDR}]/redfish/v1/Managers/1" | awk -F "\"ProductUniqueID\":" '{print $2}' | awk -F "\"" '{print $2}')
	echo ${ProductUniqueID} >>$SAVEFILE
	if [ $? -ne 0 -o -z ${ProductUniqueID} ];
	then	
		echo_fail "fail to get ProductUniqueID"
		echo "fail to get ProductUniqueID" >>$SAVEFILE
		error_quit
	else	
		echo "get the ProductUniqueID success" >>$SAVEFILE
	fi

	for key in $(echo ${!dir[*]})
	do
		if [ ${key} = ${ProductUniqueID} ];
		then
			produce_name=${dir[${key}]}
			echo_success "get the produce_name :${produce_name}"
			echo "the uid ${ProductUniqueID} match name is ${produce_name}" >>$SAVEFILE
			break
		fi
	done

	if [ "${produce_name}" = "" ];
	then
		echo_fail "cant match produce_name"
		echo "cant match produce_name" >>$SAVEFILE
		error_quit
	fi
	
	#判断是否创建了user17，如果创建了就删除，防止影响其他功能
	check_userid_null 17
}

function upgrade_sp_function()
{
	filter_zip_file=""
	filter_file=""

	if [ -f "/etc/exports" ]; then
		rm -rf /etc/exports
		touch /etc/exports
	fi

	for file_name in `ls ${G_WORK_DIR}`
	do
		filter_zip_file=`echo ${file_name} | awk -F "-" '{print $2}'`
		if [ "${filter_zip_file}" == "Kunpeng.zip" ]; then
			filter_zip_file=${file_name}
			filter_file=`echo ${file_name} | awk -F ".zip" '{print $1}'`
			echo "${filter_file}"
			echo_success "find sp upgrade zip file: $filter_zip_file"
			break
		fi
	done
	if [ -d "${filter_file}" ]; then
		rm -rf "${filter_file}"
	fi	
	if [ "${filter_zip_file}" == "" ]; then
		echo_fail "no sp upgrade zip file, please make sure the zip file"
		echo "no sp upgrade zip file" >>$SAVEFILE
		error_quit
	else
		unzip ${filter_zip_file}
	fi
	
	if [ "${filter_file}" == "" ]; then
		echo "no sp upgrade file ,please make sure"
		echo "no sp upgrade file" >>$SAVEFILE
		error_quit
	else	
		if [ -d "${filter_file}" ]; then
			cd ${filter_file}
			chmod +x *
			if [ ! -f "driver.tar.gz" ]; then
				echo "cp driver.tar.gz into sp upgrade zip"
				if [ -f "../driver.tar.gz" ]; then
					cp ../driver.tar.gz ./
				else
					echo_fail "there is no driver.tar.gz in forward.sh"
					echo "there is no driver.tar.gz" >>$SAVEFILE
					error_quit
				fi
			fi
			./05022YQE_load.sh
			if [ $? -ne 0 ]; then
				echo_fail "sp upgrade fail"
				echo "sp upgrade fail" >>$SAVEFILE
				error_quit
			else
				echo_success "sp upgrade success"
				echo "sp upgrade success" >>$SAVEFILE
			fi
			
			cd ${G_WORK_DIR}
		else
			echo "no sp upgrade file ,please make sure"
			echo "there is no sp upgrade file" >>$SAVEFILE
			error_quit
		fi
	fi
	
	return 0
}	

wbd_config_function()
{
	#删除ssh已保存的公钥，防止由于公钥不匹配原因造成的scp报错
	if [ -f "/root/.ssh/known_hosts" ]; then
		rm -rf "/root/.ssh/known_hosts"
		touch "/root/.ssh/known_hosts"
	fi

	if [ -d "driver" ]; then
		echo "delete driver file"
		rm -rf "driver"
	fi
	
	./wbd_load.sh "wbd"
	if [ $? -ne 0 ]; then
		echo "wbd_fail"
		echo "wbd_fail" >>$SAVEFILE
		error_quit
	else
		echo "wbd success"
		echo "wbd success" >>$SAVEFILE
	fi	
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

# hpm 包升级
hpm_upgrade()
{
    file_name="$1"
    if [ ! -f ./${file_name} ]; then
        echo_fail "there is no ${file_name} file"
        echo "there is no ${file_name} file" >>$SAVEFILE
        error_quit
    fi

    ./upgrade_hpm.sh "${file_name}" 0
    if [ $? -ne 0 ]; then
        echo_fail "upgrade ${file_name} fail"
        echo "upgrade ${file_name} fail" >>$SAVEFILE
        error_quit
    else
        echo_success "upgrade ${file_name} success"
    fi
}

# 升级支持伙伴模式的efuse
upgrade_efuse_function()
{
    ./upgrade_hpm.sh "efuse-crypt-image-partner.hpm" 1
    if [ $? -ne 0 ]; then
        echo_fail "upgrade new efuse fail"
        echo "upgrade new efuse fail" >>$SAVEFILE
        error_quit
    else
        echo_success "upgrade new efuse success"
    fi

    sleep 180
}

# 判断是否有efuse，如果无efuse则进行升级
upgrade_efuse()
{
    echo "start get efuse state"
    # 如果返回值为00，则代表不含有efuse
    run_cmd "0f 00 30 93 db 07 00 5b 2c 00" "0f 31 00 93 00 db 07 00 00"
    if [ $? == 0 ]; then
        upgrade_efuse_function
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
    # 清除日志
    source ./file_restore.sh
    if [ $? -ne 0 ]; then
        echo_fail "clear log fail"
        echo "clear log fail" >>$SAVEFILE
        error_quit
    else
        echo_success "clear log success"
    fi
}

# 导入华为根
load_pcert()
{
    input_para="$1"
    echo_success "start load huawei pcert"
    if [ -f "./pcert_load.sh" ]; then
        ./pcert_load.sh "$input_para"
        return_value="$?"
        if [ ${return_value} == 1 ]; then
            echo_fail "load huawei pcert fail"
            echo "load huawei pcert fail" >>$SAVEFILE
            error_quit
        elif [ ${return_value} == 2 ]; then
            echo_success "this environment have been improt pcert already"
            return 1
        else
            echo_success "load huawei pcert success"
            return 0
        fi
    else
        echo_fail "there is no pcert_load.sh"
        echo "there is no pcert_load.sh" >>$SAVEFILE
        error_quit
    fi    
}

chmod +x conver
chmod +x *

#这里添加一个a板的判断，如果是a板，就直接退出这个脚本执行。
run_cmd  "f 0 30 90 29 00 01"  "0f 31 00 90 00"
grep -wq "0f 31 00 90 00 0a" $IPMI_CMD_RETURN_FILE
if [ $? = 0 ]; then
    echo "PRODUCT_TYPE: Kunpeng_Board-Serials get_product_id: a" >>$SAVEFILE
    ok_quit
fi

UserName="Administrator"
Password="Admin@9000"
IPV6_ADDR="fe80::9e7d:a3ff:fe28:6ffa%veth"
produce_name=""
ProductUniqueID=""

load_driver
sleep 10
# 判断是否有efuse，如果无efuse则进行升级
upgrade_efuse
# redfish 接口查询 UID ，根据UID转换产品名称$1
query_produce_name
# 5290机型不需要进行导根操作
if [[ ${ProductUniqueID} == 0x02070400 ]]; then
    # 升级校验版本，确定是新的efuse且不带有根 参数1：需要升级的文件名  参数2：是否需要重启BMC操作，1：重启  0：不需要重启  (如果需要重启，会有2分钟的复位等待时间+3分钟的BMC稳定时间，共需5分钟的升级时间)
    ./upgrade_hpm.sh "wbd_verify.hpm" 0 1
    if [ $? -ne 0 ]; then
        echo_fail "upgrade wbd_verify hpm fail"
        echo "upgrade wbd_verify hpm fail" >>$SAVEFILE
        error_quit
    else
        echo_success "upgrade wbd_verify hpm success"
    fi
else
    # 判断是否导入过根，如果导入过s1和s2，则返回值为1,则不进行导根。如果为0，则进行导根操作
    load_pcert 1
    if [ $? == 1 ]; then
        # 校验根是真的正常导入，且是华为根
        echo "verify the pcert is load success"
        ./upgrade_hpm.sh "wbd_verify.hpm" 0 0 2
        if [ $? -ne 0 ]; then
            echo_success "the pcert load success"
            echo "the pcert load success" >>$SAVEFILE
        else
            echo_fail "the pcert load fail"
            echo "the pcert load fail" >>$SAVEFILE
            error_quit
        fi
    else
        # 导入华为根，导根后，OS，BMC会进行重启
        load_pcert
    fi
fi
#删除BMA
delete_bma_function
#支持SP升级功能 UADP1087918
upgrade_sp_function
# 新增产品名定制功能 UAP1087928
# 设置机器名称
set_machine_name "${produce_name}"
# 检测机器名称
check_machine_name "${produce_name}"
# 设置机器别名
set_machine_alias
# 白牌包
wbd_config_function
# 清除日志
clear_log

ok_quit