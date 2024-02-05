#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#禁用ME功耗封顶 Global_Domain 策略
disable_ME_Global_Domain()
{	
    run_cmd "f 0 30 93 DB 07 00 36 3e 0 1 10 0 0 ff ff 0 1 0 1 0" "0f 31 00 93 00 db 07 00 00 01 00 79 01 00"
    if [ $? == 0 ] ; then
        return 0
	fi 

	ME_ADDR=0x2c
	#X6000/X8000JDM单板与其他单板平台不同ME地址为0x88,此处做区分
	#if [ "$PRODUCT_ID" = "3" ] && [ $BOARD_ID = "04" ] ; then
	#	ME_ADDR=0x88
	#fi
	
	for((i=0;i<10;i++))
    do
        ipmitool -b 0x06 -t $ME_ADDR raw 0x2E 0xC0 0x57 0x01 0x00 0x00 0x00 0x01 >temp.txt
        sleep 1
        ipmitool -b 0x06 -t $ME_ADDR raw 0x2E 0xC0 0x57 0x01 0x00 0x00 0x00 0x01 >temp.txt
        sleep 1
        
        grep -wq "57 01 00" temp.txt
        if [ $? = 0 ] ; then
            echo_success "disable Global_Domain success"
            echo "disable Global_Domain success">>$SAVEFILE
            sleep 1
            return 0
        else
            rmmod ipmi_si
            sleep 1
            modprobe ipmi_si
            sleep 1
            echo "disable Global_Domain command retry times: $i"
        fi
    done

    echo_fail "disable Global_Domain fail"
    echo "disable Global_Domain fail">>$SAVEFILE
    error_quit
}

disable_imu_powercapping_policy()
{	
    run_cmd "f 0 30 93 DB 07 00 13 01 00 00 01 00" "0f 31 00 93 00 db 07 00"
    if [ $? == 0 ] ; then
        echo_success "disable powercapping success"
		echo "disable powercapping success">>$SAVEFILE
	else
		echo_fail "disable powercapping fail"
		echo "disable powercapping fail">>$SAVEFILE
		error_quit	
	fi
}


disable_syspower_psu_normal_and_redundancy()
{	
    run_cmd "f 0 30 93 DB 07 00 5b 04 00" "c1"
    if [ $? == 0 ] ; then
        echo_success "not support normal and redundancy"
		echo "not support normal and redundancy">>$SAVEFILE
		return 0;
	fi

    run_cmd "f 0 30 93 DB 07 00 5a 04 00 00" "0f 31 00 93 00 db 07 00"
    if [ $? == 0 ] ; then
        echo_success "disable normal and redundancy success"
		echo "disable normal and redundancy success">>$SAVEFILE
	else
		echo_fail "disable normal and redundancy fail"
		echo "disable normal and redundancy fail">>$SAVEFILE
		error_quit	
	fi
}

#设置上电恢复策略
set_power_policy()
{
    powerstat=99
    if [ "$1" = "poweroff" ] ; then
	    powerstat=0;	
    fi

    if [ "$1" = "poweron" ] ; then
	    powerstat=2;	
    fi
	
	if [ "$1" = "powerrestore" ] ; then
	    powerstat=1;	
    fi
	
	if [ $powerstat = 99 ] ; then	
		echo_fail "Set policy $1 fail!"
		echo "Set policy $1 fail!" >>$SAVEFILE
		error_quit
	fi
	
    run_cmd  "f 0 00 06 $powerstat" "0f 01 00 06 00"
    if [ $? != 0 ] ; then
		echo_fail "Set policy $1 fail!"
		echo "Set policy $1 fail!" >>$SAVEFILE
		error_quit
    fi

    if [ "$1" = "poweroff" ] ; then
        grep -wq "0f 01 00 06 00 01" $IPMI_CMD_RETURN_FILE
    fi

    if [ "$1" = "poweron" ] ; then
        grep -wq "0f 01 00 06 00 04" $IPMI_CMD_RETURN_FILE
    fi

    if [ "$1" = "powerrestore" ] ; then
        grep -wq "0f 01 00 06 00 02" $IPMI_CMD_RETURN_FILE
    fi

    if [ $? != 0 ] ; then
		echo_fail "Set policy $POWER_RESTOR fail!"
		echo "Set policy $POWER_RESTOR fail!" >>$SAVEFILE
		error_quit
    else
		echo_success "Set policy $POWER_RESTOR success!"
		echo "Set policy $POWER_RESTOR success!" >>$SAVEFILE
		return 0
    fi
}

#设置电源主备模式切换上下限
set_PS_Active_Standby_MAX_MIN_percentage ()
{
	max=`printf %2x $1`
	min=`printf %2x $2`
	
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
	
	set_data="$key_id $key_type $key_len $key_val $info1_id $info1_type $info1_len $info1_val $info2_id $info2_type $info2_len $info2_val"
	set_prop_cmd $fun_id 2 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "Set PS_Active_Standby fail"
		echo "Set PS_Active_Standby fail"  >>SAVEFILE
		error_quit
	else
		return 0
	fi
}

#设置漏液后的下电策略
set_power_policy_after_leak()
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
		echo_fail "Set power policy after leaking to $1 fail!"
		echo "Set power policy after leaking to $1 fail!" >>$SAVEFILE
		error_quit
	fi
	#设置漏液后下电策略
	run_cmd "f 0 30 93 db 07 00 5a 01 00 01 00 $pwrpolicy" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
		echo_fail "Set power policy after leaking to $1 fail!"
		echo "Set power policy after leaking to $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Set power policy after leaking to $1 success!"
		echo "Set power policy after leaking to $1 success!" >>$SAVEFILE
		return 0
	fi
}
set_power_info_mode ()
{
	mode=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 35 10 00 01 00 00 00 FF FF 01 00 11 00 00 01 ${mode}" "0f 31 00 93 00 db 07 00"  
	if [ $? == 0 ] ; then
		echo_success "Set power info mode $mode success!"
		echo "Set power info mode $mode success!" >>$SAVEFILE
        return 0
	else
		echo_fail "Set power info mode $mode failed!"
		echo "Set power info mode $mode failed!" >>$SAVEFILE
		error_quit
	fi 
}

set_power_unit_group_start_id ()
{
	start_id=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 35 10 00 01 00 00 00 FF FF 01 00 12 00 00 01 ${start_id}" "0f 31 00 93 00 db 07 00"  
	if [ $? == 0 ] ; then
		echo_success "Set power unit group start with $start_id success!"
		echo "Set power unit group start with $start_id success!" >>$SAVEFILE
        return 0
	else
		echo_fail "Set power unit group start with $start_id failed!"
		echo "Set power unit group start with $start_id failed!" >>$SAVEFILE
		error_quit
	fi 
}

set_psu_fru_product_version_config ()
{
	option=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 5a 1d 00 02 00 00 ${option}" "0f 31 00 93 00 db 07 00"  
	if [ $? == 0 ] ; then
		echo_and_save_success "Set PSU FRU Product Version configuration to $option success!"
		return 0
	else
		echo_and_save_fail "Set PSU FRU Product Version configuration to $option failed!"
		error_quit
	fi 
}

set_power_on_delay()
{
	# 判断设置的延时时间是否为数字
	is_digit $2
	if [ $? != 0 ] ; then
		echo_and_save_fail "Set power on delay time($2) seconds is failed! delay time value(0~120)"
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
		echo_and_save_fail "Set power on delay mode($1) is failed!"
		error_quit
	fi

	#延时上电的时间范围为0-120秒
	if [ "$2" = "" ] || [ "$2" -gt "1200" ] || [ "$2" -lt "0" ]; then
		echo_and_save_fail "Set power on delay time($delay_seconds) seconds is failed! delay time value(0~120)"
		error_quit
	fi

	run_cmd  "f 0 00 08 65 db 07 00 $mode $delay_time" "0f 01 00 08 00"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Set power on delay mode to $1,Set power up delay seconds to $delay_seconds fail!"
		error_quit
	else
		echo_and_save_success "Set power on delay mode to $1,Set power up delay seconds to $delay_seconds success!"
		return 0
	fi
}

main()
{
	chmod +x *
	local is_x86=0
	#3.1、设置电源恢复策略
	#TCE不支持定制化
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE  BMCSet_POWERRESTOR
		POWER_RESTOR=$parameter
		PRODUCT_NAME_ID=$PRODUCT_ID$BOARD_ID
		if [ "$POWER_RESTOR" != "" ] ; then
			set_power_policy $POWER_RESTOR
			sleep 1
		else
		#默认值为恢复到上电状态
			set_power_policy "poweron"
		fi
	fi
	#6.1、设置功耗封顶使能
	getini $CONFIGFILE  BMCSet_CustomPowerCappingStatus
	POWER_CAPPING=$parameter
	if [ "$POWER_CAPPING" = "on" ] ; then
		sleep 1
	else
	    #禁用Global_Domain
		if ([ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && ([ $BOARD_ID = $BOARD_G2500_ID ] || [ $BOARD_ID = $BOARD_ATLAS880_ID ])) || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ]; then
			sleep 1
		elif [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_A6_ID" ] || [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B0_ID" ] || 
			[ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B1_ID" ] || [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B2_ID" ] || 
			[ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B3_ID" ] || [ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B4_ID" ] ||
			[ "$PRODUCT_NAME_ID" = "$PRODUCT_XA320_B5_ID" ] ; then
			sleep 1
		else
			check_is_x86
			is_x86=$?
			#X86是ME,ARM是IMU
			if [ $is_x86 = 1 ]; then
				disable_ME_Global_Domain
			else
				disable_imu_powercapping_policy
			fi
		fi
	fi
	
	##电源主备状态切换条件定制化##
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
			
			if [ $MIN -ge 0 -a $MAX -ge $MIN -a $MAX -le 100 ] ;then
				echo "set PSActiveStandbyLevel MAX:[$MAX] MIN:[$MIN]"
				echo "set PSActiveStandbyLevel MAX:[$MAX] MIN:[$MIN]" >>$SAVEFILE
			else
				echo_fail "Invalid PSActiveStandbyLevel MAX:[$MAX] MIN:[$MIN]"
				echo "Invalid PSActiveStandbyLevel MAX:[$MAX] MIN:[$MIN]"  >>$SAVEFILE
				error_quit
			fi
		else
			#默认值
			MAX=75
			MIN=65
			echo "restore PSActiveStandbyLevel MAX:[$MAX] MIN:[$MIN]"
			echo "restore PSActiveStandbyLevel MAX:[$MAX] MIN:[$MIN]" >>$SAVEFILE
		fi
		#首先清空切换条件
		set_PS_Active_Standby_MAX_MIN_percentage 100 0
		#设置定制的切换条件
		set_PS_Active_Standby_MAX_MIN_percentage $MAX $MIN
		echo_success "Set PS_Active_Standby:  MAX:[$MAX] MIN:[$MIN] success"
		echo "Set PS_Active_Standby:  MAX:[$MAX] MIN:[$MIN] success"  >>$SAVEFILE	
	fi
	
	#设置液冷服务器漏液后的电源控制策略
	getini $CONFIGFILE  BMCSet_Leakage_Policy
	LEAK_POLICY=$parameter
	if [ "$LEAK_POLICY" == "poweroff" ] ; then
		set_power_policy_after_leak $LEAK_POLICY
		sleep 1
	else
		#默认值为漏液后无动作
		set_power_policy_after_leak "donothing"
	fi
	disable_syspower_psu_normal_and_redundancy

	# 定制化电源power unit group起始id值
	getini $CONFIGFILE  BMCSet_CustomPowerUnitGroupStartID
	STARTID=$parameter
	if [ "$STARTID" = "" ] ; then
		set_power_unit_group_start_id 0
		sleep 1
	elif [ "$STARTID" -ge "0" ] && [ "$STARTID" -lt "256" ] ; then
		set_power_unit_group_start_id $STARTID
		sleep 1
	else
		echo_and_save_fail "Set power unit group start id to $STARTID failed"
		error_quit
	fi
	
	# 设置电源信息模式
	getini $CONFIGFILE  BMCSet_CustomPowerInfoMode
	POWER_INFO_MODE=$parameter
	if [ "$POWER_INFO_MODE" = "0" ] || [ "$POWER_INFO_MODE" = "1" ] ; then
		set_power_info_mode $POWER_INFO_MODE
		sleep 1
	elif [ "$POWER_INFO_MODE" = "" ] ; then
		set_power_info_mode 0
		sleep 1
	else
		echo_and_save_fail "Set power info mode to $POWER_INFO_MODE failed"
		error_quit
	fi

	# 设置电源FUR Product Veriosn 显示模式
	getini $CONFIGFILE  BMCSet_CustomPsuFruProductVersion
	VERSION_OPTION=$parameter
	if [ "$VERSION_OPTION" = "" ] ; then
		set_psu_fru_product_version_config 0
		sleep 1
	elif [ "$VERSION_OPTION" -ge "0" ] && [ "$VERSION_OPTION" -lt "256" ] ; then
		set_psu_fru_product_version_config $VERSION_OPTION
		sleep 1
	else
		echo_and_save_fail "Set PSU FRU Product Version configuration to $VERSION_OPTION failed"
		error_quit
	fi

	# 设置上电延时模式和延时时间；延时上电时间，单位100ms
	getini $CONFIGFILE  BMCSet_PowerRestoreDelayMode
	PowerRestoreDelayMode=$parameter
	getini $CONFIGFILE  BMCSet_PowerRestoreDelayTime
	PowerRestoreDelayTime=$parameter
	if [ "$PowerRestoreDelayMode" = "" ] ; then
		set_power_on_delay DefaultDelay 0
	else
		set_power_on_delay $PowerRestoreDelayMode $PowerRestoreDelayTime
	fi
}

main
