#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi


#校验时区
check_bmc_timezone()
{
	#先判断是否是数字
	expr $1 "+" 10 &> /dev/null
	if [ $? -eq 0 ]; then
		#如果是数字
		if [ $1 -ge 0 ] ; then
			timeoffset=$1
			flag=1
		else
			timeoffset=$((0 - $1))
			flag=2
		fi

		hour=$(expr $timeoffset / 60)
		min=$(expr $timeoffset % 60)

		hour=`printf "%02d" $hour`
		min=`printf "%02d" $min`

		if [ $flag -eq 1 ] ; then
			if [ $hour -eq 0 ] && [ $min -eq 0 ] ; then
				timezone="UTC"
			else
				timezone="UTC+"${hour}":"${min}
			fi
		elif [ $flag -eq 2 ] ; then
			timezone="UTC-"${hour}":"${min}
		else
			echo "Input timezone is wrong"
		fi
	else
		#如果不是数字
		timezone=$1
	fi

	strlen=`echo ${#timezone}`
	timezone=$(string2hex "$timezone")
	
	fun_id=13
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 9) 
	info_type=73
	info_len=`printf "%02x" $strlen` 
	info_val=$timezone
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 9 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check timezone fail"
		echo "Check timezone fail" >> $SAVEFILE
		error_quit		
   	else
		echo_success "Check timezone success"
		echo "Check timezone success" >>$SAVEFILE
		return 0
   	fi
}

#校验blackbox是否开启
check_set_blackbox_on()
{
    run_cmd "f 0 30 93 DB 07 00 36 3e 0 1 10 0 0 ff ff 0 1 0 2 0" "0f 31 00 93 00 db 07 00 00 02 00 79 01 00"
    if [ $? == 0 ] ; then
        return 0
	fi 

    run_cmd "f 0 30 93 DB 07 00 10 0C 01 00 00 01" "0f 31 00 93 00 db 07 00 00 01"
    
	if [ $? != 0 ] ; then
		echo_fail "verify blackbox on fail!"
		echo "verify blackbox on fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "verify blackbox on success!"
		echo "verify blackbox on success!" >>$SAVEFILE
		return 0
	fi

}

#校验blackbox是否开启
check_set_blackbox_off()
{
    run_cmd "f 0 30 93 DB 07 00 36 3e 0 1 10 0 0 ff ff 0 1 0 2 0" "0f 31 00 93 00 db 07 00 00 02 00 79 01 00"
    if [ $? == 0 ] ; then
        return 0
	fi 	
	run_cmd "f 0 30 93 DB 07 00 10 0C 01 00 00 01" "0f 31 00 93 00 db 07 00 00 00"
	
	if [ $? != 0 ] ; then
	    echo_fail "verify blackbox off fail!"
	    echo "verify blackbox off fail!" >>$SAVEFILE
	    error_quit
	else
	    echo_success "verify blackbox off success!"
	    echo "verify blackbox off success!" >>$SAVEFILE
	    return 0
	fi

}

#校验trap模式
check_set_trap_mode()
{
    TrapMode=`printf "%02x" $1`
    run_cmd "f 0 30 93 db 07 00 38 43 00 01 ff 00 00 03 00"  "0f 31 00 93 00 db 07 00 00 03 00 79 01 $TrapMode"
    if [ $? = 0 ] ; then
        echo_success "Check trap mode:$TrapMode success"
        echo "Check trap mode:$TrapMode success">>$SAVEFILE
        sleep 1
    else
        echo_fail "Check trap mode:$TrapMode fail"
        echo "Check trap mode:$TrapMode fail">>$SAVEFILE
        error_quit
    fi
}

#校验trap模式定制
check_trap_mode_custom()
{
	local TrapModeCustom=$1
	if [ "$TrapModeCustom" != 0 ] && [ "$TrapModeCustom" != 1 ] ; then
		echo_and_save_fail "Invalid trap mode custom type($TrapModeCustom)"
		error_quit
	fi
	local value=`printf "%02x" $TrapModeCustom`
	run_cmd "f 0 30 93 DB 07 00 16 00 16 00 00 01" "0f 31 00 93 00 db 07 00 $value"

	if [ $? != 0 ] ; then
		echo_and_save_fail "Check trap mode custom $TrapModeCustom fail!"
		error_quit
	else
		echo_and_save_success "Check trap mode custom $TrapModeCustom success!"
		return 0
	fi
}

#校验trap的IP地址
check_set_bmc_trap_dest_ipaddr()
{
    if [ "$2" != "" ] ; then
		ipaddr=$2	
		string2substring "$ipaddr"
		loadip="$TMP_STRING"
		#整个字符串长度
		strlen=`echo ${#ipaddr}`
	else
		#删除trapid
		loadip="" 
		strlen=0
	fi

	show_id=`expr $1 + 1`	
	fun_id=4
	key_id=$(word2string 1) 
	key_type=79
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 3) 
	info_type=73
	info_val=$loadip
	info_len=`printf "%02x" $strlen`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "verify trap $show_id dest ip $2 fail!"
		echo "verify trap $show_id dest ip $2 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "verify trap $show_id dest ip $2 success!"
		return 0
	fi
}
#校验trap的端口号
check_set_bmc_trap_dest_port()
{	
	show_id=`expr $1 + 1`
	fun_id=4
	key_id=$(word2string 1) 
	key_type=79
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 4) 
	info_type=75
	info_len="04"
	info_val=$(dword2string $2)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 4 "$exp_data"
	if [ $? != 0 ] ; then
        echo_fail "verify trap $show_id dest port $2 fail!"
        echo "verify trap $show_id dest port $2 fail!" >>$SAVEFILE
        error_quit
    else 
        echo_success "verify trap $show_id dest port $2 success!"
        return 0
    fi
}

#校验trap的启用状态
check_bmc_trap_enable_flag()
{
	show_id=`expr $1 + 1`
	fun_id=4
	key_id=$(word2string 1) 
	key_type=79
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 2) 
	info_type=79
	info_len="01"
	info_val=`printf "%02x" $2`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 2 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "check trap $show_id enable state $2 fail!"
       echo "check trap $show_id enable state $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "check trap $show_id enable state $2 success!"
       return 0
    fi
}

#校验trap使能状态
check_set_bmc_trap_on()
{
    getini $CONFIGFILE Custom_Trap${1}_IPADDR
	TRAP_IP_ADDR=$parameter
	getini $CONFIGFILE Custom_Trap${1}_PORT
	TRAP_PORT=$parameter
	
	if [ "$TRAP_PORT" = "" ] ; then
		TRAP_PORT="162"
	fi
	#对于port的有效判断
	if [ "$TRAP_PORT" -gt "65535" ] ; then
		echo_fail "set trap dest $1 port failed! portvalue(1~65535)"
		error_quit
	fi
	
	if [ "$TRAP_IP_ADDR" = "" ] ; then
		echo_fail "trap $1 ipaddr must be set!"
		echo "trap $1 ipaddr must be set!" >>$SAVEFILE
		error_quit
	else
		#新平台的trapitem从0开始标
		TRAP_ITEM_ID=`expr $1 - 1`
		#校验trap启用状态
		check_bmc_trap_enable_flag $TRAP_ITEM_ID 1
		check_set_bmc_trap_dest_ipaddr $TRAP_ITEM_ID $TRAP_IP_ADDR
		check_set_bmc_trap_dest_port   $TRAP_ITEM_ID $TRAP_PORT
	fi
}

#校验trap去使能状态
check_set_bmc_trap_off()
{
	#新平台的trapitem从0开始标
	TRAP_ITEM_ID=`expr $1 - 1`
	#校验trap启用状态
	check_bmc_trap_enable_flag $TRAP_ITEM_ID 0
	#校验ip
	check_set_bmc_trap_dest_ipaddr $TRAP_ITEM_ID ""
	#校验port
	check_set_bmc_trap_dest_port $TRAP_ITEM_ID "162"
}   

#校验trap使能状态
check_trap_enable()
{
	fun_id=67
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=01
	info_id=$(word2string 1)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 1 "$exp_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "check Trap service state $1 fail!"
		error_quit
	else
		echo_and_save_success "check Trap service state $1 success!"
		return 0
	fi
}

#校验trap版本
check_trap_version()
{
	fun_id=67
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=01
	info_id=$(word2string 2)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`

	if [ "$1" != 0 ] && [ "$1" != 1 ] && [ "$1" != 3 ] ; then
		echo_and_save_fail "Invalid trap verision($info_val)"
		error_quit
	fi

	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 2 "$exp_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "check Trap version $1 fail!"
		error_quit
	else
		echo_and_save_success "check Trap version $1 success!"
		return 0
	fi
}

#校验trap SNMPv3用户
check_trap_snmpv3_user_id()
{
	fun_id=67
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=01
	info_id=$(word2string 5)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`

	is_digit $1
	if [ $? != 0 ]; then
		echo_fail "Set Trap SNMPv3 user ID($1) fail!"
		error_quit
	fi
	if [ "$1" -gt "17" ] || [ "$1" -lt "2" ] ; then
		echo_fail "Set Trap SNMPv3 user ID($1) fail!"
		error_quit
	fi

	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 5 "$exp_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "check Trap SNMPv3 user $1 fail!"
		error_quit
	else
		echo_and_save_success "check Trap SNMPv3 user $1 success!"
		return 0
	fi
}

#校验Trap团体名
check_trap_community_name()
{
	pwd_str=$1
	pwd_info=$(string2hex_with_prefix "$pwd_str")

	#整个字符串长度
	strlen=`echo ${#pwd_str}`

	#密码长度为1-32
	if [ $strlen -gt 32 ] || [ $strlen -lt 1 ]; then
		echo_fail "Length($strlen) of Trap password is too big!"
		echo "Length($strlen) of Trap password is too big!" >> $SAVEFILE
		error_quit
	fi
	info_len=`printf "0x%02x" $strlen`
	run_ipmitool "raw 0x30 0x93 0xdb 0x07 0x00 0x15 0x00 0x15 0x00 0x00 $info_len $pwd_info" "db 07 00"
	if [ $? = 0 ] ; then
		echo_and_save_success "check Trap community name $1 success!"
		return 0
	else
		echo_and_save_fail "check Trap community name $1 fail!"
		error_quit
	fi
}

#校验trap告警发送级别
check_bmc_trap_alarm_severity()
{
	local TrapSeverity=$1
	if [ "$TrapSeverity" != 0 ] && [ "$TrapSeverity" != 8 ] && [ "$TrapSeverity" != 12 ] && [ "$TrapSeverity" != 14 ] && [ "$TrapSeverity" != 15 ] ; then
		echo_and_save_fail "Invalid trap severity type($TrapSeverity)"
		error_quit
	fi
	local value=`printf "%02x" $TrapSeverity`
	run_cmd "f 0 30 93 DB 07 00 16 00 17 00 00 01" "0f 31 00 93 00 db 07 00 $value"

	if [ $? != 0 ] ; then
		echo_and_save_fail "Check Trap severity $TrapSeverity fail!"
		error_quit
	else
		echo_and_save_success "Check Trap severity $TrapSeverity success!"
		return 0
	fi
}

#校验版权信息
check_about_info()
{
	about_type=$1
	about_str=$2
	local RETRY_COUNT=10
	
    if [ "$about_str" != "" ] ; then
		about_info=$(string2hex "$about_str")
		#整个字符串长度
		strlen=`echo ${#about_str}`
	else
		about_info="" 
		strlen=0
	fi
		
	fun_id=7
	key_id=$(word2string 2) 
	key_type=75
	key_len="04"
	key_val=$(dword2string 35) 
	info_id=$(word2string $about_type) 
	info_type=73
	info_val=$about_info
	info_len=`printf "%02x" $strlen`
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	
	for ((i=0;i<$RETRY_COUNT;i++))
	do
		get_prop_cmd $fun_id "$key_string" $about_type "$exp_data"
		if [ $? != 0 ] ; then
			continue
		else
			return 0
		fi
	done
	
	return 1
}



#检查定制化VNC端口使能状态
check_vnc_enable_state()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 1)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 1 "$exp_data"
	if [ $? != 0 ] ; then
       echo_and_save_fail "check VNC service state $1 fail!"
       error_quit
    else 
       echo_and_save_success "check VNC service state $1 success!"
       return 0
    fi
}

#检查定制化KVM超时时间
check_kvm_timeout()
{
	fun_id=10
	key_id=$(word2string 0xffff)
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 5)
	info_type=69
	info_len="04"
	info_val=$(dword2string $1)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 5 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "check KVM timeout fail!"
       echo "check KVM timeout fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "check KVM timeout success!"
       return 0
    fi
}

#检查定制化VNC密码
check_vnc_password()
{
	pwd_str=$1
	if [ "$pwd_str" != "" ] ; then
		pwd_info=$(string2hex "$pwd_str")
		#整个字符串长度
		strlen=`echo ${#pwd_str}`
		#密码长度为1-8
		if [ $strlen -gt 8 ] || [ $strlen -lt 1 ]; then
			echo_fail "Length($strlen) of VNC password is too big!"
			echo "Length($strlen) of VNC password is too big!" >> $SAVEFILE
			error_quit
		fi
	else
		pwd_info=00
		strlen=1
	fi
	info_len=`printf "%02x" $strlen`
	run_cmd "0f 00 30 94 DB 07 00 67 00 02 00 00 00 $info_len $pwd_info" "0f 31 00 94 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "check VNC password fail!"
		echo "check VNC password $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "check VNC password $1 success!"
		return 0
	fi
}

#检查定制化VNC超时时间
check_vnc_timeout()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 3)
	info_type=69
	info_len="04"
	info_val=$(dword2string $1)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "check VNC timeout fail!"
       echo "check VNC timeout fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "check VNC timeout success!"
       return 0
    fi
}

#检查定制化VNC SSL使能状态
check_vnc_ssl_state()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 4)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 4 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "check VNC SSL state $1 fail!"
       echo "check VNC SSL state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "check VNC SSL state $1 success!"
       return 0
    fi
}


#检查定制化VNC监听端口号
check_vnc_port()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 5)
	info_type=69
	info_len="04"
	info_val=$(dword2string $1)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 5 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "check VNC port fail!"
       echo "check VNC port fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "check VNC port success!"
       return 0
    fi
}

#检查定制化VNC键盘布局
check_vnc_keyboard_layout()
{
	keyboard_layout_set="en,jp"
	about_str=$1
	if [ "$about_str" != "" ] ; then
		if [[ $keyboard_layout_set =~ $about_str ]] ; then
			about_layout=$(string2hex "$about_str")
			strlen=`echo ${#about_str}`
		else
			echo "$about_str is not a member of the keyboard_layout_set"
			return 1
		fi
	else
		about_layout=""
		strlen=0
	fi
	fun_id=102
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 6)
	info_type=73
	info_val=$about_layout
	info_len=`printf "%02x" $strlen`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 6 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "check VNC keyboard layout fail!"
       echo "check VNC keyboard layout fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "check VNC keyboard layout success!"
       return 0
    fi	
}

#检查定制化VNC登录规则
check_vnc_permit_rule()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 7)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 7 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "check VNC PermitRuleIds $1 fail!"
       echo "check VNC PermitRuleIds $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "check VNC PermitRuleIds $1 success!"
       return 0
    fi
}



#校验语言集合信息
check_about_languageSet()
{
	about_type=$1
	about_str=$2
	local RETRY_COUNT=10
	
    if [ "$about_str" != "" ] ; then
		about_language=$(string2hex "$about_str")
		#整个字符串长度
		strlen=`echo ${#about_str}`
	else
		about_language="" 
		strlen=0
	fi
		
	fun_id=13
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01" 
	info_id=$(word2string $about_type) 
	info_type=73
	info_val=$about_language
	info_len=`printf "%02x" $strlen`
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	for ((i=0;i<$RETRY_COUNT;i++))
	do
		get_prop_cmd $fun_id "$key_string" $about_type "$exp_data"
		if [ $? != 0 ] ; then
			continue
		else
			return 0
		fi
	done
	return 1
}

#校验语言集合属性值
check_language_info()
{
	local languageSet_str=$1
	
	check_about_languageSet 8 "$languageSet_str"	
	if [ $? != 0 ] ; then
      	echo_fail "check languageSet to $1 fail"
		echo "check languageSet to $1 fail" >>$SAVEFILE
		error_quit		
    else
		echo_success "check languageSet to $1 success"
		echo "check languageSet to $1 success" >>$SAVEFILE
   	fi	
}

#校验版权信息
check_copyright_info()
{
	local copyright_str=$1
	
	check_about_info 3 "$copyright_str"	
	if [ $? != 0 ] ; then
       	echo_fail "check copyright is $1 fail"
		echo "check copyright is $1 fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "check copyright is $1 success"
		echo "check copyright is $1 success" >>$SAVEFILE
   	fi	
}


#校验support地址信息
check_officalweb_info()
{
	local officalweb_str=$1
	
	check_about_info 1 "$officalweb_str"	
	if [ $? != 0 ] ; then
       	echo_fail "check offical web is $1 fail"
		echo "check offical web is $1 fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "check offical web is $1 success"
		echo "check offical web is $1 success" >>$SAVEFILE
   	fi	
}

#校验web界面JavaKvm入口显示状态
check_javakvm_visible_state()
{
	run_cmd "f 0 30 93 DB 07 00 5b 0e 00 00 00" "0f 31 00 93 00 db 07 00 0$1"
    if [ $? != 0 ] ; then
       	echo_fail "Check JavaKvm visible state is $1 fail"
		echo "Check JavaKvm visible state is $1 fail" >>$SAVEFILE
		error_quit		
    else
		echo_success "Check JavaKvm visible state is $1 success"
		echo "Check JavaKvm visible state is $1 success" >>$SAVEFILE
		return 0
    fi	
}

#检查自动发现状态  0 disable; 1 enable
check_autodiscovery_state()
{
	fun_id=17
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 1) 
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $1`
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 1 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "check_autodiscovery_state $1 Fail"
		echo "check_autodiscovery_state $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_autodiscovery_state $1 success!"
		echo "check_autodiscovery_state $1 success!" >>$SAVEFILE
	fi
}

#检查自动发现广播网段 255.255.255.255/192.168.255.255等
check_autodiscovery_broadcast()
{
	BROADCAST_INFO=$1
	string2substring "$BROADCAST_INFO"
	strlenlen=`echo ${#BROADCAST_INFO}`
	
	fun_id=17
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 2) 
	info_type=73 #s
	info_len=`printf "%02x" $strlenlen`
	info_val=$TMP_STRING
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 2 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "check_autodiscovery_broadcast $BROADCAST_INFO Fail"
		echo "check_autodiscovery_broadcast $BROADCAST_INFO Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_autodiscovery_broadcast $BROADCAST_INFO success!"
		echo "check_autodiscovery_broadcast $BROADCAST_INFO success!" >>$SAVEFILE
	fi
}

#检查自动发现广播端口，默认26957
check_autodiscovery_port()
{
	fun_id=17
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 3) 
	info_type=75 #u
	info_len="04"
	info_val="$(dword2string $1)"
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "check_autodiscovery_port $1 Fail"
		echo "check_autodiscovery_port $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_autodiscovery_port $1 success!"
		echo "check_autodiscovery_port $1 success!" >>$SAVEFILE
	fi
}
# 检查diagnose模块数据采集服务状态
check_data_acquisition_service_status()
{
	# 不支持数据采集的产品没有相应的对象，所以检查前先判断对象是否存在
	command="f 0 30 93 db 07 00 36 72 00 01 ff 00 00 ff ff 79 01 00 01 00"
	result="0f 31 00 93 d3"
	run_cmd "$command" "$result"
	if [ $? == 0 ] ; then
		echo_success "Check data acquisition service status success!"
		echo "Check data acquisition service status success!" >>$SAVEFILE
		return 0   # 查询到设备不支持数据采集，返回成功
	fi

	command="f 0 30 93 db 07 00 36 72 00 01 ff 00 00 ff ff 79 01 00 01 00"
	if [ 0 == $1 ]
	then
		result="0f 31 00 93 00 db 07 00 00 01 00 79 01 00"
		run_cmd "$command" "$result"
	else
		result="0f 31 00 93 00 db 07 00 00 01 00 79 01 01"
		run_cmd "$command" "$result"
	fi	
	if [ $? != 0 ] ; then
		echo_fail "Check data acquisition service status $1 failed!"
		echo "Check data acquisition service status $1 failed!" >>$SAVEFILE
		error_quit
	else
		echo_success "Check data acquisition service status $1 success!"
		echo "Check data acquisition service status $1 success!" >>$SAVEFILE
		return 0
	fi
}

#校验机器型号
check_set_server_tag()
{
	servertag=$1
	fun_id=18
	key_id=$(word2string 1) 
	key_type=79
	key_len=1
	key_val=`printf "%02x" 1`
	info_id=$(word2string 3) 
	info_type=75
	info_len=`printf "%02x" 4`
	info_val=$(dword2string $servertag)
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"
	if [ $? != 0 ] ; then
        	echo_fail "verify server tag $1 fail!"
        	echo "verify server tag $1 fail!" >>$SAVEFILE
        	error_quit
    	else 
        	echo_success "verify server tag $1 success!"
        	return 0
    	fi
}

#校验web超时时间
check_web_timeout()
{
	fun_id=51
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 1) 
	info_type=69 #i
	info_len="04"
	info_val=$(dword2string $1)
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 1 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "check_web_timeout $1 Fail"
		echo "check_web_timeout $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_web_timeout $1 success!"
		echo "check_web_timeout $1 success!" >>$SAVEFILE
	fi
}

#校验web会话模式
check_web_session_mode()
{
	fun_id=51
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 2) 
	info_type=79
	info_len="01"
	info_val=`printf "%02x" $1`

	if [ $1 == 0 ]; then
		mode_str='share'
	else
		mode_str='monopoly'
	fi

	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 2 "$exp_data"

	if [ $? != 0 ] ; then
		echo_fail "verify web session mode $mode_str Fail"
		echo "verify web session mode $mode_str Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "verify web session mode $mode_str success!"
		echo "verify web session mode $mode_str success!" >>$SAVEFILE
	fi
}

#校验syslog的总体启用状态,00 关闭,01 开启
check_bmc_syslog_general_enable_flag()
{
	if [ "$1" -gt "1" -o "$1" -lt "0" ] ; then
		echo_fail "set syslog general enable state fail! (0-1)"
		error_quit
	fi
	fun_id=63
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 1) 
	info_type=79
	info_len="01"
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 1 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "verify syslog general enable state $1 fail!"
       echo "verify syslog general enable state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "verify syslog general enable state $1 success!"
       return 0
    fi
}

#校验syslog的主机标识, 01单板序列号,02资产标签,03主机名
check_bmc_syslog_host_flag()
{
	if [ "$1" -gt "3" -o "$1" -lt "1" ] ; then
		echo_fail "set syslog host flag fail! (1-3)"
		error_quit
	fi
	fun_id=63
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 2)
	info_type=75
	info_len="04"
	info_val=$(dword2string $1)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 2 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "verify syslog host flag $1 fail!"
       echo "verify syslog host flag $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "verify syslog host flag $1 success!"
       return 0
    fi
}

#校验syslog的告警级别, 00反选所有,01正常,02轻微,04严重,08紧急
check_bmc_syslog_alarm_level()
{
	if [ "$1" -gt "15" -o "$1" -lt "0" ] ; then
		echo_fail "set syslog alarm level fail! (0-8)"
		error_quit
	fi
	fun_id=63
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 3)
	info_type=75
	info_len="04"
	info_val=$(dword2string $1)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog alarm level $1 fail!"
       echo "set syslog alarm level $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog alarm level $1 success!"
       return 0
    fi
}

#校验syslog的传输协议, 01 UDP,02 TCP,03 TLS
check_bmc_syslog_trans_protocol()
{
	if [ "$1" -gt "3" -o "$1" -lt "1" ] ; then
		echo_fail "set syslog trans protocol fail! (1-3)"
		error_quit
	fi
	fun_id=63
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 5)
	info_type=79
	info_len="01"
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 5 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog transport protocol $1 fail!"
       echo "set syslog transport protocol $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog transport protocol $1 success!"
       return 0
    fi
}

#校验syslog单项的使能状态,id 0-3
check_bmc_syslog_enable_state()
{
	show_id=`expr $1 + 1`
	if [ "$2" -gt "1" -o "$2" -lt "0" ] ; then
		echo_fail "set syslog $show_id enable state fail! (1-3)"
		error_quit
	fi
	fun_id=64
	key_id=$(word2string 1) 
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 2) 
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $2`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 2 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog $show_id enable state to $2 fail!"
       echo "set syslog $show_id enable state to $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog $show_id enable state to $2 success!"
       return 0
    fi
}

#校验syslog单项的日志类型,id 0-3,1操作日志,2安全日志,4事件日志,7所有日志
check_bmc_syslog_log_type()
{
	show_id=`expr $1 + 1`
	if [ "$2" -gt "7" -o "$2" -lt "1" ] ; then
		echo_fail "set syslog $show_id log type fail! (1-3)"
		error_quit
	fi
	fun_id=64
	key_id=$(word2string 1) 
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 3) 
	info_type=75 #u
	info_len="04"
	info_val=$(dword2string $2)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog $show_id log type to $2 fail!"
       echo "set syslog $show_id log type to $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog $show_id log type to $2 success!"
       return 0
    fi
}

#校验syslog单项的服务器地址
check_bmc_syslog_hostaddr()
{
	if [ "$2" != "" ] ; then
		hostaddr=$2
		strlen=`echo ${#hostaddr}`
		if [ $strlen -gt 255 ] ; then
			echo_fail "syslog$1 host addr'len($strlen) is too big!"
			echo "syslog$1 host addr'len($strlen) is too big!" >>$SAVEFILE
			error_quit
		fi	
		hostaddr=$(string2hex "$hostaddr")
	else
		#删除服务器地址
		hostaddr=""
		strlen=0
	fi
		
	fun_id=64
	key_id=$(word2string 0x01)
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 4)
	info_type=73 #s
	info_len=`printf "%02x" $strlen`
	info_val=$hostaddr
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 4 "$exp_data"
	if [ $? != 0 ] ; then
       echo_fail "set syslog host addr fail!"
       echo "set syslog host addr fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog host addr success!"
       return 0
    fi
}


#校验syslog单项的端口号,id 0-3
check_bmc_syslog_dest_port()
{
	show_id=`expr $1 + 1`
	if [ "$2" -gt "65535" -o "$2" -lt "0" ] ; then
		echo_fail "set syslog $show_id dest port to $2 fail! portvalue(1~65535)"
		error_quit
	fi
	port=$2
	fun_id=64
	key_id=$(word2string 1) 
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 5) 
	info_type=75 #u
	info_len="04"
	info_val=$(dword2string $port)
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 5 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog $show_id dest port to $2 fail!"
       echo "set syslog $show_id dest port to $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog $show_id dest port to $2 success!"
       return 0
    fi
}

#校验syslog总体定制项目入口
check_bmc_syslog_general_status()
{
	getini $CONFIGFILE BMCSet_CustomSyslog_State
	SYSLOG_GE_ENABLE_STATE=$parameter
	if [ "$SYSLOG_GE_ENABLE_STATE" == "" ] ; then
			SYSLOG_GE_ENABLE_STATE=0
	fi
	check_bmc_syslog_general_enable_flag $SYSLOG_GE_ENABLE_STATE
	
	getini $CONFIGFILE BMCSet_CustomSyslog_Host_Flag
	SYSLOG_HOST_FLAG=$parameter
	if [ "$SYSLOG_HOST_FLAG" == "" ] ; then
			SYSLOG_HOST_FLAG=1
	fi
	check_bmc_syslog_host_flag $SYSLOG_HOST_FLAG

    getini $CONFIGFILE BMCSet_CustomSyslog_Alarm_Level
	SYSLOG_ALARM_LEVEl=$parameter
	if [ "$SYSLOG_ALARM_LEVEl" == "" ] ; then
			SYSLOG_ALARM_LEVEl=15
	fi
	check_bmc_syslog_alarm_level $SYSLOG_ALARM_LEVEl
	
	getini $CONFIGFILE BMCSet_CustomSyslog_Trans_Protocol
	SYSLOG_TRANS_PROTOCOL=$parameter
	if [ "$SYSLOG_TRANS_PROTOCOL" == "" ] ; then
			SYSLOG_TRANS_PROTOCOL=3
	fi
	check_bmc_syslog_trans_protocol $SYSLOG_TRANS_PROTOCOL
}

#设置syslog单项定制项目入口
check_bmc_syslog_status()
{
	getini $CONFIGFILE BMCSet_CustomSyslog_${1}_State
	SYSLOG_ENABLE_STATE=$parameter
	if [ "$SYSLOG_ENABLE_STATE" == "" ] ; then
			SYSLOG_ENABLE_STATE=0
	fi
	check_bmc_syslog_enable_state $1 $SYSLOG_ENABLE_STATE
	
	getini $CONFIGFILE BMCSet_CustomSyslog_${1}_LogType
	SYSLOG_LOG_TYPE=$parameter
	if [ "$SYSLOG_LOG_TYPE" == "" ] ; then
			SYSLOG_LOG_TYPE=7
	fi
	check_bmc_syslog_log_type $1 $SYSLOG_LOG_TYPE

    getini $CONFIGFILE BMCSet_CustomSyslog_${1}_IpAddr
	SYSLOG_IP_ADDR=$parameter
	if [ "$SYSLOG_IP_ADDR" == "" ] ; then
			SYSLOG_IP_ADDR=""
	fi
	check_bmc_syslog_hostaddr $1 $SYSLOG_IP_ADDR
	
	getini $CONFIGFILE BMCSet_CustomSyslog_${1}_Port
	SYSLOG_PORT=$parameter
	if [ "$SYSLOG_PORT" == "" ] ; then
			SYSLOG_PORT=0
	fi
	check_bmc_syslog_dest_port $1 $SYSLOG_PORT
}

#校验传感器号动态分配时的起始号
check_bmc_dynamic_sensor_num_base()
{
	fun_id=68
	key_id=$(word2string 0xffff) 
	key_type=0
	key_len="01"
	key_val="00"
	info_id=$(word2string 1)
	info_type=79
	info_len="01"
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 1 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "set dynamic sensor num base $1 fail!"
       echo "set dynamic sensor num base $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "sset dynamic sensor num base $1 success!"
       return 0
    fi
}

#设置选择节点上报告警属性,目前只有X6800有
check_Single_Node_Alarm()
{
	alarm_policy=`printf "%02x" $1`
	if [ $alarm_policy != 00 ] && [ $alarm_policy != 01 ]; then
		echo_fail "verify single node alarm policy $SingleNodeAlarm fail! parameter out of range."
		echo "verify single node alarm policy $SingleNodeAlarm fail!parameter out of range." >> $SAVEFILE
		error_quit
	fi
	echo "ipmitool raw 0x30 0x93 0xdb 0x07 0x00 0x36 0x13 0x00 0x01 0xff 0x00 0x00 0xff 0xff 0x00 0x01 0x00 0x0c 0x00 0x00" >>$IPMI_CMD_LOG_FILE
	#命令如果发送不成功则重试，重试间隔1秒，重试5次仍然不成功则复位bmc，最多复位3次，复位3次不成功则返回失败
	#复位重试3次，防止sendmsg转发失败和mm板复位命令转发不过去的场景
	for ((reset_times=0;reset_times<3;reset_times++))
	do
		verify_fail_times=0
		for((i=0;i<5;i++))
		do
			ipmitool raw 0x30 0x93 0xdb 0x07 0x00 0x36 0x13 0x00 0x01 0xff 0x00 0x00 0xff 0xff 0x00 0x01 0x00 0x0c 0x00 0x00 > $IPMI_CMD_RETURN_FILE 2>&1
			cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
			grep -wq "db 07 00" $IPMI_CMD_RETURN_FILE
			if [ $? = 0 ] ; then
				grep -wq "db 07 00 00 0c 00 79 01 "$alarm_policy"" $IPMI_CMD_RETURN_FILE 
				#命令返回成功
				if [ $? = 0 ] ; then
					echo_success "verify single node alarm policy $SingleNodeAlarm success!"
					echo "verify single node alarm policy $SingleNodeAlarm success!" >> $SAVEFILE
					return 0
				else
					echo_fail "verify single node alarm policy $SingleNodeAlarm fail!"
					echo "verify single node alarm policy $SingleNodeAlarm fail!" >> $SAVEFILE
					error_quit
				fi
					
			fi
			#如果命令返回值是ff或c3则通过发送06 01 确认命令发送超时，如果06 01 可以正常发送，则说明不是超时，直接反回成功，打印校验失败
			grep -Ewq "rsp=0xff|rsp=0xc3" $IPMI_CMD_RETURN_FILE
			if [ $? = 0 ] ; then				
				#休眠1秒
				sleep 1 
				#通过发送06 01 确定命令发送超时
				ipmitool raw 6 1
				if [ $? -ne 0 ] ; then
				#06 01 发送不成功，确认超时，sleep1后发送查询告警策略命令
					echo "get version fail!"
					sleep 1
				else 
				#06 01 发送成功，校验命令发送失败(可能是MM板版本不支持查询节点告警的命令)
					((verify_fail_times=verify_fail_times+1));
					echo "verify_fail_times:$verify_fail_times" 
				fi
				#连续3次查询告警策略命令发送返回0xff或0xc3，但是06 01可以发送成功，返回成功，打印校验失败
				if [ $verify_fail_times = 3 ] ;then
				{
					echo_fail "verify single node alarm policy $SingleNodeAlarm fail!"
					echo "verify single node alarm policy $SingleNodeAlarm fail!" >> $SAVEFILE
					return 0
				}
				fi
							
			#非c3和ff的其他错误码都校验失败，但是不会返回错误退出
			else
				echo_fail "verify single node alarm policy $SingleNodeAlarm fail!"
				echo "verify single node alarm policy $SingleNodeAlarm fail!" >> $SAVEFILE
				return 0
			fi		
			echo "retry_times:$i" 
		done
		#重试5次不成功，复位bmc
		if [ $i = 5 ] ; then
			reset_bmc
			echo "reset_times:$reset_times"
		fi
		
	done
	#复位3次不成功则返回失败
	if [ $reset_times = 3 ] ; then
	echo_fail "verify single node alarm policy $SingleNodeAlarm fail!"
    echo "verify single node alarm policy $SingleNodeAlarm fail!" >>$SAVEFILE
	error_quit
	fi
	
	

}
#检查启动项定制
check_boot_device()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 1 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check Boot device is $info_val fail"
		echo "Check Boot device is $info_val fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check Boot device is $info_val success"
		echo "Check Boot device is $info_val success" >>$SAVEFILE
		return 0
   	fi
}

#检查启动生效模式定制
check_boot_effect_mode()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 2 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check Boot effect mode is $info_val fail"
		echo "Check Boot effect mode is $info_val fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check Boot effect mode is $info_val success"
		echo "Check Boot effect mode is $info_val success" >>$SAVEFILE
		return 0
   	fi
}

#检查启动模式切换功能的启用状态
check_boot_mode_changeover_status()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 4)
	info_type=79 #y类型
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 4 "$expect_data"
	
	if [ $? != 0 ] ; then
	   echo_fail "check_boot_mode_changeover_status $info_val fail!"
       echo "check_boot_mode_changeover_status $info_val fail!" >>$SAVEFILE
	   error_quit
	else
       echo_success "check_boot_mode_changeover_status $info_val success!"
	   return 0
	fi
}

#检查启动模式
check_boot_mode()
{	
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 5)
	info_type=79 #y类型
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 5 "$expect_data"
	
	if [ $? != 0 ] ; then
	   echo_fail "check_boot_mode $info_val fail!"
       echo "check_boot_mode $info_val fail!" >>$SAVEFILE
	   error_quit
	else
       echo_success "check_boot_mode $info_val success!"
	   return 0
	fi
}

#校验WOL使能状态
check_wake_on_lan_state()
{
	fun_id=48					 #48的16进制，表示PRODUCT类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 					 #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=$(word2string 5)
	info_type=79				#y的ascii码,16进制
	info_len=01
	info_val=`printf "%02x" $1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 5 "$exp_data"	
	if [ $? != 0 ] ; then
       echo_fail "set WOL state $1 fail!"
       echo "set WOL state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set WOL state $1 success!"
       return 0
    fi
}

#校验Thermal Policy定制化模式
check_thermal_mode()
{
	local mode=""
	if [ "$1" == 02 ] ; then
		mode="Middle Fan Speed"
	elif [ "$1" == 01 ] ; then
		mode="High Fan Speed"
	elif [ "$1" == 00 ] ; then
		mode="Low Fan Speed"
	elif [ "$1" == 10 ] ; then
		mode="Energy saving"
	elif [ "$1" == 11 ] ; then
		mode="Low noise"
	elif [ "$1" == 12 ] ; then
		mode="High performance"
	elif [ "$1" == 13 ] ; then
		mode="Custom"
	elif [ "$1" == 14 ] ; then
		mode="Liquid"
	else
	    echo "Check thermal policy mode: $mode fail,invalid mode!"
            echo "Check thermal policy mode: $mode fail,invalid mode!" >>$SAVEFILE
            error_quit
	fi
			
	fun_id=90					 #90的16进制，表示COOLING类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 					 #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=$(word2string 8)
	info_type=79				#y的ascii码,16进制
	info_len=01
	info_val=$1
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 8 "$exp_data"			
	
	if [ $? != 0 ] ; then
        echo_fail "Check thermal policy mode is $mode, check fail!"
        echo "Check thermal policy mode is $mode, check fail!" >>$SAVEFILE
        error_quit
   else
        echo_success "Check thermal policy mode: $mode success!"
        echo "Check thermal policy mode: $mode success!" >>$SAVEFILE
        return 0
	fi
}

check_Customized_InletTemp_Speed()
{	
	InletTempSpeed_str="$1"
	InletTempSpeed=""
	if [ "$InletTempSpeed_str" != "" ] ; then
		InletTempSpeed_str=${InletTempSpeed_str//,/ }
		for i in $InletTempSpeed_str 
		do tempStr=`printf "%02x" $i`
			InletTempSpeed=$InletTempSpeed$tempStr" "
		done
	else
		InletTempSpeed="03 14 1e 28 28 3c 50 64"
	fi
	run_cmd "f 0 30 93 DB 07 00 46" "0f 31 00 93 00 db 07 00 $InletTempSpeed" 
	if [ $? != 0 ] ; then
        	echo_fail "check Customized_InletTemp_Speed $InletTempSpeed_info fail!"
        	echo "check Customized_InletTemp_Speed $InletTempSpeed_info fail!" >>$SAVEFILE
        	error_quit
    	else
		echo_success "check Customized_InletTemp_Speed $InletTempSpeed_info success!"
        	echo "check Customized_InletTemp_Speed $InletTempSpeed_info success!" >>$SAVEFILE
        	return 0
   	fi
}

check_Customized_CpuCoremTobj()
{	
	fun_id=90
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 6)  #CpuCoremTobj infoid 6
	info_type=79 #y类型
	info_len=01
	info_val=`printf "%02x" $1`	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 6 "$expect_data"
	
	if [ $? != 0 ] ; then
		echo_fail "check_CpuCoremTobj $1 fail!"
       		echo "check_CpuCoremTobj $1 fail!" >>$SAVEFILE
	   	error_quit
	else
       		echo_success "check_CpuCoremTobj $1 success!"
	   	return 0
	fi
}



check_Customized_OutletTobj()
{	
	fun_id=90
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 10)  #CpuCoremTobj infoid 10
	info_type=79 #y类型
	info_len=01
	info_val=`printf "%02x" $1`	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 10 "$expect_data"
	
	if [ $? != 0 ] ; then
		echo_fail "check_OutletTobj $1 fail!"
       		echo "check_OutletTobj $1 fail!" >>$SAVEFILE
	   	error_quit
	else
       		echo_success "check_OutletTobj $1 success!"
	   	return 0
	fi
}


#AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑:校验拓扑配置是否保存到EEPROM
check_pcie_topo_mode()
{	
	fun_id=106
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2)  #待生效拓扑模式
	info_type=79 #y类型
	info_len=01
	info_val=`printf "%02x" $1`	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 2 "$expect_data"
	
	if [ $? != 0 ] ; then
		echo_fail "check pcie topo mode $1 fail!"
       	echo "check pcie topo mode $1 fail!" >>$SAVEFILE
	   	error_quit
	else
       	echo_success "check pcie topo mode $1 successfully!"
	   	return 0
	fi
}

#开启DISK SLOT定制化功能检查
check_set_diskslot_customize_id()
{
    #校验是否开启
	customize_id=$1;	
	get_prop_cmd 113 "ff ff 00 01 00" 1 "01 00 79 01 $customize_id"
	if [ $? != 0 ] ; then
	   echo_fail "verify disk slot customize id to $customize_id fail!"
	   echo "verify disk slot customize id to $customize_id fail!" >>$SAVEFILE
	   error_quit
	else
	   echo_success "verify disk slot customize id to $customize_id success!"
	   echo "verify disk slot customize id to $customize_id success!" >>$SAVEFILE
	   return 0
	fi
}

#检查IPMI设置启动模式选项定制
check_boot_mode_sw_enable()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 10) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 10 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check Boot mode switch enable is $info_val fail"
		echo "Check Boot mode switch enable is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check Boot mode switch enable is $info_val success"
		echo "Check Boot mode switch enable is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#检查IPMITOOL elist设置支持查询iBMC的SEL条数支持可配置定制:1~2000条(默认2000)
check_query_sel_max_num_value()
{
	fun_id=132
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1) 
	info_type=71 #q的ascii码,16进制
	info_len=02
	info_val=$(word2string $1)
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 1 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check SEL max num value is $info_val fail"
		echo "Check SEL max num value is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check SEL max num value is $info_val success"
		echo "Check SEL max num value is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#检查定制化的BMC默认证书加密算法
check_cert_algorithm()
{
	fun_id=13
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 10) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 10 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check certificate algorithm is $info_val fail"
		echo "Check certificate algorithm is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check certificate algorithm is $info_val success"
		echo "Check certificate algorithm is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#检查IPMI消息过滤总开关的状态：0：关闭; 1:开启; 
check_ipmi_fire_wall_status()
{
	fun_id=134
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 1 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check ipmi fire wall status is $info_val fail"
		echo "Check ipmi fire wall status is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check ipmi fire wall status is $info_val success"
		echo "Check ipmi fire wall status is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#检查IPMI消息过滤总开关开启时，设置黑白名单模式的状态：0：黑名单; 1:白名单; 0xff:无效模式
check_ipmi_fire_wall_mode()
{
	fun_id=134
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 2 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check ipmi fire wall mode is $info_val fail"
		echo "Check ipmi fire wall mode is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check ipmi fire wall mode is $info_val success"
		echo "Check ipmi fire wall mode is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#校验定制化的customsysname
check_customized_sysname()
{
	READ_ELABEL_STRING=""
	
	ipmitool raw 0x30 0x93 0xdb 0x07 0x00 0x5b 0x10 0x00 0x00 0x00 >tmp_ipmitool_sysname_cfg.txt
	if [ $? != 0 ]; then
		echo_fail "read sysname fail"
		rm -rf tmp_ipmitool_sysname_cfg.txt
		echo "read sysname fail!" >> $SAVEFILE
		error_quit
	fi

	hextochar tmp_ipmitool_sysname_cfg.txt 10 temp_csutom_sysname.txt
	READ_ELABEL_STRING=`cat temp_csutom_sysname.txt`
	if [ "$READ_ELABEL_STRING" = "$1" ]; then
		echo_success "verify custom sysname success"
		rm -rf tmp_ipmitool_sysname_cfg.txt
		echo "verify custom sysname success" >>$SAVEFILE
		rm -rf temp_csutom_sysname.txt
	else
		echo_fail "verify custom sysname fail"
		rm -rf tmp_ipmitool_sysname_cfg.txt
		echo "verify custom sysname fail" >>$SAVEFILE
		rm -rf temp_csutom_sysname.txt
		error_quit
	fi
}

#检查sysname从hostname同步的开关：0：关闭; 1:开启; 
check_sysname_sync_from_hostname_flag()
{
	fun_id=1
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 14) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 14 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check sysname synchronize from hostname flag $info_val fail"
		echo "Check sysname synchronize from hostname flag $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check sysname synchronize from hostname flag $info_val success"
		echo "Check sysname synchronize from hostname flag $info_val success" >>$SAVEFILE
		return 0
	fi
}
#检查定制化的IErr诊断失败时处理策略
check_fdm_diag_fail_policy()
{
	fun_id=54
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 4) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 4 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check FDM IErr diagnose fail policy is $info_val fail"
		echo "Check FDM IErr diagnose fail policy is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check FDM IErr diagnose fail policy is $info_val success"
		echo "Check FDM IErr diagnose fail policy is $info_val success" >>$SAVEFILE
		return 0
	fi
}
##检查定制化的同步开关
check_sync_hostname_enable()
{
	fun_id=142
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1)
	info_type=79 #y
	info_len=01
	is_digit "$1"
	if [ $? != 0 ]; then
		echo_and_save_fail "Check sync hostname enable $1 fail! valid value(0~1)"
		error_quit
	fi
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 1 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check sync hostname enable is $info_val fail"
		echo "Check sync hostname enable is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check sync hostname enable is $info_val success"
		echo "Check sync hostname enable is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#检查redfish事件上报显示事件详细字段的定制化ID
check_rf_eventdetail_customized_id()
{
	fun_id=99
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 3) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 3 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check redfish event detail customized id is $info_val fail"
		echo "Check redfish event detail customized id is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check redfish event detail customized id is $info_val success"
		echo "Check redfish event detail customized id is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#校验电源按钮定制状态
check_power_button_lock_status()
{
	local short_lock=0
	
	if [ "$1" == 0 ] ; then
		#短按不屏蔽
		short_lock=00
		
		#判断功能是否支持
		run_cmd "f 0 30 93 DB 07 00 5b 0a 00 02 00 00 00" "0f 31 00 93 d5"
		if [ $? == 0 ] ; then
			echo "Not support set_power_button_lock_status!" >>$SAVEFILE
			return 0
		fi
	elif [ "$1" == 1 ] ; then
		#短按 屏蔽
		short_lock=01
	else
		#参数超出范围
		echo_fail "check_power_button_lock_status: para $1 is out of range!"
		echo "check_power_button_lock_status: para $1 is out of range!" >>$SAVEFILE
		error_quit
	fi	
	
	run_cmd "f 0 30 93 DB 07 00 5b 0a 00 02 00 00 00" "0f 31 00 93 00 db 07 00 $short_lock"
	if [ $? != 0 ] ; then
		echo_fail "check_power_button_short_lock_status: $short_lock fail!"
		echo "check_power_button_short_lock_status: $short_lock  fail!" >>$SAVEFILE
		error_quit
	fi
	
	echo_success "check_power_button_lock_status: $short_lock success!"
	echo "check_power_button_lock_status: $short_lock success!" >>$SAVEFILE
	return 0
}
#检查系统软件名称
check_smsname()
{
	strlen=`echo ${#1}`
	strlen_02x=`printf %02x $strlen`
	string2substring "$1"
	TMP_STRING=$(remove_space "$TMP_STRING")
	run_cmd  "f 0 30 93 db 07 00 5b 1e 00" "0f 31 00 93 00 db 07 00 $strlen_02x $TMP_STRING"
	if [ $? != 0 ] ; then
        echo_fail "check SmsName: $1 fail!"
        echo "check SmsName: $1 fail!" >>$SAVEFILE      
	    error_quit		
    else 
        echo_success "check SmsName: $1 success!"
        echo "check SmsName: $1 success!" >>$SAVEFILE
        return 0
    fi
}

# 检查AC上电通电开机场景风扇默认速率比 范围50-100
check_fan_init_default_level()
{
	level=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 5b 1f 00 00 00" "0f 31 00 93 00 db 07 00 $level"
	if [ $? == 0 ] ; then
		echo_success "Check the default initial fan speed ratio with 0x$level success!"
		echo "Check the default initial fan speed ratio with 0x$level success!" >>$SAVEFILE
		return 0
	else
		echo_fail "Check the default initial fan speed ratio with 0x$level fail!"
		echo "Check the default initial fan speed ratio with 0x$level fail!" >>$SAVEFILE
		error_quit
	fi
}

#检查定制化厂商ID
check_custom_manu_id()
{
    local LONGB0=`printf "%08x" $1 | cut -b 7-8`
    local LONGB1=`printf "%08x" $1 | cut -b 5-6`
    local LONGB2=`printf "%08x" $1 | cut -b 3-4`
    run_cmd  "f 0 30 90 22 08" "0f 31 00 90 00 11 $LONGB0 $LONGB1 $LONGB2"
    if [ $? != 0 ] ; then
        echo_fail "check custom manu id fail!"
        echo "check custom manu id fail!" >>$SAVEFILE      
	    error_quit		
    else 
        echo_success "check custom manu id $LONGB0 $LONGB1 $LONGB2 success!"
        echo "check custom manu id $LONGB0 $LONGB1 $LONGB2 success!" >>$SAVEFILE
        return 0
    fi
}

getsysnameini()
{

    FILENAME=$1
    KEYWORD=$2
	
    tmp_param=`cat $FILENAME | grep "^${KEYWORD}\>"`
    parameter=`echo "${tmp_param#*=}"`

}

# 检查USB管理服务功能使能状态
check_usb_mgmt_service_state()
{
	service_state=`printf "%02x" $1`
	run_cmd "f 0 30 93 DB 07 00 5b 11 00 00 00" "0f 31 00 93 00 db 07 00 $service_state"
	if [ $? != 0 ] ; then
		echo_fail "check USB management service state is $service_state fail!"
		echo "check USB management service state is $service_state fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "check USB management service state is $service_state success!"
		echo "check USB management service state is $service_state success!" >>$SAVEFILE
		return 0
	fi
}

# 检查USB管理服务功能使能状态
check_sp_device_info_collect_enable()
{
	service_state=`printf "%02x" $1`
	run_cmd "f 0 30 93 DB 07 00 5b 24 00 00 00" "0f 31 00 93 00 db 07 00 $service_state"
	if [ $? != 0 ] ; then
		echo_fail "check SP device info collect enable state is $service_state fail!"
		echo "check SP device info collect enable state is $service_state fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "check SP device info collect enable state is $service_state success!"
		echo "check SP device info collect enable state is $service_state success!" >>$SAVEFILE
		return 0
	fi
}

#校验SKUNumber
check_smbios_skunumber()
{
	sku_str=$1
	fun_id=101
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2)
	info_type=73
	strlen=${#sku_str}
	info_len=$(printf "%02x" $strlen)
	info_val=$(string2hex "$sku_str")

	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"

	get_prop_cmd $fun_id "$key_string" 2 "$exp_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Check SmBios SKUNumber $1 fail!"
		error_quit
	else
		echo_and_save_success "Check SmBios SKUNumber $1 success!"
		return 0
	fi
}

#检查移动配置模型定制化
check_configuration_model()
{
	model_str=$1
	is_have_space "$model_str"
	if [ $? == 0 ] ; then
		echo_and_save_fail "The configuration model($1) contains spaces"
		error_quit
	fi
	model_info=$(string2hex "$model_str")

	#整个字符串长度
	strlen=`echo ${#model_str}`
	info_len=`printf "%02x" $strlen`
	run_ipmitool "raw 0x30 0x93 0xDB 0x07 0x00 0x5b 0x21 0x00 0x00 0x00" "db 07 00 $info_len $model_info"
	if [ $? != 0 ] ; then
		echo_fail "Check configuration model $1 fail!"
		echo "Check configuration model $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Check configuration model $1 success!"
		return 0
	fi
}

#检查移动版本信息定制化
check_cmcc_version()
{
	version_str=$1
	is_have_space "$version_str"
	if [ $? == 0 ] ; then
		echo_and_save_fail "The cmcc version($1) contains spaces"
		error_quit
	fi
	version_info=$(string2hex "$version_str")
	#整个字符串长度
	strlen=`echo ${#version_str}`
	info_len=`printf "%02x" $strlen`
	run_ipmitool "raw 0x30 0x93 0xDB 0x07 0x00 0x5b 0x22 0x00 0x00 0x00" "db 07 00 $info_len $version_info"
	if [ $? != 0 ] ; then
		echo_fail "Check cmcc version $1 fail!"
		echo "Check cmcc version $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Check cmcc version $1 success!"
		return 0
	fi
}

#检查redfish定制化厂商名称
check_redfish_custom_manu_name()
{
	manu_str=$1
    is_have_space "$manu_str"
    if [ $? == 0 ] ; then
        echo_and_save_fail "The custom manufacturer name($1) contains spaces"
        error_quit
    fi
	manu_info=$(string2hex "$manu_str")

	#整个字符串长度
	strlen=`echo ${#manu_str}`

	#厂商长度为1-128
	if [ $strlen -gt 128 ] || [ $strlen -lt 1 ]; then
		echo_fail "Length($strlen) of manufacturer is too big!"
		echo "Length($strlen) of manufacturer is too big!" >> $SAVEFILE
		error_quit
	fi

	# 先模糊匹配后使用字符串精确匹配
	run_ipmitool "raw 0x30 0x93 0xDB 0x07 0x00 0x5b 0x23 0x00 0x00 0x00" "db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "check custom manufacturer name $1 fail!"
		echo "check custom manufacturer name $1 fail!" >>$SAVEFILE
		error_quit
	else
		hextochar $IPMI_CMD_RETURN_FILE 9 temp_tmp.txt
		read_manu=`cat temp_tmp.txt`
		if [ "$read_manu" != "$manu_str" ] ; then
			echo_fail "check custom manufacturer name $1 fail!"
			echo "check custom manufacturer name $1 fail!" >>$SAVEFILE
			error_quit
		else		
			echo_success "check custom manufacturer name $1 success!"
			return 0
		fi
	fi
}

main()
{
	local is_x86=0
	
	check_is_x86
	is_x86=$?
	
	chmod +x *

	#1 时区定制检测
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE  Custom_TIMEZONE
		TIMEZONE=$parameter
		if [ "$TIMEZONE" == "" ] ; then
			check_bmc_timezone "UTC"
		else
			check_bmc_timezone "$TIMEZONE"	
		fi
	fi

	#2 检测 black_box 定制化, V3产品默认关闭；V5产品默认打开
	echo "PRODUCT_VERSION_NUM=$PRODUCT_VERSION_NUM"
	if [ "$PRODUCT_VERSION_NUM" != "03" ];then
		BLACKBOX="on"
	else
		BLACKBOX="off"
	fi
	getini $CONFIGFILE  BMCSet_BLACKBOX
	if [ "$parameter" != ""	]; then
		BLACKBOX=$parameter
	fi
	if [ "$BLACKBOX" = "on" ] ; then
		check_set_blackbox_on
	else
		check_set_blackbox_off   
	fi
	sleep 1
	
	#自动发现定制化: E9000 不支持
	#使能: E9000 不支持
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE BMCSet_AutoDiscoveryEnable
		AutoDiscoveryEnable=$parameter
		
		if [ "$AutoDiscoveryEnable" = "1" ] ; then
			check_autodiscovery_state 1
			echo "check auto discovery state enable success"
		else
			check_autodiscovery_state 0
			echo "check auto discovery state disable success"
		fi
	fi
	
	#广播地址: E9000 不支持
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE BMCSet_AutoDiscoveryBroadcast
		AutoDiscoveryBroadcast=$parameter
	
		if [ "$AutoDiscoveryBroadcast" != "" ] ; then
			check_autodiscovery_broadcast "$AutoDiscoveryBroadcast"
		else
			check_autodiscovery_broadcast "255.255.255.255"
		fi
	fi
	
	#广播端口: E9000 不支持
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE BMCSet_AutoDiscoveryPort
		AutoDiscoveryPort=$parameter
	
		if [ "$AutoDiscoveryPort" != "" ] ; then
			check_autodiscovery_port "$AutoDiscoveryPort"
		else
			check_autodiscovery_port "26957"
		fi
	fi
	

	####日志告警上报################################################################

	#Trap使能
	getini $CONFIGFILE  BMCSet_TrapEnable
	TrapEnable=$parameter
	if [ "$TrapEnable" != "" ] ; then
		if [ "$TrapEnable" == "on" ] ; then
			check_trap_enable 1
		elif [ "$TrapEnable" == "off" ]; then
			check_trap_enable 0
		else
			echo_and_save_fail "set Trap service state $TrapEnable fail!"
			error_quit
		fi
	else
		check_trap_enable 1
	fi
	#Trap版本
	getini $CONFIGFILE  BMCSet_TrapVersion
	TrapVersion=$parameter
	if [ "$TrapVersion" != "" ] ; then
		check_trap_version $TrapVersion
	else
		check_trap_version 0
	fi
	#Trap SNMPv3用户
	getini $CONFIGFILE  BMCSet_TrapSNMPv3UserID
	TrapSNMPv3UserID=$parameter
	if [ "$TrapSNMPv3UserID" != "" ] ; then
		check_trap_snmpv3_user_id $TrapSNMPv3UserID
	else
		check_trap_snmpv3_user_id 2
	fi
	#Trap团体名
	getini $CONFIGFILE  BMCSet_TrapCommunityName
	TrapCommunityName=$parameter
	if [ "$TrapCommunityName" != "" ] ; then
		check_trap_community_name "$TrapCommunityName"
	elif [ "$TrapVersion" != "3" ] ; then
		check_trap_community_name "TrapAdmin12#$"
	else
		echo_and_save_success "skip Trap community name check with TrapVersion=$TrapVersion"
	fi

	#3 设置多trap目标地址
	IDValue=0
	while [ "$IDValue" -lt "4" ]
	do 
		IDValue=`expr $IDValue + 1`
		getini $CONFIGFILE  BMCSet_TRAP_${IDValue}_Enable
		TRAPEnable=$parameter
		if [ "$TRAPEnable" = "on" ] ; then
			check_set_bmc_trap_on $IDValue
		else
			check_set_bmc_trap_off $IDValue
		fi
		sleep 1
	done

	#trap告警发送级别
	getini $CONFIGFILE BMCSet_TrapAlarmSeverity
	TrapSeverity=$parameter
	if [ "$TrapSeverity" != "" ] ; then
		check_bmc_trap_alarm_severity $TrapSeverity
	else
		check_bmc_trap_alarm_severity 15
	fi

	#4 copyright信息定制化
	getini $CONFIGFILE  BMCSet_Copyright
	Copyright=$parameter
	need_special_dispose
	if [ "$Copyright" == "" -a $? -eq 1 ] ; then
		getini $CONFIGFILE BMCSet_Cooperation_Copyright
		Cooperation_Copyright=$parameter
		if [ "$Cooperation_Copyright" == "null" ] ; then
			Copyright=""
		elif [ "$Cooperation_Copyright" != "" ] ; then
			Copyright = "$Cooperation_Copyright"
		else
			Copyright="Huawei Technologies Co., Ltd. 2004-2022. All rights reserved."
		fi
	fi
	if [ "$Copyright" == "null" ] ; then
		Copyright=""		
	fi	
	check_copyright_info "$Copyright"
	
	#5 offical web信息定制化
	getini $CONFIGFILE  BMCSet_OfficalWeb
	OfficalWeb=$parameter
	need_special_dispose
	special_dispose="$?"
	if [ "$OfficalWeb" == "" -a "${special_dispose}" -eq 1 ] ; then
		getini $CONFIGFILE  BMCSet_Cooperation_OfficalWeb
		Cooperation_OfficalWeb=$parameter
		if [ "$Cooperation_OfficalWeb" == "null" ] ; then
			OfficalWeb=""
		elif [ "$Cooperation_OfficalWeb" != "" ] ; then
		 	OfficalWeb = "$Cooperation_OfficalWeb"
		else
			OfficalWeb="http://enterprise.huawei.com/"
		fi
	fi
	if [ "${special_dispose}" -eq 0 ]; then
		OfficalWeb=""
	fi
	check_officalweb_info "$OfficalWeb"
	
	#定制化语言集合信息
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE  BMCSet_LanguageSet
		languageSet=$parameter
		if [ "$languageSet" == "" ] ; then
			languageSet="en,zh,ja,fr,ru"
		fi
		check_language_info "$languageSet"
	fi
	
	#定制化web界面JavaKvm入口显示
	getini $CONFIGFILE  BMCSet_VisibleJavaKVM
	VisibleJavaKVM=$parameter
	if [ "$VisibleJavaKVM" != "" ] ; then
		if [ "$VisibleJavaKVM" = "on" ] ; then
			check_javakvm_visible_state 1
		elif [ "$VisibleJavaKVM" = "off" ] ; then
			check_javakvm_visible_state 0
		else
			echo_fail "Check JavaKvm visible state $VisibleJavaKVM fail"
			echo "Check JavaKvm visible state $VisibleJavaKVM fail" >>$SAVEFILE
			error_quit	
		fi 
	else
		check_javakvm_visible_state 1
	fi
	
	##检查kvm超时时间
	getini $CONFIGFILE  BMCSet_KVMTimeout
	KVMTimeout=$parameter
	if [ "$KVMTimeout" != "" ] ; then
		check_kvm_timeout $KVMTimeout
	else
		check_kvm_timeout 60
	fi
	
	#VNC服务V3默认关闭，V5默认打开
	if [ "$PRODUCT_VERSION_NUM" != "03" ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	#检查定制化VNC配置信息
	##检查VNC端口使能
	getini $CONFIGFILE BMCSet_VNCEnableState
	VNCEnableState=$parameter
	if [ "$VNCEnableState" != "" ] ; then
		if [ "$VNCEnableState" = "on" ] ; then
			check_vnc_enable_state 1
		else
			check_vnc_enable_state 0
		fi 
	else
		check_vnc_enable_state 0
	fi

	##检查VNC密码
	getini $CONFIGFILE  BMCSet_VNCPassword
	VNCPassword=$parameter
	check_vnc_password "$VNCPassword"

	##检查VNC超时时间
	getini $CONFIGFILE  BMCSet_VNCTimeout
	VNCTimeout=$parameter
	if [ "$VNCTimeout" != "" ] ; then
		check_vnc_timeout $VNCTimeout
	else
		check_vnc_timeout 60
	fi
	##检查VNC SSL使能状态
	getini $CONFIGFILE BMCSet_VNCSSLEnableState
	VNCSSLEnableState=$parameter
	if [ "$VNCSSLEnableState" != "" ] ; then
		if [ "$VNCSSLEnableState" = "on" ] ; then
			check_vnc_ssl_state 1
		else
			check_vnc_ssl_state 0
		fi 
	else
		check_vnc_ssl_state 1
	fi	
	##检查VNC端口号
	getini $CONFIGFILE BMCSet_VNCPort
	VNCPort=$parameter
	if [ "$VNCPort" != "" ] ; then
		check_vnc_port $VNCPort
	else
		check_vnc_port 5900
	fi
	##检查VNC键盘布局
	getini $CONFIGFILE BMCSet_VNCKeyboardLayout
	VNCKeyboardLayout=$parameter
	if [ "$VNCKeyboardLayout" != "" ] ; then
		check_vnc_keyboard_layout $VNCKeyboardLayout
	else
		check_vnc_keyboard_layout "jp"
	fi
	##检查VNC登录规则
	getini $CONFIGFILE BMCSet_VNCPermitRuleIds
	VNCPermitRuleIds=$parameter
	if [ "$VNCPermitRuleIds" != "" ] ; then
		check_vnc_permit_rule $VNCPermitRuleIds
	else
		check_vnc_permit_rule 0
		fi
	fi
	
	# 检查diagnose模块数据采集功能
	getini $CONFIGFILE BMCSet_DataAcquisitionServiceStatus
	DataAcquisitionServiceStatus=$parameter
	if [ "$DataAcquisitionServiceStatus" == "on" ] ; then
		check_data_acquisition_service_status 1
	else
		check_data_acquisition_service_status 0
	fi

	#机器型号定制化
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ]; then
	getini $CONFIGFILE  BMCSet_ServerTag
	ServerTag=$parameter
	if [ "$ServerTag" != "" ] ; then
		check_set_server_tag $ServerTag   
	fi
	fi
	#检查web超时时间
	getini $CONFIGFILE BMCSet_SessionTimeout
	SessionTimeout=$parameter

	if [ "$SessionTimeout" = "" ] ; then
		check_web_timeout 300
		echo "check restore web timeout 5 minutes success"
	else
		second=`expr $SessionTimeout \* 60`
		check_web_timeout $second
		echo "check web timeout $SessionTimeout minutes success"
	fi

	#检查web会话模式
	getini $CONFIGFILE BMCSet_WebSessionMode
	WebSessionMode=$parameter

	if [ "$WebSessionMode" = "share" ] ; then
		check_web_session_mode 0
	else
		check_web_session_mode 1
	fi

	#syslog功能定制
	check_bmc_syslog_general_status
	IDValue=0
	while [ "$IDValue" -lt "4" ]
	do 
		check_bmc_syslog_status $IDValue
		IDValue=`expr $IDValue + 1`
		sleep 1
	done
	
	# 传感器号动态分配时的起始号的定制
	# 这个值给BMC定制了传感器后生效（有SensorOemConfig类对象）
	getini $CONFIGFILE BMCSet_CustomDynamicSensorNumBase
	DynamicSensorNumBase=$parameter
	if [ "$DynamicSensorNumBase" != "" ] ; then
		check_bmc_dynamic_sensor_num_base $DynamicSensorNumBase
	else
		check_bmc_dynamic_sensor_num_base 1
	fi
	
	#只有V5类型的单板才会进入如下Boot定制化配置
	if [ "$PRODUCT_VERSION_NUM" != "03" ] && [ $is_x86 = 1 ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ];then
		#设置启动模式切换功能的启用状态，默认情况打开
		getini $CONFIGFILE BMCSet_BootModeChangeoverStatus
		BootModeChangeoverStatus=$parameter
		if [ "$BootModeChangeoverStatus" == "off" ] ; then
			check_boot_mode_changeover_status 0
		else
			check_boot_mode_changeover_status 1
		fi
		
		#设置启动模式，默认情况为UEFI模式
		getini $CONFIGFILE BMCSet_BootMode
		BootMode=$parameter
		if [ "$BootMode" != "" ] ; then
			check_boot_mode $BootMode
		else
			check_boot_mode 1
		fi
	fi
	
	#检查启动项定制
	getini $CONFIGFILE BMCSet_BootDevice
	BootDevice=$parameter
	if [ "$BootDevice" == "" ] ; then
		check_boot_device 0
	else
		check_boot_device $BootDevice
	fi
	
	#检查启动生效模式定制
	getini $CONFIGFILE BMCSet_BootEffectMode
	BootEffectMode=$parameter
	if [ "$BootEffectMode" == "" ] ; then
		check_boot_effect_mode 1
	else
		check_boot_effect_mode $BootEffectMode
	fi
	
	#选择节点告警策略,目前只有X6800有
	if [ $PRODUCT_ID == $PRODUCT_XSERIALS_TYPE ] || [ $PRODUCT_ID = $PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE ] ; then	
		if [ $BOARD_ID == $BOARD_XH628_XH622_ID ] || [ $BOARD_ID == $BOARD_XH620_ID ] || [ $BOARD_ID == $BOARD_XH321V5_ID ] || [ $BOARD_ID == $BOARD_XH321V5L_ID ] || [ $BOARD_ID == $BOARD_XH628V5_ID ] ; then
			getini $CONFIGFILE BMCSet_SingleNodeAlarm
			SingleNodeAlarm=$parameter
			#如果无此定制化项则默认值0
			if [ "$SingleNodeAlarm" == "" ] ;then
				SingleNodeAlarm=0
			fi
			check_Single_Node_Alarm $SingleNodeAlarm
		fi
	elif [ $PRODUCT_ID == $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID != $BOARD_ATLAS880_ID ] && [ $BOARD_ID != $BOARD_ATLAS800_9010_ID ] && [ $BOARD_ID != $BOARD_G2500_ID ]; then
		getini $CONFIGFILE BMCSet_SingleNodeAlarm
		SingleNodeAlarm=$parameter
		#如果无此定制化项则默认值0
		if [ "$SingleNodeAlarm" == "" ] ;then
			SingleNodeAlarm=0
		fi
		check_Single_Node_Alarm $SingleNodeAlarm	
	fi
	
	#只有OSCA部分刀片支持WOL功能，因此先判断该产品是否支持WOL功能，再进行定制化 DTS2018032601574
	get_wol_config_enable_state
	wol_config_enable=$? 
	if [ "$wol_config_enable" == 1 ];then
		#校验WOL使能状态
		getini $CONFIGFILE BMCSet_WOLState
		WOLState=$parameter

		if [ "$WOLState" == "on" ] ; then
			check_wake_on_lan_state 1
		else
			check_wake_on_lan_state 0
		fi
	fi

	#校验trap模式, V3产品0-事件码模式；V5产品默认2-精准告警模式	
	echo "PRODUCT_VERSION_NUM=$PRODUCT_VERSION_NUM"
	if [ "$PRODUCT_VERSION_NUM" != "03" ];then
		TrapMode=2
	else
		TrapMode=0
	fi
	getini $CONFIGFILE Custom_Trap_Mode
	if [ "$parameter" != ""	]; then
		TrapMode=$parameter
	fi
	check_set_trap_mode $TrapMode

	#校验trap模式定制，默认：0，腾讯定制：1
	getini $CONFIGFILE  BMCSet_TrapModeCustom
	BMCSet_TrapModeCustom=$parameter
	if [ "$BMCSet_TrapModeCustom" != "" ] ; then
		check_trap_mode_custom $BMCSet_TrapModeCustom
	else
		check_trap_mode_custom 0
	fi

	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		#校验Thermal Policy定制化模式
		getini $CONFIGFILE BMCSet_Thermal_Policy
		THERMAL=$parameter
		#获取当前单板是否支持SmartCooling功能
		get_SmartCooling_Enable_state
		SMARTCOOLING_ENABLE=$?
		#获取当前单板是否处于液冷模式
		get_Cooling_Medium_Liquid
		COOLING_MEDIUM_LIQUID=$?
		#如果单板支持SmartCooling功能，需要校验进风口和CpuCorem目标温度
		if [ "$SMARTCOOLING_ENABLE" == 1 ];then
		
			get_SmartCooling_Custom_Setting_Mask
			CUSTOM_SETTING_MASK=$CustomSettingMask
		  
		    if [ $(($CUSTOM_SETTING_MASK&0x01)) != 0 ];then
				getini $CONFIGFILE Custom_Inlet_Temp_Level
				Inlet_Temp_Level=$parameter
				#校验客户配置是否一致
				check_Customized_InletTemp_Speed $Inlet_Temp_Level
			fi
			
		    if [ $(($CUSTOM_SETTING_MASK&0x02)) != 0 ];then			
				getini $CONFIGFILE Custom_CoreRemObjTem
				CoreRemObjTem=$parameter
				#如果CPUCorem目标值为空，则校验默认值Tjmax-15，ARM CPU无Tjmax概念，无需换算，直接65度。
				if [ "$CoreRemObjTem" == "" ] ;then
					#从获取CoreRemObjTem默认值			
					if [ $is_x86 = 1 ];then
						get_CpuCoreRemObjTem	
						CoreRemObjTem=$?
					else
						CoreRemObjTem=65
					fi
				fi
				#校验配置
				check_Customized_CpuCoremTobj $CoreRemObjTem
			fi
			
			if [ $(($CUSTOM_SETTING_MASK&0x04)) != 0 ];then	
				getini $CONFIGFILE Custom_OutletObjTem
				OutletObjTem=$parameter
				#如果OutletObjTem目标值为空，则校验默认值50
				if [ "$OutletObjTem" == "" ] ;then	
					OutletObjTem=50
				fi
				#校验配置
				check_Customized_OutletTobj $OutletObjTem	
			fi

		fi
		#如果调速级别参数不为空，则校验客户配置值
		if [ "$THERMAL" != "" ] ; then	
			THERMAL=`printf "%02x" $THERMAL`	
			check_thermal_mode $THERMAL
		#否则校验默认值		
		else
			#当单板支持SmartCooling模式时，风冷默认节能模式（0x10）, 液冷默认液冷模式(0x14)
			if [ "$SMARTCOOLING_ENABLE" == 1 ];then
				if [ "$COOLING_MEDIUM_LIQUID" == 1 ];then
					check_thermal_mode 14
				else
					check_thermal_mode 10
				fi
			#不支持SmartCooling模式时，默认BIOS低功耗模式（0）
			else
				check_thermal_mode 00
			sleep 1
			fi
		fi	 
	fi	
    
    #6.1 PCIe拓扑配置检查
    if [ $PRODUCT_ID == $PRODUCT_ATLAS_TYPE ] ; then	
		if [ $BOARD_ID == $BOARD_G560_ID ] || [ $BOARD_ID == $BOARD_G560V5_ID ] || [ $BOARD_ID == $BOARD_G530V5_ID ] ; then
            
            #GP308不支持拓扑切换，需要排除DTS2018071305732
            if [ $GPU_BOARD_ID != $GPU_BOARD_GP308_ID ] ; then
                getini $CONFIGFILE BMCSet_TopoMode
                pcie_topo_mode=$parameter
            
                if [ "$parameter" == ""	]; then
                    get_default_pcie_topo_mode
                    pcie_topo_mode=$?
                fi
	    
                check_pcie_topo_mode $pcie_topo_mode
            fi
        fi
    fi

	####DISK SLOT Tencent定制化配置检查########################################################################
    echo "PRODUCT_VERSION_NUM=$PRODUCT_VERSION_NUM"
    if [ "$PRODUCT_VERSION_NUM" != "06" ];then
	getini $CONFIGFILE  DiskSlotCustomize
	DiskSlot_Customize=$parameter
	getini $CONFIGFILE  DiskSlotCustomizeId
	DiskSlot_CustomizeId=$parameter
	if [ "$DiskSlot_Customize" = "on" ] ; then
	
			if [ "$DiskSlot_CustomizeId" == "Tencent" ] ;then
				check_set_diskslot_customize_id 01
			else
				check_set_diskslot_customize_id 00
			fi
	else
		check_set_diskslot_customize_id 00
	fi
    fi
	#检查IPMI设置启动模式选项显示定制
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE BMCSet_BootModeSwEnable
	BootModeSwEnable=$parameter
	if [ "$BootModeSwEnable" == "off" ] ; then
		check_boot_mode_sw_enable 0
	else
		check_boot_mode_sw_enable 1
		fi
	fi
	#ipmitool elsit命令设置支持查询iBMC的SEL条数支持可配置定制:1~2000条(默认2000)
	getini $CONFIGFILE BMCSet_QuerySELMaxNumValue
	QuerySELMaxNumValue=$parameter
	if [ "$QuerySELMaxNumValue" != "" ] ; then
		check_query_sel_max_num_value $QuerySELMaxNumValue
	else
		check_query_sel_max_num_value 2000
	fi
	#检查定制化的BMC默认证书加密算法
	getini $CONFIGFILE BMCSet_CertAlgorithm
	CertAlgorithm=$parameter
	if [ "$CertAlgorithm" != "" ] ; then
		check_cert_algorithm $CertAlgorithm
	else
		check_cert_algorithm 0
	fi

	#只有V5类型的单板才会进入如下定IPMI消息过滤总开关以及黑白名单模式定制化配置
	if [ "$PRODUCT_VERSION_NUM" != "03" ];then	
		#定制化IPMI消息过滤总开关的状态：0：关闭; 1:开启
		getini $CONFIGFILE BMCSet_FireWallStatus
		FireWallStatus=$parameter
		if [ "$FireWallStatus" != "" ] ; then
			check_ipmi_fire_wall_status $FireWallStatus
		else
			check_ipmi_fire_wall_status 1
		fi
		#定制化IPMI消息过滤总开关开启时，设置黑白名单模式的状态：0：黑名单; 1:白名单; 默认0：黑名单
		#定制化IPMI消息过滤总开关关闭时，设置黑白名单模式为无效状态：0xff
		getini $CONFIGFILE BMCSet_FireWallMode
		FireWallMode=$parameter
		if [ "$FireWallMode" != "" ] ; then
			check_ipmi_fire_wall_mode $FireWallMode
		else
			check_ipmi_fire_wall_mode 0
		fi
		sleep 1	
	fi
	
	#检查sysname从hostname同步的标志
	getini $CONFIGFILE BMCSet_SysNameSyncHostName
	QuerySysNameSyncHostName=$parameter
	if [ "$QuerySysNameSyncHostName" != "" ] ; then
		check_sysname_sync_from_hostname_flag $QuerySysNameSyncHostName
	else
		check_sysname_sync_from_hostname_flag 0
	fi

	#校验sysname信息
	getsysnameini $CONFIGFILE BMCSet_CustomSysName
	CustomSysName=$parameter
	if [ "$CustomSysName" != "" ] ; then
		check_customized_sysname "$CustomSysName"
	else
		getsysnameini $CONFIGFILE BMCSet_Cooperation_CustomSysName
		Cooperation_QueryCustomSysName=$parameter
		if [ "$Cooperation_QueryCustomSysName" != "" ] ; then
			check_customized_sysname "$Cooperation_QueryCustomSysName"
		else
			check_customized_sysname "iBMC"
		fi
	fi

	#检查定制化的IErr诊断失败时处理策略
	getini $CONFIGFILE BMCSet_FDMIErrWarmResetEnable
	FDMDiagFailPolicy=$parameter
	if [ "$FDMDiagFailPolicy" != "" ] ; then
		check_fdm_diag_fail_policy $FDMDiagFailPolicy
	else
		check_fdm_diag_fail_policy 1
	fi

	#检查定制化的同步开关
	getini $CONFIGFILE BMCSet_ComSysHostnameSyncEnable
	hostSyncEnable=$parameter
	if [ "$hostSyncEnable" != "" ] ; then
		check_sync_hostname_enable $hostSyncEnable
	else
		check_sync_hostname_enable 1
	fi
	getini $CONFIGFILE BMCSet_EventDetailCustomizedID
	EventDetailCustomizedID=$parameter
	if [ "$EventDetailCustomizedID" != "" ] ; then
		check_rf_eventdetail_customized_id $EventDetailCustomizedID
	else
		check_rf_eventdetail_customized_id 0
	fi
	#检查IPMI定制化厂商ID
	getini $CONFIGFILE BMCSet_IpmiCustomManuID
	CustomManuID=$parameter
	if [ "$CustomManuID" == "" ] ; then
		check_custom_manu_id 2011
	elif [ "$CustomManuID" -gt "16777215" -o "$CustomManuID" -lt "0" ] ; then
	    echo_fail "check ipmi custom manu id parameter out of range! ID value(0~16777215)"
		echo "check ipmi custom manu id parameter out of range! ID value(0~16777215)" >>$SAVEFILE
		error_quit
    else
		check_custom_manu_id $CustomManuID
	fi

	#检查按钮定制状态,只有TaiShan的单板才会进入按钮(长按或短按)屏蔽定制化配置
	if [ "$PRODUCT_VERSION_NUM" != "03" ] && [ $is_x86 = 0 ];then
		#获取按钮定制状态：0:不屏蔽; 1:屏蔽短按;
		getini $CONFIGFILE BMCSet_PowerButtonLockStatus
		PowerButtonLockStatus=$parameter
		if [ "$PowerButtonLockStatus" != "" ] ; then
			check_power_button_lock_status $PowerButtonLockStatus
		else
			check_power_button_lock_status 0
		fi
	fi
	# 检查USB管理服务使能状态
	PRODUCT_NAME_ID=$PRODUCT_ID$BOARD_ID
	if [ "$PRODUCT_VERSION_NUM" == "06" ] ; then
		if [ "$PRODUCT_NAME_ID" == "$PRODUCT_2488HV6_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_2288HV6_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_1288HV6_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_TAISHAN2280V2_1711_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_TIANCHI_ID" ] ; then
            if [ "$PRODUCT_UNIQUE_ID" != "$PRODUCT_DA122C_UNIQUEID" ] ; then
			    getini $CONFIGFILE BMCSet_USBMgmtState
			    USBMgmtState=$parameter
			    if [ "$USBMgmtState" == "" ] || [ "$USBMgmtState" == "on" ] ; then
				    check_usb_mgmt_service_state 1
			    else
				    check_usb_mgmt_service_state 0
			    fi
            fi
		fi
	fi
	#检查系统软件名称
	getini $CONFIGFILE BMCSet_SmsName
	SmsName=$parameter
	if [ "$SmsName" == "" ] ; then
		getini $CONFIGFILE BMCSet_Cooperation_SmsName
		Cooperation_SmsName=$parameter
		if [ "$Cooperation_SmsName" != "" ] ; then
			SmsName="$Cooperation_SmsName"
		else
			SmsName="iBMA"
		fi
	fi
	check_smsname "$SmsName"

	# 检查AC上电通电开机场景风扇默认速率比
	getini $CONFIGFILE BMCSet_FanInitDefaultLevel
	FanInitDefaultLevel=$parameter
	if [ "$FanInitDefaultLevel" == "" ] ; then
		check_fan_init_default_level 100
	elif [ $FanInitDefaultLevel -ge 50 -a $FanInitDefaultLevel -le 100 ] ; then
		check_fan_init_default_level $FanInitDefaultLevel
	else
		echo_fail "Check the default initial fan speed ratio($FanInitDefaultLevel) fail! The number must range from 50 to 100."
		error_quit
	fi

	# 检查SP设备信息收集功能开关状态
	getini $CONFIGFILE BMCSet_SPDeviceInfoCollectEnable
	SPDeviceInfoCollectEnable=$parameter
	if [ "$PRODUCT_VERSION_NUM" == "06" ] && ([ $PRODUCT_ID == $PRODUCT_RACK_TYPE ] || [ $PRODUCT_ID == $PRODUCT_XSERIALS_TYPE ] || [ $PRODUCT_ID = $PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE ] || [ $PRODUCT_ID == $PRODUCT_OSCA_TYPE ]) ; then
		# V6 X86，X系列，E9000系列空定制化值为开启
		if [ "$SPDeviceInfoCollectEnable" == "" ] || [ "$SPDeviceInfoCollectEnable" == "on" ] ; then
			check_sp_device_info_collect_enable 1
		elif [ "$SPDeviceInfoCollectEnable" == "off" ] ; then
		    check_sp_device_info_collect_enable 0
		else
		    echo_fail "check SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable"
		    echo "check SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable" >>$SAVEFILE
		    error_quit
		fi
	else
		# 其他产品空定制化值为关闭
		if [ "$SPDeviceInfoCollectEnable" == "" ] || [ "$SPDeviceInfoCollectEnable" == "off" ] ; then
			check_sp_device_info_collect_enable 0
		elif [ "$SPDeviceInfoCollectEnable" == "on" ] ; then
		    check_sp_device_info_collect_enable 1
		else
		    echo_fail "check SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable"
		    echo "check SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable" >>$SAVEFILE
		    error_quit
		fi
	fi

	#SKUNumber定制化
	getini $CONFIGFILE  BMCSet_SmBiosSKUNumber
	SKUNumber=$parameter
	if [ "$SKUNumber" != "" ] ; then
		check_smbios_skunumber "$SKUNumber"
	else
		check_smbios_skunumber ""
	fi

	#检查移动配置模型定制化
	getini $CONFIGFILE  BMCSet_ComSysConfigurationModel
	ConfigurationModel=$parameter
	if [ "$ConfigurationModel" != "" ] ; then
		check_configuration_model "$ConfigurationModel"
	else
		check_configuration_model "N/A"
	fi
	#检查移动版本信息定制化
	getini $CONFIGFILE  BMCSet_ComSysCmccVersion
	CmccVersion=$parameter
	if [ "$CmccVersion" != "" ] ; then
		check_cmcc_version "$CmccVersion"
	else
		check_cmcc_version "N/A"
	fi

	#检查redfish定制化厂商信息
	getini $CONFIGFILE BMCSet_RedfishCustomManuName
	CustomManufacturerName=$parameter
	if [ "$CustomManufacturerName" != "" ] ; then
		check_redfish_custom_manu_name "$CustomManufacturerName"
	else
		check_redfish_custom_manu_name "Huawei"
	fi
}

main
