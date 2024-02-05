#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

FRU_ID=0
AREA_NUMBER=0
FIELD_NUMBER=0
READ_ELABEL_STRING=

#设置预置信息
check_set_3rd_info()
{
    #THIRDINFO #配置文件中的字符串
	#读取第一帧字符串
	run_cmd "f 0 6 59 00 c2 00 00" "0f 07 00 59 00"
	if [ $? != 0 ]; then
		echo_fail "read third info fail!"
		echo "read third info fail!" >> $SAVEFILE
		error_quit
	fi
	hextochar $IPMI_CMD_RETURN_FILE 22 temp_tmp.txt    #此19还需要调试
	READ_ELABEL_STRING=`cat temp_tmp.txt `
	echo $READ_ELABEL_STRING > temp_tmp_info.txt
	
	infotimes=1
	
	while [ $infotimes -le 15 ] ; do
		hexinfotimes=`printf "%x" ${infotimes}`
		run_cmd "f 0 6 59 00 c2 $hexinfotimes 00" "0f 07 00 59 00"
		if [ $? != 0 ]; then
			echo_fail "read third info $i fail!"
			echo "read third info $i fail!" >> $SAVEFILE
			error_quit
		fi
		hextochar $IPMI_CMD_RETURN_FILE 19 temp_tmp.txt    #此19还需要调试
		READ_ELABEL_STRING=`cat temp_tmp.txt`
		READ_ELABEL_STRING=`cat temp_tmp_info.txt`$READ_ELABEL_STRING
		echo $READ_ELABEL_STRING > temp_tmp_info.txt	
	let infotimes=$infotimes+1
	done
	
    if [ "$THIRDINFO" != "$READ_ELABEL_STRING" ] ; then
        echo_fail "check 3rdinfo :$THIRDINFO fail!"
		echo "check 3rdinfo:$THIRDINFO fail!" >>$SAVEFILE
		error_quit
    else
        echo_success "check 3rdinfo:$THIRDINFO success"
		echo "check 3rdinfo:$THIRDINFO success" >>$SAVEFILE
	    return 0
    fi		
}

#校验系统序列号
check_system_num()
{
    #校验系统序列号是否写入    
    
	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=3
	READ_ELABEL_STRING=""

	read_elabel
	
    echo $READ_ELABEL_STRING > temp_tmp.txt
		
	if [ "$SYS_NUM" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "verify system_num:$SYS_NUM fail!"
		echo "verify system_num:$SYS_NUM fail!" >>$SAVEFILE
		error_quit
    else
    	echo_success "verify system_num:$SYS_NUM success"
		echo "verify system_num:$SYS_NUM success" >>$SAVEFILE
		return 0
    fi
}

#校验默认系统序列号
check_restore_system_num()
{
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=4
	READ_ELABEL_STRING=""

	read_elabel
	SYSTEMNUM=$READ_ELABEL_STRING   

    #校验系统序列号
	
	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=3
	READ_ELABEL_STRING=""

	read_elabel
	
    echo $READ_ELABEL_STRING > temp_tmp.txt
	if [ "$SYSTEMNUM" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "verify system_num:$SYSTEMNUM fail!"
		echo "verify system_num:$SYSTEMNUM fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "verify  system_num:$SYSTEMNUM success!"
		echo "verify  system_num:$SYSTEMNUM success!" >>$SAVEFILE
		return 0
	fi
}

#board part num 是由前缀03和board sn的前6个字符组成
check_board_part_num()
{
	BOARD_PART_NUM_PREFIX="03" 
    BOARD_PART_NUM_SUFFIX=`echo ${BOARD_SERIAL_NUMBER:0:6}`
	BOARD_PART_NUM=$BOARD_PART_NUM_PREFIX$BOARD_PART_NUM_SUFFIX 
	
	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=4
	READ_ELABEL_STRING=""
	read_elabel
    echo $READ_ELABEL_STRING > temp_tmp.txt
	
    if [ "$BOARD_PART_NUM" != "$READ_ELABEL_STRING" ] ; then
       	echo_fail "check board_part_num:$BOARD_PART_NUM fail!"
       	echo "check board_part_num:$BOARD_PART_NUM fail!" >>$SAVEFILE
       	error_quit
    else 
       	echo_success "check board_part_num:$BOARD_PART_NUM success!"
       	echo "check board_part_num:$BOARD_PART_NUM success!" >>$SAVEFILE
	   	return 0
    fi	
}

#校验board_part_num是否为空
check_restore_board_part_num()
{
	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=4
	READ_ELABEL_STRING=""
	read_elabel
	
    if [ "$READ_ELABEL_STRING" = "" ] ; then
        echo_success "check  restore_board_part_num:$READ_ELABEL_STRING success!"
        echo "check  restore_board_part_num:$READ_ELABEL_STRING success!" >>$SAVEFILE
	    return 0	
	else
        echo_fail "check  restore_board_part_num:$READ_ELABEL_STRING fail!"
        echo "check  restore_board_part_num:$READ_ELABEL_STRING fail!" >>$SAVEFILE
        error_quit	
	fi
}

get_board_serial_number()
{
	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=3
	READ_ELABEL_STRING=""
	BOARD_SERIAL_NUMBER=""

	read_elabel
	BOARD_SERIAL_NUMBER=$READ_ELABEL_STRING
	
	echo "get BOARD_SERIAL_NUMBER:$BOARD_SERIAL_NUMBER" >>$SAVEFILE
}

#校验资产标签
check_asset_tag()
{
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=5
	READ_ELABEL_STRING=""

	read_elabel
	
	echo $READ_ELABEL_STRING > temp_tmp.txt

	if [ "$ASSET_NUM" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "verify assettag:$ASSET_NUM fail!"
		echo "verify assettag:$ASSET_NUM fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "verify assettag:$ASSET_NUM success"
		echo "verify assettag:$ASSET_NUM success" >>$SAVEFILE
		return 0
	fi
}

#校验TaiShan机器名称是否写入
check_taishan_name()
{
	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=1
	read_elabel
	TAISHAN_NAME="TaiShan 200"
	echo "$READ_ELABEL_STRING" > temp.txt
	grep -wq "$TAISHAN_NAME" temp.txt
	if [ $? = 0 ] ; then			
		echo_success "verify TaiShan Machine name:$READ_ELABEL_STRING success"
		echo "verify TaiShan Machine name:$READ_ELABEL_STRING success" >>$SAVEFILE
	else
		echo_fail "verify TaiShan Machine name:$READ_ELABEL_STRING fail!"
		echo "verify TaiShan Machine name:$READ_ELABEL_STRING fail!" >>$SAVEFILE
		error_quit
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


#校验机器别名
check_machine_alias()
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
	local CHECK_MACHINEALIAS_FLAG=0

	getini $CONFIGFILE  BMCSet_MACHINEALIAS
	MACHINEALIAS=$parameter
	
	#1、解析参数
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
				echo_fail "Get default machine alias failed. Check BMCSet_MACHINEALIAS failed!"
				echo "Get default machine_alias failed. Check BMCSet_MACHINEALIAS failed!" >>$SAVEFILE
				error_quit
			fi
			default_machine_alias=${g_get_prop_cmd_rsp:12}
			info_len=${g_get_prop_cmd_rsp:9:2}
			info_val=$default_machine_alias	

			if [ "$info_val" == "" ] ; then
				echo_success "Check product alias success"
				echo "Check product alias success" >>$SAVEFILE
				return  0
			fi
		elif [ "$Cooperation_MACHINEALIAS" == "null" ] ; then
			CHECK_MACHINEALIAS_FLAG=1
			#设置为空
			info_val=""
			strlen=0
			info_len=`printf "%02x" $strlen`
		else
			CHECK_MACHINEALIAS_FLAG=1
			strlen=`echo ${#Cooperation_MACHINEALIAS}`
			info_val=$(string2hex "$Cooperation_MACHINEALIAS")
			info_len=`printf "%02x" $strlen`
		fi
	elif [ "$MACHINEALIAS" == "null" ] ; then
		info_val=""
		strlen=0
		info_len=`printf "%02x" $strlen` 
	else
		strlen=`echo ${#MACHINEALIAS}`
		info_val=$(string2hex "$MACHINEALIAS")
		info_len=`printf "%02x" $strlen` 			
	fi

	info_id=$(word2string 8)
	info_type=73
	
	#2、下发命令
	expect_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 8 "$expect_data"
	if [ $? != 0 ] ; then
		if [ $CHECK_MACHINEALIAS_FLAG -eq 1 ] ; then
			echo_and_save_fail "Check product alias $Cooperation_MACHINEALIAS fail!"
		else
			echo_and_save_fail "Check product alias $MACHINEALIAS fail!"
		fi
		error_quit              
	else
		if [ $CHECK_MACHINEALIAS_FLAG -eq 1 ] ; then
			echo_and_save_success "Check product alias $Cooperation_MACHINEALIAS success!"
		else
			echo_and_save_success "Check product alias $MACHINEALIAS success!"
		fi
		return 0
	fi
}

#校验chassis part number
check_set_chassis_part_number()
{
	FRU_ID=0
	AREA_NUMBER=1
	FIELD_NUMBER=1
	READ_ELABEL_STRING=""

	read_elabel

	echo $READ_ELABEL_STRING > temp_tmp.txt
	if [ "$PARTNUMBER" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "verify chassis part number:$PARTNUMBER fail!"
		echo "verify chassis part number:$PARTNUMBER fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "verify chassis part number:$PARTNUMBER success"
		echo "verify chassis part number:$PARTNUMBER success" >>$SAVEFILE
		return 0
	fi
}

#校验chassis part number
check_restore_chassis_part_number()
{
	FRU_ID=0
	AREA_NUMBER=1
	FIELD_NUMBER=1
	READ_ELABEL_STRING=""

	read_elabel

	if [ "$READ_ELABEL_STRING" != "" ] ; then
		echo_fail "verify chassis part number:$READ_ELABEL_STRING fail!"
		echo "verify chassis part number:$READ_ELABEL_STRING fail!" >>$SAVEFILE
		error_quit
	fi
	
	echo_success "verify chassis part number:$READ_ELABEL_STRING success"
	echo "verify chassis part number:$READ_ELABEL_STRING success" >>$SAVEFILE
	return 0
}

#校验chassis_type
check_set_chassis_type()
{	
    chassistype=`printf "%02x" $PARTNUMBER`

    run_cmd  "f 0 30 90 05 00 01 00 00 01"  "0f 31 00 90 00 80 $chassistype"	
    if [ $? != 0 ] ; then
        echo_fail "verify chassis type:$PARTNUMBER fail!"
	    echo "verify chassis type:$PARTNUMBER fail!" >>$SAVEFILE
	    error_quit
    else
        echo_success "verify chassis type:$PARTNUMBER success"
	    echo "verify chassis type:$PARTNUMBER success" >>$SAVEFILE
	    return 0
    fi
}

check_restore_chassis_type()
{	
    #写电子标签时如果长度为0则数据data域不用填写,同样读取的时候后台也不会返回data域
    run_cmd  "f 0 30 90 05 00 01 00 00 01"  "0f 31 00 90 00 80"	
    if [ $? != 0 ] ; then
        echo_fail "verify restore chassis type fail!"
	    echo "verify restore chassis type fail!" >>$SAVEFILE
	    error_quit
    else
        echo_success "verify restore chassis type success"
	    echo "verify restore chassis type success" >>$SAVEFILE
	    return 0
    fi	
	
}

check_manufacturer_name()
{
	FRU_ID=0
	AREA_NUMBER=2
	FIELD_NUMBER=1
	READ_ELABEL_STRING=""

	read_elabel

	echo $READ_ELABEL_STRING > temp_tmp.txt
	if [ "$1" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "Verify board manufacturer name:$1 fail!"
		echo "Verify board manufacturer name:$1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Verify board manufacturer name:$1 success"
		echo "Verify board manufacturer name:$1 success" >>$SAVEFILE
	fi
	
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=0
	READ_ELABEL_STRING=""

	read_elabel

	echo $READ_ELABEL_STRING > temp_tmp.txt
	if [ "$1" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "Verify pruduct manufacturer name:$1 fail!"
		echo "Verify pruduct manufacturer name:$1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Verify pruduct manufacturer name:$1 success"
		echo "Verify pruduct manufacturer name:$1 success" >>$SAVEFILE
	fi

	FRU_ID=0
	AREA_NUMBER=6
	FIELD_NUMBER=0
	READ_ELABEL_STRING=""

	read_elabel

	echo $READ_ELABEL_STRING > temp_tmp.txt
	if [ "$1" != "$READ_ELABEL_STRING" ] ; then
		echo_fail "Verify system manufacturer name:$1 fail!"
		echo "Verify system manufacturer name:$1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Verify system manufacturer name:$1 success"
		echo "Verify system manufacturer name:$1 success" >>$SAVEFILE
		return 0
	fi
}

#FRU扩展域内容长度35~128需多次接收拼接
check_long_fru_extra_description()
{
	local max_fru_string_len=128
	local max_string_once=34
	string2substring "$3" $max_fru_string_len
	TMP_STRING=$(remove_space "$TMP_STRING")
	elabel_rows=11
	local flag=0
	local i

	for((i=1;i<$elabel_rows;i++))
	do
		i_02x=`printf %02x $i`
		echo > $IPMI_CMD_RETURN_FILE
		local j
		for((j=0;j<4;j++))
		do
			local offset=`expr $j \* $max_string_once`
			local offset_02x=`printf %02x $offset`
			ipmicmd -k "f 0 30 90 05 $1 $2 $i_02x $offset_02x 22" smi 0 > temp.txt
			local tmp_string=`cat temp.txt|xargs echo`
			local sub_string=`echo ${tmp_string:18:120}`
			echo "$sub_string" >> $IPMI_CMD_RETURN_FILE
		done
		cat $IPMI_CMD_RETURN_FILE|xargs echo > temp.txt
		grep -wq "$TMP_STRING" temp.txt
		if [ $? = 0 ] ; then
			flag=1
			break
		fi
	done

	if [ $flag = 1 ] ; then
		echo_success "Check fru id $1 extend label $3 success"
		echo "Check fru id $1 extend label $3 success" >>$SAVEFILE
	else
		echo_fail "Check fru id $1 extend label $3 fail"
		echo "Check fru id $1 extend label $3 fail" >>$SAVEFILE
	fi
}

#FRU扩展域内容长度小于35字节可一次接收完毕
check_fru_extra_description()
{
	local max_fru_string_len=128
	string2substring "$3" $max_fru_string_len
	TMP_STRING=$(remove_space "$TMP_STRING")
	elabel_rows=10
	local flag=0
	local i
	for((i=0;i<$elabel_rows;i++))
	do
		i_02x=`printf %02x $i`
		run_cmd "f 0 30 90 05 $1 $2 $i_02x 00 ff" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo "read fru id $1 extend label row $i fail!" >> $SAVEFILE
		else
			cat $IPMI_CMD_RETURN_FILE |xargs echo > temp.txt
			grep -wq "$TMP_STRING" temp.txt
			if [ $? = 0 ] ; then
				flag=1
				break
			fi
	fi
	sleep 1
	done
	
	if [ $flag = 1 ] ; then
		echo_success "Check fru id $1 extend label $3 success"
		echo "Check fru id $1 extend label $2 success" >>$SAVEFILE
	else
		echo_fail "Check fru id $1 extend label $3 fail"
		echo "Check fru id $1 extend label $3 fail" >>$SAVEFILE
	fi
}

#校验FRU扩展域信息
check_fru_extend_info()
{
	fruid=$1
	WRITE_SPLIT_STRING="$3"
	READ_ELABEL_STRING="$3"
	if [[ "$READ_ELABEL_STRING" =~ ";" ]]; then
		tmp=1
		while true
		do
			split=`echo $READ_ELABEL_STRING | cut -d ";" -f$tmp`
			WRITE_SPLIT_STRING=$split
			if [ "$split" != "" ]; then
				local fru_len=`echo ${#WRITE_SPLIT_STRING}`
				if [ "$fru_len" -le 34 ] ; then
					check_fru_extra_description $fruid $2 "$WRITE_SPLIT_STRING"
				else
					check_long_fru_extra_description $fruid $2 "$WRITE_SPLIT_STRING"
				fi

				if [ $? != 0 ]; then
					echo_fail "Check fru $fruid extend label:$WRITE_SPLIT_STRING fail!"
					echo "Set fru $fruid extend label:$WRITE_SPLIT_STRING fail!" >>$SAVEFILE
					error_quit
				fi
				((tmp++))
			else
				break
			fi
		done				
	else
		local fru_len=`echo ${#3}`
		if [ "$fru_len" -le 34 ] ; then
			check_fru_extra_description $fruid $2 "$3"
		else
			check_long_fru_extra_description $fruid $2 "$3"
		fi
	fi
}

#校验FRU厂商信息
check_fru_manufacturer_info()
{
	FRU_ID=$1
	AREA_NUMBER=$2
	FIELD_NUMBER=$3
	max_str_len=`printf %2x $MAX_STRING_NUM`
	#读取24个字符
	run_cmd "f 0 30 90 05 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 00 $max_str_len" "0f 31 00 90 d3"
	if [ $? == 0 ]; then
		echo "FRU ID $FRU_ID not support elabel operation" >> $SAVEFILE			
	else
		READ_ELABEL_STRING=""

		read_elabel
		
		echo $READ_ELABEL_STRING > temp_tmp.txt
		if [ "$4" != "$READ_ELABEL_STRING" ] ; then
			echo_fail "Verify fru $FRU_ID manufacturer name:$4 fail!"
			echo "Verify fru $FRU_ID manufacturer name:$4 fail!" >>$SAVEFILE
			error_quit
		else
			echo_success "Verify fru $FRU_ID manufacturer name:$4 success"
			echo "Verify fru $FRU_ID manufacturer name:$4 success" >>$SAVEFILE
		fi
	fi			
}

check_fru_asset_info()
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
	
	#3 获取FRU 扩展域信息
	getini $CONFIGFILE BMCSet_CustomBoardExtendLabel
	BoardExtendLabel=$parameter
	
	#4 空定制化不执行后续校验操作(此定制化项仅在白牌需求里面会用到，因此正常装备场景不用执行)
	if [ "$BoardManufacturer" == "" ] && [ "$ProductManufacturer" == "" ] && [ "$BoardExtendLabel" == "" ] && [ "$Cooperation_BoardManufacturer" == "" ] && [ "$Cooperation_ProductManufacturer" == "" ] ; then
		return 0
	fi
	
	#5 获取所有的FRU ID信息
	read_fruid_set
	
	#6 遍历所有FRU，对每个FRU执行校验操作
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
			#校验FRU Board域厂商信息
			if [ "$BoardManufacturer" != "" ] || [ "$Cooperation_BoardManufacturer" != "" ]; then
				if [ $SET_BOARD_MANUFACTURER_FLAG -eq 1 ] ; then
					check_fru_manufacturer_info $fruid 02 01 "$Cooperation_BoardManufacturer"
				else
					check_fru_manufacturer_info $fruid 02 01 "$BoardManufacturer"
				fi
			fi
			
			#校验FRU Product域厂商信息
			if [ "$ProductManufacturer" != "" ] || [ "$Cooperation_ProductManufacturer" != "" ]; then
				if [ $SET_PRODUCT_MANUFACTURER_FLAG -eq 1 ] ; then
					check_fru_manufacturer_info $fruid 03 00 "$Cooperation_ProductManufacturer"
				else
					check_fru_manufacturer_info $fruid 03 00 "$ProductManufacturer"
				fi
			fi	

			#校验FRU Board扩展域信息
			if [ "$BoardExtendLabel" != "" ]; then
				check_fru_extend_info $fruid 05 "$BoardExtendLabel"
			fi
		fi
	done
}

main()
{
	chmod +x *
	#1.1、校验序列号  序列号不会被清除
	#X8000Chassis Part Number由rmc同步，BMC不进行定制化
	getini  $CONFIGFILE  BMCSet_CustomSerialNUM
	SYS_NUM=$parameter
	if [ "$SYS_NUM" = "on" ] ; then
		getini  $CONFIGFILE  Custom_SerialNUM
		SYS_NUM=$parameter
		if [ "$SYS_NUM" != "" ] ; then
			check_system_num
			sleep 1
		else
			echo_fail "SYS_NUM=NULL verify SYS_NUM fail" 
			echo "SYS_NUM=NULL verify SYS_NUM fail" >>$SAVEFILE
			error_quit
		fi
	else
		check_restore_system_num
	fi	

	#校验PartNumber
	getini  $CONFIGFILE  BMCSet_CustomBoardPartNumberFlag
	BOARD_PART_NUMBER_FLAG=$parameter
	if [ "$BOARD_PART_NUMBER_FLAG" = "on" ] ; then
		get_board_serial_number
		if [ "$BOARD_SERIAL_NUMBER" != "" ] ; then
			check_board_part_num
			sleep 1
		else
			echo_fail "BOARD_SERIAL_NUMBER=NULL" 
			echo "BOARD_SERIAL_NUMBER=NULL " >>$SAVEFILE
			error_quit		
		fi
	#else
		#check_restore_board_part_num
	fi
	
	#1.3、校验资产编号
	#X8000资产编号由rmc同步，BMC不进行定制化
	getini $CONFIGFILE  BMCSet_CustomAssetTag
	ASSET_NUM=$parameter
	if [ "$ASSET_NUM" = "on" ] ; then
		getini $CONFIGFILE Custom_AssetTag
		ASSET_NUM=$parameter
		if [ "$ASSET_NUM" != "" ] ; then
			check_asset_tag
			sleep 1
		else
			echo_fail "ASSET_NUM=NULL verify ASSET_NUM fail" 
			echo "ASSET_NUM=NULL verify ASSET_NUM fail" >>$SAVEFILE
			error_quit
		fi
	fi
	
	#1.4、校验机器名称
	getini $CONFIGFILE  BMCSet_MACHINENAME
	MACHINENAME=$parameter
	if [ "$MACHINENAME" != "" ] ; then
		check_machine_name "$MACHINENAME"
		sleep 1
	else
		getini $CONFIGFILE BMCSet_Cooperation_MACHINENAME
		Cooperation_MACHINENAME=$parameter
		if [ "$Cooperation_MACHINENAME" != "" ] ; then
			check_machine_name "$Cooperation_MACHINENAME"
			sleep 1
		else
			check_is_new_name
			#TaiShan空定制化，需要判断新老名称
			if [ $? = 1 ];then
				check_taishan_name
				sleep 1
			fi
		fi
	fi
	
	#校验机器别名
	check_machine_alias

	#1.5、校验chassis part number
	#X8000CustomChassisPartNumber由rmc同步，BMC不进行定制化
	getini $CONFIGFILE  BMCSet_CustomChassisPartNumber
	PARTNUMBER=$parameter
	if [ "$PARTNUMBER" = "on" ] ; then
		getini $CONFIGFILE Custom_ChassisPartNumber
		PARTNUMBER=$parameter
		if [ "$PARTNUMBER" != "" ] ; then
			check_set_chassis_part_number
			sleep 1
		else
			echo_fail "PARTNUMBER=NULL verify PARTNUMBER fail" 
			echo "PARTNUMBER=NULL verify PARTNUMBER fail" >>$SAVEFILE
			error_quit
		fi
	else
		check_restore_chassis_part_number  
	fi

	#1.6、校验locationid(设置未实现)
	
	#校验chassis type
	getini $CONFIGFILE  BMCSet_CustomChassisType
	PARTNUMBER=$parameter
	if [ "$PARTNUMBER" = "on" ] ; then
		getini $CONFIGFILE Custom_ChassisChassisType
		PARTNUMBER=$parameter
		if [ "$PARTNUMBER" != "" ] ; then
			check_set_chassis_type
			sleep 1
		else
			echo_fail "PARTNUMBER=NULL verify chassis type PARTNUMBER fail" 
			echo "PARTNUMBER=NULL verify chassis type PARTNUMBER fail" >>$SAVEFILE
			error_quit
		fi
	else
		check_restore_chassis_type  
	fi
	
	#1.8、校验第三方预置信息
	getini $CONFIGFILE BMCSet_3rdInfoEnable
	THIRDINFOENABLE=$parameter
	if [ "$THIRDINFOENABLE" = "on" ] ;then
		getini $CONFIGFILE  BMCSet_3rdInfo
		THIRDINFO=$parameter
		if [ "$THIRDINFO" != "" ] ; then
			check_set_3rd_info
			sleep 1
		fi	
	fi	
	
    #校验厂商信息
	getini $CONFIGFILE BMCSet_CustomManufacturer
	IS_MANUFACTURER=$parameter
	if [ "$IS_MANUFACTURER" = "on" ] ; then
		getini $CONFIGFILE Custom_Manufacturer
		MANUFACTURER=$parameter
		if [ "$MANUFACTURER" != "" ] ; then
			check_manufacturer_name "$MANUFACTURER"
			sleep 1
		else
			echo_fail "Custom_Manufacturer=NULL, verify Manufacturer fail" 
			echo "Custom_Manufacturer=NULL, verify Manufacturer fail" >>$SAVEFILE
			error_quit
		fi
	else
		getini $CONFIGFILE Custom_Cooperation_Manufacturer
		Cooperation_MANUFACTURER=$parameter
		if [ "$Cooperation_MANUFACTURER" != "" ] ; then
			check_manufacturer_name "$Cooperation_MANUFACTURER"
			sleep 1
		elif [ "$Cooperation_MANUFACTURER" == "" ] && ([ "$bp_flag" = "01" ] || [ "$bp_flag" = "11" ]) ; then
			echo_and_save_fail "Manufacturer is NULL, set manufacturer name $Cooperation_MANUFACTURER fail"
			error_quit
		else
			MANUFACTURER="Huawei"
			check_manufacturer_name "$MANUFACTURER"
		fi
	fi
	
	#校验主板定制化FRU Board扩展域信息
	getini $CONFIGFILE BMCSet_FRUDescription
	if [ "$parameter" != ""	]; then
		FRUDescription=$parameter
		check_fru_extend_info 00 05 "$FRUDescription"
	fi
	
	#校验主板定制化FRU Product扩展域信息
	getini $CONFIGFILE BMCSet_FRUProductExtraDescription
	if [ "$parameter" != ""	]; then
		FRUProductExtraDescription=$parameter
		check_fru_extend_info 00 09 "$FRUProductExtraDescription"
		sleep 1
	fi
	
	#校验除主板外的其他FRU白牌定制化信息
	check_fru_asset_info
}

main


