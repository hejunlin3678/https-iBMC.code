#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#检测上电恢复策略
check_set_power_policy()
{
    if [ "$POWER_RESTOR" = "poweroff" ] ; then
		temp=01
	elif [ "$POWER_RESTOR" = "poweron" ] ; then
		temp=04
	elif [ "$POWER_RESTOR" = "powerrestore" ] ; then
		temp=02
	else
		#如果全部都不是,则报错,默认不配置情况在调用的时候对参数进行赋值'poweron'
		echo_fail "verify policy $POWER_RESTOR fail!"
		echo "verify policy $POWER_RESTOR fail!" >>$SAVEFILE
		error_quit
    fi

    run_cmd  "f 0 00 06 03" "0f 01 00 06 00"
    if [ $? != 0 ] ; then
       echo_fail "Get policy $POWER_RESTOR fail!"
       echo "Get policy $POWER_RESTOR fail!" >>$SAVEFILE
       error_quit
    fi
	
	temp=`printf %02x $temp`
	grep -wq "0f 01 00 06 00 $temp"  $IPMI_CMD_RETURN_FILE

    if [ $? != 0 ] ; then
        echo_fail "verify policy $POWER_RESTOR fail!"
        echo "verify policy $POWER_RESTOR fail!" >>$SAVEFILE
        error_quit
    else
        echo_success "verify policy $POWER_RESTOR success!"
        echo "verify policy $POWER_RESTOR success!" >>$SAVEFILE
        return 0
    fi
}

#校验电源主备状态切换条件
check_PS_Active_Standby_MAX_MIN_percentage ()
{
	max=`printf %02x $1`
	min=`printf %02x $2`
	fun_id=16
	key_id="ff ff"
	key_type=79
	key_len=01
	key_val=01
	info1_id=$(word2string 11) 
	info1_type=79 #y
	info1_len=01
	info1_val=$max
	info2_id=$(word2string 12) 
	info2_type=79 #y
	info2_len=01
	info2_val=$min
	
	key_string="$key_id $key_type $key_len $key_val"
	exp1_data="$info1_id $info1_type $info1_len $info1_val"
	exp2_data="$info2_id $info2_type $info2_len $info2_val"
	get_prop_cmd $fun_id "$key_string" 11 "$exp1_data"
	if [ $? != 0 ] ; then
		echo_fail "Check PS_Active_Standby MAX fail"
		echo "Check PS_Active_Standby MAX fail"  >>SAVEFILE
		error_quit
	fi
	
	get_prop_cmd $fun_id "$key_string" 12 "$exp2_data"
	if [ $? != 0 ] ; then
		echo_fail "Check PS_Active_Standby MIN fail"
		echo "Check PS_Active_Standby MIN fail"  >>SAVEFILE
		error_quit
	fi
	
	echo "Check PS_Active_Standby:  MAX:[$max] MIN:[$min] success"  >>SAVEFILE	
	return 0
}

check_disable_imu_powercapping_policy()
{	
    run_cmd "f 0 30 93 DB 07 00 14 01 00 01" "0f 31 00 93 00 db 07 00 00 00"
    if [ $? == 0 ] ; then
        echo_success "check disable powercapping success"
		echo "check disable powercapping success">>$SAVEFILE
	else
		echo_fail "check disable powercapping fail"
		echo "check disable powercapping fail">>$SAVEFILE
		error_quit
	fi
}	

check_psu_fru_product_version_config()
{
	option=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 5b 1d 00 01 00 00" "0f 31 00 93 00 db 07 00 $option"  
	if [ $? == 0 ] ; then
		echo_and_save_success "Check PSU FRU Product Version configuration with $option success!"
		return 0
	else
		echo_and_save_fail "Check PSU FRU Product Version configuration with $option failed"
		error_quit
	fi 
}

#检查漏液后的下电策略
check_power_policy_after_leak()
{
	#判断是否液冷服务器
	run_cmd "f 0 30 93 db 07 00 36 5a 00 01 FF 00 00 ff ff 00 01 01 04 00" "0f 31 00 93 00 db 07 00 00 04 00 79 01 01"
	if [ $? != 0 ] ; then
		echo_success "Not liquid cooling mode"
		echo "Not liquid cooling mode">>$SAVEFILE
		return 0
	fi
	pwrpolicy=0
    if [ "$1" = "donothing" ] ; then
	    pwrpolicy=0;	
	elif [ "$1" = "poweroff" ]; then
		pwrpolicy=1;
    else
		echo_fail "Power policy after leaking $1 is illegal!"
		echo "Power policy after leaking $1 is illegal!" >>$SAVEFILE
		error_quit
	fi
	pwrpolicy=`printf %02x $pwrpolicy`
	#获取漏液后下电策略
	run_cmd "f 0 30 93 db 07 00 5b 01 00 01 00 00" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
		echo_fail "verify power policy after leaking $1 fail!"
		echo "verify power policy after leaking $1 fail!" >>$SAVEFILE
		error_quit
	else
		grep -wq "0f 31 00 93 00 db 07 00 $pwrpolicy"  $IPMI_CMD_RETURN_FILE
		if [ $? != 0 ] ; then
			echo_fail "verify power policy after leaking $pwrpolicy fail!"
			echo "verify power policy after leaking $pwrpolicy fail!" >>$SAVEFILE
			error_quit
		else
			echo_success "verify power policy after leaking $pwrpolicy success!"
			echo "verify power policy after leaking $pwrpolicy success!" >>$SAVEFILE
			return 0
		fi
	fi
}

check_power_info_mode ()
{
	mode=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 36 10 00 01 ff 00 00 FF FF 00 01 00 11 00" "0f 31 00 93 00 db 07 00 00 11 00 79 01 ${mode}"  
	if [ $? == 0 ] ; then
		echo_and_save_success "Check power info mode $mode success!"
        return 0
	else
		echo_and_save_fail "Check power info mode $mode failed!"
		error_quit
	fi 
}

check_power_unit_group_start_id ()
{
	start_id=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 36 10 00 01 ff 00 00 FF FF 00 01 00 12 00" "0f 31 00 93 00 db 07 00 00 12 00 79 01 ${start_id}"  
	if [ $? == 0 ] ; then
		echo_and_save_success "Check power unit group start with $start_id success!"
        return 0
	else
		echo_and_save_fail "Check power unit group start with $start_id failed!"
		error_quit
	fi 
}

check_power_on_delay()
{
	# 判断设置的延时时间是否为数字
	is_digit $2
	if [ $? != 0 ] ; then
		echo_and_save_fail "Check power on delay time($2) seconds is failed! delay time value(0~120)"
		error_quit
	fi

	delay_time=$(dword2string $2)
	delay_seconds=$(echo "scale=2;$2/10"|bc)

	# 默认模式下设置的时间无效
	if [ "$1" = "DefaultDelay" ] ; then
		mode=00
	elif [ "$1" = "HalfDelay" ] ; then
		mode=01
	elif [ "$1" = "FixedDelay" ] ; then
		mode=02
	elif [ "$1" = "RandomDelay" ] ; then
		mode=03
	else
		echo_and_save_fail "Check power on delay mode($1) is failed!"
		error_quit
	fi

	#延时上电的时间范围为0-120秒
	if [ "$2" = "" ] || [ "$2" -gt "1200" ] || [ "$2" -lt "0" ]; then
		echo_and_save_fail "Check power on delay time($delay_seconds) seconds is failed! delay time value(0~120)"
		error_quit
	fi

	run_cmd "f 0 00 09 65 00 00" "0f 01 00 09 00 01 65 $(get_custom_manu_id) $mode $delay_time"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Check power on delay mode to $1,Set power up delay seconds to $delay_seconds fail!"
		error_quit
	else
		echo_and_save_success "Check power on delay mode to $1,Set power up delay seconds to $delay_seconds success!"
		return 0
	fi
}

main()
{
	chmod +x *
	local is_x86=0
	#3.1、校验电源恢复策略
	#TCE不支持定制化，因此此处不需要check
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE  BMCSet_POWERRESTOR
		POWER_RESTOR=$parameter
		PRODUCT_NAME_ID=$PRODUCT_ID$BOARD_ID
		if [ "$POWER_RESTOR" != "" ] ; then
			check_set_power_policy
			sleep 1
		else
			POWER_RESTOR=poweron
			check_set_power_policy	
			sleep 1
		fi
		echo_success "power_check success!"	
	fi	
	#6.1、校验功耗封顶使能
	getini $CONFIGFILE  BMCSet_CustomPowerCappingStatus
	POWER_CAPPING=$parameter
	if [ "$POWER_CAPPING" = "" ] ; then
		check_is_x86
		is_x86=$?
		#ARM是通过BMC控制的需要确认BMC上默认关闭
		if [ $is_x86 != 1 ] && [ $BOARD_ID != $BOARD_ATLAS880_ID ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ]; then
			if [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_A6_ID" ] || [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B0_ID" ] || 
				[ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B1_ID" ] || [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B2_ID" ] || 
				[ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B3_ID" ] || [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B4_ID" ] ||
				[ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B5_ID" ] ; then
				sleep 1
			else 
				check_disable_imu_powercapping_policy
			fi
		fi
	fi
	
	#校验电源主备状态切换条件
	if [ $PRODUCT_ID = $PRODUCT_RACK_TYPE ] || [ $PRODUCT_ID = $PRODUCT_RACK_TYPE_AJ_CUSTOMIZE ] || [ $PRODUCT_ID = $PRODUCT_TAISHAN_TYPE ] || [ $PRODUCT_ID = $PRODUCT_KUNPENG_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TAISHAN_1711_TYPE ]; then
		getini $CONFIGFILE BMCSet_CustomPSActiveStandby
		STATE=$parameter
		if [ "$STATE" = "on" ] ;then
			getini $CONFIGFILE Custom_PSActiveStandbyMaxLevel
			MAX=$parameter
			getini $CONFIGFILE Custom_PSActiveStandbyMinLevel
			MIN=$parameter
			if [ "$MAX" == "" ] ;then
				MAX=75
			fi
		
			if [ "$MIN" == "" ] ;then
				MIN=65
			fi
		else
			MAX=75
			MIN=65
		fi
		check_PS_Active_Standby_MAX_MIN_percentage $MAX $MIN
		echo_success "Check PS_Active_Standby: MAX=$MAX MIN=$MIN success"
		echo "Check PS_Active_Standby: MAX=$MAX MIN=$MIN success"  >>$SAVEFILE
	fi
	
	#检查液冷服务器漏液后的电源控制策略
	getini $CONFIGFILE  BMCSet_Leakage_Policy
	LEAK_POLICY=$parameter
	if [ "$LEAK_POLICY" != "" ] ; then
		check_power_policy_after_leak $LEAK_POLICY
		sleep 1
	else
		#默认值为漏液后无动作
		check_power_policy_after_leak "donothing"
	fi
	echo_success "Check power policy after leakage success!"

	# 检查电源FUR Product Veriosn 显示模式
	getini $CONFIGFILE  BMCSet_CustomPsuFruProductVersion
	VERSION_OPTION=$parameter
	if [ "$VERSION_OPTION" = "" ] ; then
		check_psu_fru_product_version_config 0
		sleep 1
	elif [ "$VERSION_OPTION" -ge "0" ] && [ "$VERSION_OPTION" -lt "256" ] ; then
		check_psu_fru_product_version_config $VERSION_OPTION
		sleep 1
	else
		echo_and_save_fail "Check PSU FRU Product Version configuration with $VERSION_OPTION failed"
		error_quit
	fi

	# 定制化电源power unit group起始id值
	getini $CONFIGFILE  BMCSet_CustomPowerUnitGroupStartID
	STARTID=$parameter
	if [ "$STARTID" = "" ] ; then
		check_power_unit_group_start_id 0
		sleep 1
	elif [ "$STARTID" -ge "0" ] && [ "$STARTID" -lt "256" ] ; then
		check_power_unit_group_start_id $STARTID
		sleep 1
	else
		echo_and_save_fail "Check power unit group start id failed"
		error_quit
	fi
	
	# 电源信息模式
	getini $CONFIGFILE  BMCSet_CustomPowerInfoMode
	POWER_INFO_MODE=$parameter
	if [ "$POWER_INFO_MODE" = "1" ] || [ "$POWER_INFO_MODE" = "0" ] ; then
		check_power_info_mode $POWER_INFO_MODE
		sleep 1
	elif [ "$POWER_INFO_MODE" = "" ] ; then
		check_power_info_mode 0
		sleep 1
	else
		echo_and_save_fail "Check power info mode $POWER_INFO_MODE failed"
		error_quit
	fi

	# 校验上电延时模式和延时时间；延时上电时间，单位100ms
	getini $CONFIGFILE  BMCSet_PowerRestoreDelayMode
	PowerRestoreDelayMode=$parameter
	getini $CONFIGFILE  BMCSet_PowerRestoreDelayTime
	PowerRestoreDelayTime=$parameter
	if [ "$PowerRestoreDelayMode" = "" ]; then
		check_power_on_delay DefaultDelay 0
	else
		check_power_on_delay $PowerRestoreDelayMode $PowerRestoreDelayTime
	fi
}

main

