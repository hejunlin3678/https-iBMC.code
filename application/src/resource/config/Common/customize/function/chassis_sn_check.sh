#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#定制化机箱SN同步到主板比较耗时，如果提前校验SN可能未能同步完成，等待其他校验操完后再校验
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
#配置是否定制资产标签后缀
check_assetflag_sync_state()
{
	fun_id=19
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 7) 
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $1`
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 7 "$exp_data"
        if [ $? != 0 ] ; then
		echo_fail "check_assetflag_sync_state $1 Fail"
		echo "check_assetflag_sync_state $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_assetflag_sync_state $1 success!"
		echo "check_assetflag_sync_state $1 success!" >>$SAVEFILE
	fi
}
#校验电子标签后缀
check_serialnum_suffix()
{
	suffix=$1
	is_have_space "$suffix"
	if [ $? == 0 ] ; then
		echo_and_save_fail "The serialnum suffix($1) contains spaces"
		error_quit
	fi
	string2substring "$suffix"
	suffix_str="$TMP_STRING"
	strlen=`echo ${#suffix}`
	fun_id=19
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 8) 
	info_type=73 #s
	info_len=`printf "%02x" $strlen`
	info_val=$suffix_str
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 8 "$exp_data"
        if [ $? != 0 ] ; then
		echo_fail "check_serialnum_suffix $1 Fail"
		echo "check_serialnum_suffix $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_serialnum_suffix $1 success!"
		echo "check_serialnum_suffix $1 success!" >>$SAVEFILE
	fi
}

#BEGIN: 问题单 DTS2015111209712, 2015-11-12 by l00205093
#【定制化】增加 X 系列服务器SN、后缀 校验检查功能，增强功能。
#获取X6000/X6800的MM板product.SN
get_mm_sn()
{
	local fun_id=$(word2string 56)
	local key_id=$(word2string 0xffff) 
	local key_type=79
	local key_len="01"
	local key_val="01"
	local key_id_string="$key_id $key_type $key_len $key_val"
	local info_id=$(word2string 34)
	local end_flag=0
	local offset=0
	local max_len=`printf "%02x" $MAX_FUN_STRING_LEN`
	GET_PROP_INF_STRING=""
	
	frame_count=0
		
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
		if [ 0 = $frame_count ]; then
			GET_PROP_INF_STRING=$GET_PROP_INF_STRING" ${tmp_arr[@]:9}"
		else
			GET_PROP_INF_STRING=$GET_PROP_INF_STRING" ${tmp_arr[@]:13}"
		fi
		
		frame_count=99
		
		if [ "$end_flag" = "01" ] ; then
			sleep 1
			continue
		else					
			break
		fi
	done
	
	echo $GET_PROP_INF_STRING > tmp_sn.txt
	hextochar tmp_sn.txt 12 temp_tmp.txt
	
	MM_SN=`cat temp_tmp.txt`
	echo $MM_SN
}

#获取X6000/X6800的单板后缀
get_blade_suffix()
{
	get_blade_num
	echo $BLADNUM

	getini $CONFIGFILE  Custom_SerialnumSuffixBySlot$BLADNUM
	SERIALNUM_SUFFIX_TMP=$parameter
	if [ "$SERIALNUM_SUFFIX_TMP" != "" ] ; then
		SLOT_SUFFIX=$parameter
	else
		getini $CONFIGFILE  BMCSet_CustomSerialnumSuffix
		SERIALNUM_SUFFIX=$parameter
		if [ "$SERIALNUM_SUFFIX" = "on" ] ; then
		    SLOT_SUFFIX=$SLOT_SUFFIX
		else
		    SLOT_SUFFIX=-$BLADNUM #XML中默认值
		fi
	fi

	echo $SLOT_SUFFIX
}

# 检查X6000/X6800的单板后缀
check_blade_sn()
{
	get_mm_sn
	
	#检查MM.SN, 如果为空, 报错。从X系列看，一定会进行MM板序列号的同步
	if [ "$MM_SN" = "" ]; then
		echo_fail "check_blade_sn: MM_SN is NULL!"
		echo "check_blade_sn: MM_SN is NULL!" >>$SAVEFILE
		error_quit
	fi
	
	# 获取单板后缀
	get_blade_suffix
	
	# “目标SN”= “MM810.SN”+“后缀”
	TARGET_SN="$MM_SN""$SLOT_SUFFIX"
	
	#读取单板SN
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=4
	READ_ELABEL_STRING=""

	read_elabel
	BLADE_SN=$READ_ELABEL_STRING   
	
	# 检查单板SN, 当check脚本运行时，SN有概率未同步完成，增加等待重试机制
	chk_retry=0
	while [ "$BLADE_SN" != "$TARGET_SN" ]; do
		chk_retry=$(($chk_retry+1)) 
		echo "check_blade_sn: $BLADE_SN != $TARGET_SN $chk_retry"
		if [ $chk_retry -gt 20 ]; then
			echo_fail "check_blade_sn: $BLADE_SN != $TARGET_SN"
			error_quit
		fi
		sleep 6
		read_elabel
		BLADE_SN=$READ_ELABEL_STRING
	done
	
	echo_success "check_blade_sn success!"
	echo "check_blade_sn success!" >>$SAVEFILE
}
#END: 问题单 DTS2015111209712, 2015-11-12 by l00205093
main()
{
	chmod +x *
	
    PRODUCT_NAME_ID=$PRODUCT_ID$BOARD_ID
	if [ $PRODUCT_ID == $PRODUCT_XSERIALS_TYPE ] || [ $PRODUCT_ID = $PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE ] || ([ $PRODUCT_ID == $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID != $BOARD_ATLAS880_ID ] && [ $BOARD_ID != $BOARD_ATLAS800_9010_ID ] && [ $BOARD_ID != $BOARD_G2500_ID ] && [ $BOARD_ID != $BOARD_ATLAS500_ID ]); then	
		echo " only for X-Serials or Atlas Custom"
		
		#设置资产标签定制化标志 on开启定制化,off表示同步MMC/MM电子标签,X6000 V2默认同步,X6800默认不同步, X6000 V3默认不同步，X6000 V6默认不同步
		getini $CONFIGFILE  BMCSet_CustomAssetTagFlag
		ASSET_TAG_FLAG=$parameter
		#设置产品序列号同步标志 on不同步,off表示同步MMC/MM电子标签，X6000 V2默认同步,X6800默认不同步, X6000 V3默认不同步，X6000 V6默认不同步
		getini $CONFIGFILE  BMCSet_CustomSerialNUMFlag
		PRODUCT_SERIAL_FLAG=$parameter
		
		declare -i product_assettag_flag;
		declare -i product_sn_flag;
		declare -i elabel_sync_flag;
		
        #使用产品+单板Id唯一标识
		if [ $PRODUCT_NAME_ID != $PRODUCT_XH628_XH622_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH620_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321_HIGH_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321_STANDARD ] && \
			[ $PRODUCT_NAME_ID != $PRODUCT_XH321V5_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH321V5L_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_G560_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_G560V5_ID ] && \
			[ $PRODUCT_NAME_ID != $PRODUCT_G530V5 ] && [ $PRODUCT_NAME_ID != $PRODUCT_XH628V5_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XA320_A6_ID ] && [ $PRODUCT_NAME_ID != $PRODUCT_XA320_B0_ID ] && \
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
				product_sn_flag=2;
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
		check_assetflag_sync_state $elabel_sync_flag
		sleep 1

		#定制化标签后缀	
		getini $CONFIGFILE  BMCSet_CustomSerialnumSuffix
		SERIALNUM_SUFFIX=$parameter
		if [ "$SERIALNUM_SUFFIX" = "on" ] ; then
			get_max_blade_count
			for((I=0; I<$MAX_BLADE_COUNT; ++I))
			do
				ID=`expr $I + 1`
				getini $CONFIGFILE  Custom_SerialnumSuffixBySlot$ID
				SERIALNUM_SUFFIX_BY_SLOT=${SERIALNUM_SUFFIX_BY_SLOT}$parameter
			done
			echo $SERIALNUM_SUFFIX_BY_SLOT
			check_serialnum_suffix "$SERIALNUM_SUFFIX_BY_SLOT"
		fi
		
		#BEGIN: 问题单 DTS2015111209712, 2015-11-12 by l00205093
		#【定制化】增加 X 系列服务器SN、后缀 校验检查功能，增强功能。
		#检查单板SN、后缀
		    #BEGIN: 问题单 DTS2016052403484 , 2016-6-2 by wx350283	
			# 如果 X系列有打开SN的同步，则需需要检验
			if [ $product_sn_flag = 2 ]; then
				check_blade_sn
			fi
			#END: 问题单 DTS2016052403484 , 2015-6-2 by wx350283
		#END: 问题单 DTS2015111209712, 2015-11-12 by l00205093
	fi	
}

main