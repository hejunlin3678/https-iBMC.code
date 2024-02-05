#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

READ_ELABEL_STRING=
BOARD_SERIAL_NUMBER=
FRU_ID=0
AREA_NUMBER=0
FIELD_NUMBER=0
FRUID_SET=

#设置BMC系统序列号
set_system_num()
{
	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=3

	echo "set system_num:$1" >> $SAVEFILE

	write_elabel "$1"
}

#读取ProductSerialNumber还原BMC系统序列号
restore_system_num()
{
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=4
	read_elabel

	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=3

	echo "restore system_num:$READ_ELABEL_STRING" >> $SAVEFILE

	write_elabel "$READ_ELABEL_STRING"
}

#获取单板序列号并保存在BOARD_SERIAL_NUMBER
get_board_serial_number()
{
	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=3
	READ_ELABEL_STRING=""
	BOARD_SERIAL_NUMBER=""

	read_elabel
	BOARD_SERIAL_NUMBER=$READ_ELABEL_STRING
	
	echo "get BOARD_SERIAL_NUMBER:$BOARD_SERIAL_NUMBER" >> $SAVEFILE
}

#board part num 是由前缀03和board sn的前6个字符组成
set_board_part_num()
{
	BOARD_PART_NUM_PREFIX="03" 
    BOARD_PART_NUM_SUFFIX=`echo ${BOARD_SERIAL_NUMBER:0:6}`
	BOARD_PART_NUM=$BOARD_PART_NUM_PREFIX$BOARD_PART_NUM_SUFFIX 
	
	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=4

	echo "set board_part_num:$BOARD_PART_NUM" >>$SAVEFILE	
	write_elabel "$BOARD_PART_NUM"
}

#将board part number置为空
restore_board_part_num()
{
	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=4

	echo "restore board_part_num:$WRITE_ELABEL_STRING" >>$SAVEFILE

	write_elabel ""
}

#写BMC资产标签
set_asset_tag()
{
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=5

	echo "set bmc_assettag:$1" >>$SAVEFILE

	write_elabel "$1"
	
    run_cmd "f 0 30 90 06 00 aa" "0f 31 00 90 00 00"
    if [ $? != 0 ] ; then
		echo_fail "update assettag:$1 fail"
		echo "update assettag:$1 fail" >>$SAVEFILE
		error_quit
    else
		echo_success "update assettag:$1 success"
		echo "update assettag:$1 success" >>$SAVEFILE
		return 0
    fi
}

#设置机器名称 比如Tecal RH2286
set_machine_name()
{
    #判断是否是特别定制化的单板，如果是的话，就不要修改标志位。
    run_cmd "f 0 30 90 59 04 00 00" "0f 31 00 90 00 00 03"
    if [ $? -ne 0 ]; then
        #定制化标志位
        run_cmd "f 00 30 90 21 04 01" "0f 31 00 90 00"
        if [ $? != 0 ] ; then
            echo_fail "Set support_customize_dft fail!"
            echo "Set support_customize_dft fail!" >>$SAVEFILE
            error_quit
        fi
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
}


#设置机器别名
set_machine_alias()
{
	local fun_id=48
	local key_id=$(word2string 0xffff)
	local key_type=00
	local key_len=01
	local key_val=00
	local info_id=0
	local info_type=0
	local info_len=0
	local info_val=0
	local MACHINEALIAS=0
	local key_string=0
	local default_machine_alias=0
	local machine_alias=0
	local set_data=0
	local SET_MACHINEALIAS_FLAG=0

	#1 获取机器别名
	getini $CONFIGFILE BMCSet_MACHINEALIAS
	MACHINEALIAS=$parameter

	if [ "${#MACHINEALIAS}" -gt 64 ] ; then
		echo_fail "The greatest machine alias length is 64! Input length is ${#MACHINEALIAS}.Set product alias $MACHINEALIAS fail!"
		echo "The greatest machine alias length is 64! Input length is ${#MACHINEALIAS}.Set product alias $MACHINEALIAS fail!" >>$SAVEFILE 
		error_quit
	fi
	
	#2 解析机器别名
	if [ "$MACHINEALIAS" == "" ] ; then
		getini $CONFIGFILE BMCSet_Cooperation_MACHINEALIAS
		Cooperation_MACHINEALIAS=$parameter
		if [ "${#Cooperation_MACHINEALIAS}" -gt 64 ] ; then
			echo_and_save_fail "The greatest machine alias length is 64! Input length is ${#Cooperation_MACHINEALIAS}.Set product alias $Cooperation_MACHINEALIAS fail!"
			error_quit
		fi
		if [ "$Cooperation_MACHINEALIAS" == "" ] ; then
			#获取default alais默认值
			info_id=12
			key_string="$key_id $key_type $key_len $key_val"
			get_prop_cmd_rsp $fun_id "$key_string" $info_id
			if [ $? != 0 ] ; then
				echo_fail "Get default machine alias fail. Set product alias $MACHINEALIAS fail!"
				echo "Get default machine_alias fail. Set product alias $MACHINEALIAS fail!" >>$SAVEFILE
				error_quit
			fi
			default_machine_alias=${g_get_prop_cmd_rsp:12}
			info_len=${g_get_prop_cmd_rsp:9:2}
			info_val=$default_machine_alias
			
			#获取alais值
			info_id=8
			get_prop_cmd_rsp $fun_id "$key_string" $info_id
			if [ $? != 0 ] ; then
				echo_fail "Get machine alias fail. Set product alias $MACHINEALIAS fail!"
				echo "Get machine_alias fail. Set product alias $MACHINEALIAS fail!" >>$SAVEFILE
				error_quit
			fi
			machine_alias=${g_get_prop_cmd_rsp:12}
			
			#比较不相等则进行设置
			if [ "$default_machine_alias" ==  "$machine_alias" ]; then
				echo_success "Set BMCSet_MACHINEALIAS success"
				echo "Set BMCSet_MACHINEALIAS success" >>$SAVEFILE
				return 0
			fi
		elif [ "$Cooperation_MACHINEALIAS" == "null" ] ; then
			SET_MACHINEALIAS_FLAG=1
			#设置为空
			info_val=""
			strlen=`echo ${#info_val}`
			info_len=`printf "%02x" $strlen`
		elif [ "$Cooperation_MACHINEALIAS" != "" ] ; then
			SET_MACHINEALIAS_FLAG=1
			strlen=`echo ${#Cooperation_MACHINEALIAS}`
			info_val=$(string2hex "$Cooperation_MACHINEALIAS")
			info_len=`printf "%02x" $strlen`
		fi
	elif [ "$MACHINEALIAS" == "null" ] ; then
		#设置为空
		info_val=""
		strlen=`echo ${#info_val}`
		info_len=`printf "%02x" $strlen`
	else
		
		strlen=`echo ${#MACHINEALIAS}`
		info_val=$(string2hex "$MACHINEALIAS")
		info_len=`printf "%02x" $strlen`
	fi

	#3 下发命令
	info_id=$(word2string 8)
	info_type=73

	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		if [ $SET_MACHINEALIAS_FLAG -eq 1 ] ; then
			echo_and_save_fail "Set product alias $Cooperation_MACHINEALIAS fail!"
		else
			echo_and_save_fail "Set product alias $MACHINEALIAS fail!"
		fi
		error_quit
	else
		if [ $SET_MACHINEALIAS_FLAG -eq 1 ] ; then
			echo_and_save_success "Set product alias $Cooperation_MACHINEALIAS success!"
		else
			echo_and_save_success "Set product alias $MACHINEALIAS success!"
		fi
		return 0
	fi
}

#清除机器名称 写空
restore_machine_name()
{
	check_is_new_name
	#ARM服务器新老名称切换需求特殊处理
	if [ $? = 1 ] ; then
		#恢复定制化标志位为02
		run_cmd "f 00 30 90 21 04 02" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "TaiShan restore support_customize_dft fail!"
			echo "TaiShan restore support_customize_dft fail!" >>$SAVEFILE
			error_quit
		fi
	else
		#恢复定制化标志位为00
		run_cmd "f 00 30 90 21 04 00" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "restore support_customize_dft fail!"
			echo "restore support_customize_dft fail!" >>$SAVEFILE
			error_quit
		fi
	fi
	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=1
	echo_success "restore machine_name success!"
	echo "restore machine_name success!" >>$SAVEFILE

	write_elabel ""
}

#写chassis part number
set_chassis_part_number()
{
	FRU_ID=0
	AREA_NUMBER=1
	FIELD_NUMBER=1

	echo "set chassis_part_number:$1" >>$SAVEFILE

	write_elabel "$1"

    run_cmd "f 0 30 90 06 00 aa" "0f 31 00 90 00 00"
    if [ $? != 0 ] ; then
		echo_fail "update chassis part number:$1 fail"
		echo "update chassis part number:$1 fail" >>$SAVEFILE
		error_quit
    else
		echo_success "update chassis part number:$1 success"
		echo "update chassis part number:$1 success" >>$SAVEFILE
		return 0
    fi
}

#写location id
set_location_id()
{
	FRU_ID=0
	AREA_NUMBER=1
	FIELD_NUMBER=3
	WRITE_ELABEL_STRING="$1"

	#get_nodeid_info(机框节点需要加上槽位号)			
	#if [ "$NodeID_get" != "" ] ; then
	#	WRITE_ELABEL_STRING=$WRITE_ELABEL_STRING-$NodeID_get
	#fi						
	
	echo "set location id:$WRITE_ELABEL_STRING" >>$SAVEFILE

	write_elabel "$WRITE_ELABEL_STRING"

    run_cmd "f 0 30 90 06 00 aa" "0f 31 00 90 00 00"
    if [ $? != 0 ] ; then
		echo_fail "update location id:$WRITE_ELABEL_STRING fail"
		echo "update location id:$WRITE_ELABEL_STRING fail" >>$SAVEFILE
		error_quit
    else
		echo_success "update location id:$WRITE_ELABEL_STRING success"
		echo "update location id:$WRITE_ELABEL_STRING success" >>$SAVEFILE
		return 0
    fi
}

#设置机框类型
set_chassis_type()
{
    #处理一下输入参数
    chassistype=`printf "%02x" $1`
    run_cmd "f 0 30 90 04 00 01 00 00 01 $chassistype"  "0f 31 00 90 00"	
	
    run_cmd "f 0 30 90 06 00 aa" "0f 31 00 90 00 00"
    if [ $? != 0 ] ; then
		echo_fail "update chassis type:$1 fail"
		echo "update chassis type:$1 fail" >>$SAVEFILE
		error_quit
    else
		echo_success "update chassis type:$1 success"
		echo "update chassis type:$1 success" >>$SAVEFILE
		return 0
    fi
}


#设置厂商名称
set_manufacturer_name()
{
 	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=1
	WRITE_ELABEL_STRING="$1"

	echo "Set board Manufacturer:$WRITE_ELABEL_STRING" >>$SAVEFILE

	write_elabel "$WRITE_ELABEL_STRING"
	
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=0
	WRITE_ELABEL_STRING="$1"

	echo "Set product Manufacturer:$WRITE_ELABEL_STRING" >>$SAVEFILE

	write_elabel "$WRITE_ELABEL_STRING"
	
 	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=0
	WRITE_ELABEL_STRING="$1"

	echo "Set system Manufacturer:$WRITE_ELABEL_STRING" >>$SAVEFILE

	write_elabel "$WRITE_ELABEL_STRING"
	   
	run_cmd "f 0 30 90 06 00 aa" "0f 31 00 90 00 00"
    if [ $? != 0 ] ; then
		echo_fail "Update manufacturer name:$WRITE_ELABEL_STRING fail"
		echo "Update manufacturer name:$WRITE_ELABEL_STRING fail" >>$SAVEFILE
		error_quit
    else
		echo_success "Update manufacturer name:$WRITE_ELABEL_STRING success"
		echo "Update manufacturer name:$WRITE_ELABEL_STRING success" >>$SAVEFILE
		return 0
    fi
}


#配置第三方信息
set_3rd_info()
{		
	getini $CONFIGFILE BMCSet_3rdInfoEnable
	THIRDINFOENABLE=$parameter
	if [ "$THIRDINFOENABLE" != "on" ] ;then
		return 0
	fi	
	
	getini $CONFIGFILE  BMCSet_3rdInfo
	THRIRD_INFO=$parameter
	if [ "$THRIRD_INFO" != "" ] ; then
		#整个字符串长度
		info_strlen=`echo ${#THRIRD_INFO}`
		
		if [ $info_strlen -gt 255 ] ; then
	        echo_fail "the length of 3rdinfo is err!"
	        echo "the length of 3rdinfo is err!" >>$SAVEFILE
	        error_quit			
		fi
		
		if [ $info_strlen -le 15 ] ; then
			string1=$THRIRD_INFO
			string2=""	
		else
			string1=`echo ${THRIRD_INFO:0:15}`
			string2=`echo ${THRIRD_INFO:15}`		
		fi
		
		#获取前15个字符，并把他们转化成十六进制数据
		converstring2hex "$string1"		
		
		#不管字符串长度多少，先发送第一帧
		info_len=`printf %02x $info_strlen`
		run_cmd "f 0 6 58 c2 00 $info_len $TMP_STRING" "0f 07 00 58 00"
    	if [ $? != 0 ] ; then
	        echo_fail "Set third info fail!"
	        echo "Set third info fail!" >>$SAVEFILE
	        error_quit
	    fi		
		#比较字符串长度是否大于15，如果大于15，则需要循环多帧发完，否则结束
		infotimes=1		
		info_strlen=`echo ${#string2}`
		
		while [ "$string2" != "" ] ; do
		    if [ $info_strlen -le 16 ] ; then
				string1=$string2
				string2=""
			else
				string1=`echo ${string2:0:16}`
				string2=`echo ${string2:16}`
			fi
			
			info_strlen=`echo ${#string2}`
			hexinfotimes=`printf "%x" ${infotimes}`

			converstring2hex "$string1"
			 
			run_cmd  "f 0 6 58 c2 $hexinfotimes $TMP_STRING "  "0f 07 00 58 00"
			if [ $? != 0 ] ; then
	        	echo_fail "Set third info fail!"
	        	echo "Set third info fail!" >>$SAVEFILE
	        	error_quit
	    	fi
			let infotimes=${infotimes}+1
		done			
		echo_success "Set third info $THRIRD_INFO success!"
	else
	    echo_fail "The config BMCSet_3rdInfo is null!"
	    echo "The config BMCSet_3rdInfo is null!" >>$SAVEFILE
	    error_quit	
	fi		
}


#获取X6800\X6000最大节点数
get_max_blade_count()
{
	local fun_id=$(word2string 19)
	local key_id=$(word2string 0xffff) 
	local key_type=79
	local key_len="01"
	local key_val=`printf "%02x" 0`
	local key_id_string="$key_id $key_type $key_len $key_val"
	local info_id=$(word2string 5)
	local end_flag=0
	local offset=0
	local max_len=`printf "%02x" $MAX_FUN_STRING_LEN`
	GET_PROP_INF_STRING=""
		
	while true
	do
		offset_s=$(word2string $offset)
		run_cmd "f 0 30 93 db 07 00 36 $fun_id 1 $max_len $offset_s $key_id_string $info_id" "0f 31 00 93 00 db 07 00"
		if [ $? != 0 ] ; then
			echo_fail "Get property value fail!"
			echo "Get property value fail!" >>$SAVEFILE
			error_quit
		fi
		
		offset=`expr $MAX_FUN_STRING_LEN + $offset`
		tmp_arr=(`cat $IPMI_CMD_RETURN_FILE`)
		end_flag=${tmp_arr[8]}
		GET_PROP_INF_STRING=$GET_PROP_INF_STRING" ${tmp_arr[@]:9}"
		if [ "$end_flag" = "01" ] ; then
			sleep 1
			continue
		else					
			break
		fi
	done
	
	GET_PROP_INF_STRING=$(remove_space "$GET_PROP_INF_STRING")

	value_string=`echo $GET_PROP_INF_STRING | cut -b 12-14`
	value_string=$(remove_space "$value_string")
	local value=$((16#${value_string}))
	MAX_BLADE_COUNT=$value
	echo "$MAX_BLADE_COUNT"
}
#配置是否同步电子标签
set_elabel_sync_state()
{
	fun_id=19
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len=01
	key_val=01
	info_id=$(word2string 7) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		echo_fail "set_assetflag_sync_state $1 Fail"
		echo "set_assetflag_sync_state $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set_assetflag_sync_state $1 success!"
		echo "set_assetflag_sync_state $1 success!" >>$SAVEFILE
	fi
}

#设置同步电子标签后缀
config_serialnum_suffix_info()
{
 	SUFFIX_INFO=$2
	string2substring "$SUFFIX_INFO"
	strlenlen=`echo ${#SUFFIX_INFO}`
	fun_id=19
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len=1
	key_val=1
	info_id=$(word2string $1) 
	info_type=73 #s
	info_len=`printf "%02x" $strlenlen`
	info_val=$TMP_STRING
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		return 1
	else
		return 0       	
   	fi
}
#设置节点后缀
set_serialnum_suffix_by_slotid()
{
	get_max_blade_count
	echo $MAX_BLADE_COUNT
	for((I=0; I<$MAX_BLADE_COUNT; ++I))
	do
		ID=`expr $I + 1`
		getini $CONFIGFILE  Custom_SerialnumSuffixBySlot$ID
		SERIALNUM_SUFFIX_BY_SLOT=$parameter

		#配置项为空，采用默认值		
		if [ "$SERIALNUM_SUFFIX_BY_SLOT" != "" ] ; then
			#判断参数是否含有空格
			is_have_space "$SERIALNUM_SUFFIX_BY_SLOT"
			if [ $? == 0 ] ; then
				echo_and_save_fail "The serialnum suffix($SERIALNUM_SUFFIX_BY_SLOT) contains spaces"
				error_quit
			fi
			#判断参数长度
			if [ "${#SERIALNUM_SUFFIX_BY_SLOT}" -gt 10 ] ; then
        			echo_fail "Surpasses the greatest length limit 10! set $I failed!"
        			error_quit
    		fi
    			suffix=$suffix$SERIALNUM_SUFFIX_BY_SLOT","
		else
				suffix=$suffix","
		fi
	done
	echo $suffix
	config_serialnum_suffix_info 8 "$suffix"
	if [ $? != 0 ] ; then
		echo_fail "set SerialnumSuffix Fail"
		echo "set SerialnumSuffix Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set SerialnumSuffix ok!"
		echo "set SerialnumSuffix ok!" >>$SAVEFILE
	fi

}

#还原所有节点后缀
restore_serialnum_suffix_by_slotid()
{
	get_max_blade_count
	echo $MAX_BLADE_COUNT
	for((I=0; I<$MAX_BLADE_COUNT; ++I))
	do
		suffix=$suffix","
	done
	echo $suffix
	config_serialnum_suffix_info 8 "$suffix"
	if [ $? != 0 ] ; then
		echo_fail "restore SerialnumSuffix Fail"
		echo "restore SerialnumSuffix Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "restore SerialnumSuffix ok!"
		echo "restore SerialnumSuffix ok!" >>$SAVEFILE
	fi
}

#设置FRU 扩展域内容(24~128字节)
set_fru_long_extend_label_info()
{
	local max_fru_string_len=128
	local max_string_once=19
	local strlen_fru=`echo ${#4}`
	local repeat_times=`expr $strlen_fru / $max_string_once`
	local remain_flag=`expr $strlen_fru % $max_string_once`
	if [ $remain_flag != 0 ] ; then
		repeat_times=`expr $repeat_times + 1`
	fi
	repeat_times=`printf %d $repeat_times`
	local fru_string=$(remove_space "$4")
	string2substring "$fru_string" $max_fru_string_len
	local i
	for((i=1;i<=$repeat_times;i++))
	do
		local start=`expr $i \* $max_string_once \* 3 - $max_string_once \* 3`
		#最后一条命令格式不一样
		if [ $i = $repeat_times ] ; then
			local offset=`expr $i \* $max_string_once - $max_string_once`
			local end=`expr $strlen_fru \* 3 - $offset \* 3`
			local one_msg_len=`expr $strlen_fru - $i \* $max_string_once + $max_string_once`
		else
			local offset=`expr $i \* $max_string_once + 128 - $max_string_once`
			local end=`expr $max_string_once \* 3`
			local one_msg_len=$max_string_once
		fi
		local offset_02x=`printf %02x $offset`
		local tmp_string=`echo ${TMP_STRING:$start:$end}`
		local one_msg_len_02x=`printf %02x $one_msg_len`
		tmp_string=$(remove_space "$tmp_string")
		run_cmd "f 0 30 90 04 $1 $2 $3 $offset_02x $one_msg_len_02x $tmp_string" "0f 31 00 90 00"
		if [ $? != 0 ] ; then		
			echo_fail "Set fru $1 description:$tmp_string fail!"
			echo "Set fru $1 description:$tmp_string fail!" >>$SAVEFILE
			return 1
		fi
	done
	return 0
}

#设置FRU 扩展域内容(0~24)
set_fru_extend_label_info()
{
	local max_fru_string_len=128
	local max_ipmi_cmd_len=23
	local min_fru_string_len=0
	local set_flag=0
	strlen=`echo ${#4}`
	strlen_02x=`printf %02x $strlen`
	if [ "$strlen" -le "$max_ipmi_cmd_len" ] && [ "$strlen" -ge "$min_fru_string_len" ]; then		
		string2substring "$4"
		TMP_STRING=$(remove_space "$TMP_STRING")
		run_cmd "f 0 30 90 04 $1 $2 $3 00 $strlen_02x $TMP_STRING" "0f 31 00 90 00"
		set_flag=$?
	elif [ "$strlen" -le "$max_fru_string_len" ] && [ "$strlen" -ge "$max_ipmi_cmd_len" ]; then
		set_fru_long_extend_label_info $1 $2 $3 "$4"
		set_flag=$?
	else
		set_flag=1
	fi
	if [ $set_flag = 0 ] ; then
		echo_success "Set fru $1 extend info:$4 success!"
		echo "Set fru $1 extend info:$4 success!" >>$SAVEFILE
	else
		echo_fail "Set fru $1 extend info:$4 fail!"
		echo "Set fru $1 extend info:$4 fail!" >>$SAVEFILE
		error_quit
	fi
}

#设置FRU厂商信息
set_fru_manufacturer_info()
{
	FRU_ID=$1
	AREA_NUMBER=$2
	FIELD_NUMBER=$3
	WRITE_ELABEL_STRING="$4"

	echo "Set fru $fruid manufacturer:$WRITE_ELABEL_STRING" >>$SAVEFILE

	write_elabel "$WRITE_ELABEL_STRING"
}

#设置FRU扩展域信息
set_fru_extend_info()
{
	local tmp=1
	fruid=$1
	AREA_NUMBER=$2
	FIELD_NUMBER=$3
	WRITE_ELABEL_STRING="$4"
	WRITE_SPLIT_STRING="$4"
	
	if [[ $4 =~ ";" ]]; then
		tmp=1
		while true
		do
			split=`echo $WRITE_ELABEL_STRING | cut -d ";" -f$tmp`
			WRITE_SPLIT_STRING=$split
			if [ "$split" != "" ]; then
				set_fru_extend_label_info $fruid $AREA_NUMBER $FIELD_NUMBER "$WRITE_SPLIT_STRING"
				((tmp++))
			else
				break
			fi
		done				
	else
		set_fru_extend_label_info $fruid $AREA_NUMBER $FIELD_NUMBER "$WRITE_ELABEL_STRING"
	fi		
}

#定制化除主板外的其他板卡类FRU白牌定制化信息
set_fru_asset_info()
{	
	local SET_BOARD_MANUFACTURER_FLAG=0
	local SET_PRODUCT_MANUFACTURER_FLAG=0

	#如果客户没有定制，先判断伙伴有没有定制
	#1 获取FRU Board域的厂商信息
	getini $CONFIGFILE BMCSet_CustomBoardManufacturer
	BoardManufacturer=$parameter
	if [ "$BoardManufacturer" == "" ] ; then
		SET_BOARD_MANUFACTURER_FLAG=1
		getini $CONFIGFILE BMCSet_Cooperation_CustomBoardManufacturer
		Cooperation_BoardManufacturer=$parameter
	fi
	
	#2 获取FRU Product域的厂商信息
	getini $CONFIGFILE BMCSet_CustomProductManufacturer
	ProductManufacturer=$parameter
	if [ "$ProductManufacturer" == "" ] ; then
		SET_PRODUCT_MANUFACTURER_FLAG=1
		getini $CONFIGFILE BMCSet_Cooperation_CustomProductManufacturer
		Cooperation_ProductManufacturer=$parameter
	fi
	
	#4 获取FRU Board扩展域信息
	getini $CONFIGFILE BMCSet_CustomBoardExtendLabel
	BoardExtendLabel=$parameter
	
	#空定制化不执行后续校验操作(此定制化项仅在白牌需求里面会用到，因此正常装备场景不用执行)
	if [ "$BoardManufacturer" == "" ] && [ "$ProductManufacturer" == "" ] && [ "$BoardExtendLabel" == "" ] && [ "$Cooperation_BoardManufacturer" == "" ] && [ "$Cooperation_ProductManufacturer" == "" ] ; then
		return 0
	fi
	
	#5 获取所有的FRU ID信息
	read_fruid_set
	
	#6 遍历所有FRU，对每个FRU执行定制操作
	for fruid in $FRUID_SET
	do
		#定制除主板外的其他板卡的FRU
		if [ $fruid == 00 ]; then
			continue
		fi
		
		#判断FRU是否支持装备电子标签操作
		check_fru_support_elabel $fruid
		if [ $? != 0 ]; then
			continue			
		else	
			#设置FRU Board域厂商信息
			if [ "$BoardManufacturer" != "" ] || [ "$Cooperation_BoardManufacturer" != "" ]; then
				if [ $SET_BOARD_MANUFACTURER_FLAG -eq 1 ] ; then
					set_fru_manufacturer_info $fruid 02 01 "$Cooperation_BoardManufacturer"
				else
					set_fru_manufacturer_info $fruid 02 01 "$BoardManufacturer"
				fi
			fi
			
			#设置FRU Product域厂商信息
			if [ "$ProductManufacturer" != "" ] || [ "$Cooperation_ProductManufacturer" != "" ]; then
				if [ $SET_PRODUCT_MANUFACTURER_FLAG -eq 1 ] ; then
					set_fru_manufacturer_info $fruid 03 00 "$Cooperation_ProductManufacturer"
				else
					set_fru_manufacturer_info $fruid 03 00 "$ProductManufacturer"
				fi
			fi			

			#设置FRU Board扩展域信息
			if [ "$BoardExtendLabel" != "" ]; then
				set_fru_extend_info $fruid 05 00 "$BoardExtendLabel"
			fi
			
			run_cmd "f 0 30 90 06 $fruid aa" "0f 31 00 90 00 00"
			if [ $? != 0 ] ; then
				echo_fail "Update fru $fruid info fail!"
				echo "Update fru $fruid info fail!" >>$SAVEFILE
				error_quit
			else
				echo_success "Update fru $fruid info success!"
				echo "Update fru $fruid info success!" >>$SAVEFILE
			fi
		fi
	done	
}

#设置主板Board扩展域信息
set_board_fru_extend_info()
{
	FRU_ID=00
	set_fru_extend_info $FRU_ID 05 00 "$1"
	
	run_cmd "f 0 30 90 06 $FRU_ID aa" "0f 31 00 90 00 00"
	if [ $? != 0 ] ; then
		echo_fail "Update fru $FRU_ID board extend info:$1 fail!"
		echo "Update fru $FRU_ID board extend info:$1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Update fru $FRU_ID board extend info:$1 success!"
		echo "Update fru $FRU_ID board extend info:$1 success!" >>$SAVEFILE
	fi
}

#设置主板Product扩展域信息
set_product_fru_extend_info()
{
	FRU_ID=00
	set_fru_extend_info $FRU_ID 03 07 "$1"
	
	run_cmd "f 0 30 90 06 $FRU_ID aa" "0f 31 00 90 00 00"
	if [ $? != 0 ] ; then
		echo_fail "Update fru $FRU_ID product extend info:$1 fail!"
		echo "Update fru $FRU_ID product extend info:$1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Update fru $FRU_ID product extend info:$1 success!"
		echo "Update fru $FRU_ID product extend info:$1 success!" >>$SAVEFILE
	fi	
}

main()
{
	chmod +x *
	####电子标签配置#####################################################
	#1.1、设置序列号  未定制，不置空序列号
	#X8000序列号由rmc同步，BMC不进行定制化
	getini $CONFIGFILE BMCSet_CustomSerialNUM
	IS_SET_SYS_NUM=$parameter
	if [ "$IS_SET_SYS_NUM" = "on" ] ; then
		getini  $CONFIGFILE  Custom_SerialNUM
		SYS_NUM=$parameter
		
		if [ "$SYS_NUM" != "" ] ; then
			if [ "${#SYS_NUM}" -gt 32 ] ; then
					echo_fail "Surpasses the greatest length limit 32! set BMCSet_CustomSerialNUM failed!"
					error_quit
			fi
			set_system_num "$SYS_NUM"
			sleep 1
		else
			echo_fail "SYS_NUM=NULL set SYS_NUM fail" 
			echo "SYS_NUM=NULL set SYS_NUM fail" >> $SAVEFILE
			error_quit
		fi
	else
		restore_system_num
	fi
	
	
	
	#1.3、设置资产标签
	#X8000资产标签由rmc同步，BMC不进行定制化
	#if [ "$BLADE_TYPE" != "0" ] ; then
	getini $CONFIGFILE  BMCSet_CustomAssetTag
	ASSET_NUM=$parameter
	if [ "$ASSET_NUM" = "on" ] ; then
		getini $CONFIGFILE Custom_AssetTag
		ASSET_NUM=$parameter

		if [ "$ASSET_NUM" != "" ] ; then
			set_asset_tag "$ASSET_NUM"
			sleep 1
		else
			echo_fail "ASSET_NUM=NULL set ASSET_NUM fail" 
			echo "ASSET_NUM=NULL set ASSET_NUM fail" >>$SAVEFILE
			error_quit
		fi
	else
		set_asset_tag ""  #写空为恢复
	fi  
	
	#1.4、设置机器名称（设置System Product Name）
	getini $CONFIGFILE BMCSet_MACHINENAME
	MACHINENAME=$parameter

	need_special_dispose
	special_dispose="$?"
	if [ "${#MACHINENAME}" -gt 48 ] ; then
			echo_fail "Surpasses the greatest length limit 48! set BMCSet_MACHINENAME failed!"
			error_quit
	fi
	if [ "$MACHINENAME" != "" ] ; then
		set_machine_name "$MACHINENAME"
		sleep 1
	elif [ "$MACHINENAME" = "" ] && [ "${special_dispose}" -eq 1 ]; then
		getini $CONFIGFILE BMCSet_Cooperation_MACHINENAME
		Cooperation_MACHINENAME=$parameter
		if [ "${#Cooperation_MACHINENAME}" -gt 48 ] ; then
			echo_and_save_fail "Surpasses the greatest length limit 48! set BMCSet_Cooperation_MACHINENAME failed!"
			error_quit
		fi
		if [ "$Cooperation_MACHINENAME" != "" ] ; then
			set_machine_name "$Cooperation_MACHINENAME"
			sleep 1
		else
			restore_machine_name
			sleep 1
		fi
	fi
	
	#设置机器别名
	set_machine_alias
	
	#1.5、设置Chassis Part Number
	#X8000Chassis Part Number由rmc同步，BMC不进行定制化
	getini $CONFIGFILE  BMCSet_CustomChassisPartNumber
	PARTNUMBER=$parameter
	if [ "$PARTNUMBER" = "on" ] ; then
		getini $CONFIGFILE Custom_ChassisPartNumber
		PARTNUMBER=$parameter
		
		if [ "$PARTNUMBER" != "" ] ; then
			set_chassis_part_number "$PARTNUMBER"
			sleep 1
		else
			echo_fail "PARTNUMBER is NULL set PARTNUMBER fail" 
			echo "PARTNUMBER is NULL set PARTNUMBER fail" >>$SAVEFILE
	        error_quit
		fi
	else
		set_chassis_part_number ""  
	fi

	#定制化单板Part Number
	getini  $CONFIGFILE  BMCSet_CustomBoardPartNumberFlag
	BOARD_PART_NUMBER_FLAG=$parameter
	if [ "$BOARD_PART_NUMBER_FLAG" = "on" ] ; then
		get_board_serial_number
		if [ "$BOARD_SERIAL_NUMBER" != "" ] ; then
			set_board_part_num
			sleep 1
		else
			echo_fail "BOARD_SERIAL_NUMBER is NULL" 
			echo "BOARD_SERIAL_NUMBER is NULL " >>$SAVEFILE
			error_quit		
		fi
	#else
		#restore_board_part_num
	fi

	#1.6、设置location id(不支持)
	#getini $CONFIGFILE  Custom_LocationID_Check
	#LOCATIONIDENABLE=$parameter
	#if [ "$LOCATIONIDENABLE" = "on" ] ; then
	#	getini $CONFIGFILE Custom_LocationID_CheckValue
	#	LOCATIONID=$parameter
	#	
	#	if [ "$LOCATIONID" != "" ] ; then
	#		set_location_id $LOCATIONID
	#		sleep 1
	#	else
	#		echo_fail "LOCATIONID=NULL set LOCATIONID fail" 
	#		echo "LOCATIONID=NULL set LOCATIONID fail" >>$SAVEFILE
	#		error_quit
	#	fi
	#fi

	########设置chassis bype域，X8000写入25，X6000写入1，其他为默认值
	getini $CONFIGFILE  BMCSet_CustomChassisType
	PARTNUMBER=$parameter
	if [ "$PARTNUMBER" = "on" ] ; then
		getini $CONFIGFILE Custom_ChassisChassisType
		PARTNUMBER=$parameter
		
		if [ "$PARTNUMBER" != "" ] ; then
			set_chassis_type $PARTNUMBER
			sleep 1
		else
			echo_fail "PARTNUMBER is NULL set chassis type PARTNUMBER fail" 
			echo "PARTNUMBER is NULL set chassis type PARTNUMBER fail" >>$SAVEFILE
			error_quit
		fi
	else
		set_chassis_type "0"
	fi
	
	#定制化厂商名称
	getini $CONFIGFILE  BMCSet_CustomManufacturer
	IS_MANUFACTURER=$parameter
	if [ "$IS_MANUFACTURER" = "on" ] ; then
		getini $CONFIGFILE Custom_Manufacturer
		MANUFACTURER=$parameter
		if [ "$MANUFACTURER" != "" ] ; then
			set_manufacturer_name "$MANUFACTURER"
			sleep 1
		else
			echo_fail "Manufacturer is NULL, set manufacturer name $MANUFACTURER fail" 
			echo "Manufacturer is NULL, set manufacturer name $MANUFACTURER fail" >>$SAVEFILE
			error_quit
		fi
	else
		getini $CONFIGFILE Custom_Cooperation_Manufacturer
		Cooperation_MANUFACTURER=$parameter
		sleep 1
		if [ "$Cooperation_MANUFACTURER" != "" ] && ([ "$bp_flag" = "01" ] || [ "$bp_flag" = "11" ]) ; then
			set_manufacturer_name "$Cooperation_MANUFACTURER"
			sleep 1
		elif [ "$Cooperation_MANUFACTURER" == "" ] && ([ "$bp_flag" = "01" ] || [ "$bp_flag" = "11" ]) ; then
			echo_and_save_fail "Manufacturer is NULL, set manufacturer name $Cooperation_MANUFACTURER fail"
			error_quit
		else
			MANUFACTURER="Huawei"
			set_manufacturer_name "$MANUFACTURER"
		fi
	fi	
	
	#定制化主板的FRU Board扩展域信息
	getini $CONFIGFILE BMCSet_FRUDescription
	if [ "$parameter" != ""	]; then
		FRUDescription=$parameter		
		set_board_fru_extend_info "$FRUDescription"		
	fi
	sleep 1
	
	#定制化主板的FRU Product扩展域信息
	getini $CONFIGFILE BMCSet_FRUProductExtraDescription
	if [ "$parameter" != ""	]; then
		FRUProductExtraDescription=$parameter
		set_product_fru_extend_info "$FRUProductExtraDescription"
	fi
	sleep 1
	
	#定制化除主板外的其他FRU白牌定制化信息
	set_fru_asset_info
	sleep 1
	
	########配置第三方用户信息###
	set_3rd_info
	
    PRODUCT_NAME_ID=$PRODUCT_ID$BOARD_ID
	if [ $PRODUCT_ID == $PRODUCT_XSERIALS_TYPE ] || [ $PRODUCT_ID = $PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE ] || ([ $PRODUCT_ID == $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID != $BOARD_ATLAS880_ID ] && [ $BOARD_ID != $BOARD_ATLAS800_9010_ID ] && [ $BOARD_ID != $BOARD_G2500_ID ] && [ $BOARD_ID != $BOARD_ATLAS500_ID ]); then	
		echo " only for X-Serials or Atlas Custom"
		#定制化标签后缀	
		getini $CONFIGFILE  BMCSet_CustomSerialnumSuffix
		SERIALNUM_SUFFIX=$parameter
		if [ "$SERIALNUM_SUFFIX" = "on" ] ; then
			set_serialnum_suffix_by_slotid
		fi
		
		#设置资产标签定制化标志 on开启定制化,off表示同步MMC/MM电子标签, X6000 V2默认同步,X6800默认不同步, X6000 V3默认不同步，X6000 V6默认不同步
		getini $CONFIGFILE  BMCSet_CustomAssetTagFlag
		ASSET_TAG_FLAG=$parameter
		#设置资产序列号同步标志 on不同步,off表示同步MMC/MM电子标签， X6000 V2默认同步,X6800默认不同步, X6000 V3默认不同步，X6000 V6默认不同步
		getini $CONFIGFILE  BMCSet_CustomSerialNUMFlag
		PRODUCT_SERIAL_FLAG=$parameter
		
		declare -i product_assettag_flag;
		declare -i product_sn_flag;
		declare -i elabel_sync_flag;
		
        #使用产品+单板Id唯一标识
		if [ $PRODUCT_NAME_ID != $PRODUCT_XH628_XH622_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH620_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321_HIGH_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321_STANDARD ] && \
			[ $PRODUCT_NAME_ID != $PRODUCT_XH321V5_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321V5L_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_G560_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_G560V5_ID ] && \
			[ $PRODUCT_NAME_ID != $PRODUCT_G530V5 ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH628V5_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XA320_A6_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XA320_B0_ID ]&& \
			[ $PRODUCT_NAME_ID != $PRODUCT_XA320_B1_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XA320_B2_ID ]&& [ $PRODUCT_NAME_ID != $PRODUCT_XA320_B3_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XA320_B4_ID ] && \
			[ $PRODUCT_NAME_ID != $PRODUCT_XA320_B5_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321V6_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321CV6_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_D320V2_ID ] && \
			[ $PRODUCT_NAME_ID != $PRODUCT_D320CV2_ID ] ; then
			if [ "$ASSET_TAG_FLAG" = "on" ] ; then
				product_assettag_flag=0;
			else
				product_assettag_flag=1; #bit0：Product AssetTag 同步标志
			fi
			
			if [ "$PRODUCT_SERIAL_FLAG" = "on" ] ; then
				product_sn_flag=0;
			else
				product_sn_flag=2; #bit1：bit1：Product SN 同步标志
			fi
			
			
		else
			if [ "$ASSET_TAG_FLAG" = "off" ] ; then
				product_assettag_flag=1;
			else
				product_assettag_flag=0;
			fi
			
			if [ "$PRODUCT_SERIAL_FLAG" = "off" ] ; then
				product_sn_flag=2;
			else
				product_sn_flag=0;
			fi
			
		fi
		
		elabel_sync_flag=$product_assettag_flag+$product_sn_flag;
		set_elabel_sync_state $elabel_sync_flag
		sleep 1
	fi
}

main
