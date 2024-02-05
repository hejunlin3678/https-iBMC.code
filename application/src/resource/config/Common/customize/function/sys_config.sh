#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#设置时区，时区可以为GMT+xx:xx或GMT-xx:xx格式（范围：GMT-12:00~GMT+14:00），或者UTC+xx:xx或UTC-xx:xx格式（范围：UTC-12:00~UTC+14:00）或者城市名（例如：Asia/Shanghai，America/New_York），或者时间偏移480，-240等（范围：[-720,840]）
set_bmc_timezone()
{
	if [ "$1" == "" ] ; then
		time_zone="UTC"
		strlen=3
	else
		time_zone=$1
		strlen=`echo ${#time_zone}`
		if [ $strlen -gt 32 ] ; then
			echo_fail "Length($strlen) of time zone name is too big!"
			echo "Length($strlen) of time zone name is too big!" >> $SAVEFILE
			error_quit
		fi
		
		time_zone=$(string2hex "$time_zone")
	fi
	
	fun_id=13
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 9)
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$time_zone
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set time zone $1 fail!"
       echo "set time zone $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set time zone $1 success!"
       return 0
    fi
}

#设置black_box
set_blackbox_on()
{
    run_cmd "f 0 30 93 DB 07 00 36 3e 0 1 10 0 0 ff ff 0 1 0 2 0" "0f 31 00 93 00 db 07 00 00 02 00 79 01 00"
    if [ $? == 0 ] ; then
        return 0		
    fi
	run_cmd "f 0 30 93 DB 07 00 0F 0C 01 00 00 01 01" "0f 31 00 93 00 db 07 00"
   
	if [ $? != 0 ] ; then		
	    echo_fail "Set blackbox on fail!"
	    echo "Set blackbox on fail!" >>$SAVEFILE
	    error_quit
	else 
	    echo_success "set blackbox on success!"
	    echo "set blackbox on success!" >>$SAVEFILE
	    return 0
	fi
}
set_blackbox_off()
{
    run_cmd "f 0 30 93 DB 07 00 36 3e 0 1 10 0 0 ff ff 0 1 0 2 0" "0f 31 00 93 00 db 07 00 00 02 00 79 01 00"
    if [ $? == 0 ] ; then
        return 0
	fi 
	
	run_cmd "f 0 30 93 DB 07 00 0F 0C 01 00 00 01 00" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		#关闭blackbox时，判断是否命令不支持，不支持继续往下运行
		run_cmd "f 0 30 93 DB 07 00 0F 0C 01 00 00 01 00" "0f 31 00 93 80 db 07 00"
		if [ $? == 0 ] ; then
			echo "Not support blackbox !">>$SAVEFILE
			return 0
		fi
	    echo_fail "Set blackbox off fail!"
	    echo "Set blackbox off fail!" >>$SAVEFILE
	    error_quit
	else 
	    echo_success "set blackbox off success!"
	    echo "set blackbox off success!" >>$SAVEFILE
	    return 0
	fi  
}
#设置trap模式
set_bmc_trap_mode()
{
	TrapMode=$1
	run_cmd "f 0 30 93 db 07 00 35 43 00 01 00 00 00 ff ff 00 01 00 03 00 00 01 $TrapMode" "0f 31 00 93 00 db 07 00"
   
	if [ $? != 0 ] ; then		
	    echo_fail "Set trap mode $TrapMode fail!"
	    echo "Set trap mode $TrapMode fail!" >>$SAVEFILE	    
        error_quit
	else
	    echo_success "Set trap mode $TrapMode success!"
	    echo "Set trap mode $TrapMode success!" >>$SAVEFILE
	    return 0	 
	fi
}

#设置trap模式定制
set_trap_mode_custom()
{
	local TrapModeCustom=$1
	if [ "$TrapModeCustom" != 0 ] && [ "$TrapModeCustom" != 1 ] ; then
		echo_and_save_fail "Invalid trap mode custom type($TrapModeCustom)"
		error_quit
	fi
	local value=`printf "%02x" $TrapModeCustom`
	run_cmd "f 0 30 93 DB 07 00 15 00 16 00 00 01 $value" "0f 31 00 93 00 db 07 00"

	if [ $? != 0 ] ; then
		echo_and_save_fail "Set trap mode custom $TrapModeCustom fail!"
		error_quit
	else
		echo_and_save_success "Set trap mode custom $TrapModeCustom success!"
		return 0
	fi
}

#设置trap的IP地址(ipmi命令不支持)
set_bmc_trap_dest_ipaddr()
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
	

	#set in progress
	#run_cmd "f 0 c 1 1 0 1" "0f 0d 00 01 00"
	show_id=`expr $1 + 1`	
	fun_id=4
	key_id=$(word2string 1) 
	key_type=79
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 3) 
	info_type=73
	info_val=$loadip
	info_len=`printf "%02x" $strlen`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
    if [ $? != 0 ] ; then
       	echo_fail "Set trap $show_id addr to $2 fail"
		echo "Set trap $show_id addr to $2 fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Set trap $show_id addr to $2 success"
		echo "Set trap $show_id addr to $2 success" >>$SAVEFILE
   	fi
}

#设置trap的端口号(ipmi命令不支持)
set_bmc_trap_dest_port()
{
	show_id=`expr $1 + 1`
	port=$2
	fun_id=4
	key_id=$(word2string 1) 
	key_type=79
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 4) 
	info_type=75
	info_len=4
	 # START AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127
	info_val=$(dword2string $port)
	 # END AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set trap $show_id dest port to $2 fail!"
       echo "set trap $show_id dest port to $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set trap $show_id dest port to $2 success!"
       return 0
    fi
}

#设置trap使能状态
set_trap_enable()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "set Trap service state $1 fail!"
		error_quit
	else
		echo_and_save_success "set Trap service state $1 success!"
		return 0
	fi
}

#设置trap版本
set_trap_version()
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
		echo_and_save_fail "Invalid trap verision($1)"
		error_quit
	fi

	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "set Trap version $1 fail!"
		error_quit
	else
		echo_and_save_success "set Trap version $1 success!"
		return 0
	fi
}

#设置trap SNMPv3用户
set_trap_snmpv3_user_id()
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

	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Set Trap SNMPv3 user $1 fail!"
		error_quit
	else
		echo_and_save_success "set Trap SNMPv3 user $1 success!"
		return 0
	fi
}

#设置Trap团体名()
set_trap_community_name()
{
	pwd_str=$1
	pwd_info=$(string2hex_with_prefix "$pwd_str")

	#整个字符串长度
	strlen=`echo ${#pwd_str}`
	#密码长度为1-32
	if [ $strlen -gt 32 ] || [ $strlen -lt 1 ]; then
		echo_and_save_fail "Length($strlen) of Trap password is too big!"
		error_quit
	fi
	info_len=`printf "%02x" $strlen`
	run_ipmitool_without_echo "raw 0x0C 0x01 0x01 0x10 $pwd_info"
	if [ $? != 0 ] ; then
		echo_and_save_fail "set Trap community name $1 fail!"
		error_quit
	else
		echo_and_save_success "set Trap community name $1 success!"
		return 0
	fi
}

#设置trap使能状态
set_bmc_trap_on()
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
		#设置trap启用
		set_bmc_trap_enable_flag $TRAP_ITEM_ID 1
		set_bmc_trap_dest_ipaddr $TRAP_ITEM_ID $TRAP_IP_ADDR
		set_bmc_trap_dest_port   $TRAP_ITEM_ID $TRAP_PORT
	fi
}

#设置trap的启用状态
set_bmc_trap_enable_flag()
{
	show_id=`expr $1 + 1`
	fun_id=4
	key_id=$(word2string 1) 
	key_type=79
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 2) 
	info_type=79
	info_len=1
	info_val=`printf "%02x" $2`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set trap $show_id enable state $2 fail!"
       echo "set trap $show_id enable state $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set trap $show_id enable state $2 success!"
       return 0
    fi
}

#设置trap去使能状态
set_bmc_trap_off()
{
	TRAP_ITEM_ID=`expr $1 - 1`
	#禁止使能
	set_bmc_trap_enable_flag $TRAP_ITEM_ID 0
	#设置默认ip
	set_bmc_trap_dest_ipaddr $TRAP_ITEM_ID ""
	#设置默认port
	set_bmc_trap_dest_port $TRAP_ITEM_ID 162
}

#设置trap告警发送级别
set_bmc_trap_alarm_severity()
{
	local TrapSeverity=$1
	if [ "$TrapSeverity" != 0 ] && [ "$TrapSeverity" != 8 ] && [ "$TrapSeverity" != 12 ] && [ "$TrapSeverity" != 14 ] && [ "$TrapSeverity" != 15 ] ; then
		echo_and_save_fail "Invalid trap severity type($TrapSeverity)"
		error_quit
	fi
	local value=`printf "%02x" $TrapSeverity`
	run_cmd "f 0 30 93 DB 07 00 15 00 17 00 00 01 $value" "0f 31 00 93 00 db 07 00"

	if [ $? != 0 ] ; then
		echo_and_save_fail "Set Trap severity $TrapSeverity fail!"
		error_quit
	else
		echo_and_save_success "Set Trap severity $TrapSeverity success!"
		return 0
	fi
}

#操作定制化版权等信息的公共接口
set_about_info()
{
	about_type=$1
	about_str=$2
	
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		return 1	
   	else
		return 0
   	fi
}

#操作定制化VNC端口使能状态
set_vnc_enable_state()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=01
	info_id=$(word2string 1)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set VNC service state $1 fail!"
       echo "set VNC service state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set VNC service state $1 success!"
       return 0
    fi
}

#定制化web界面JavaKvm入口显示
set_javakvm_visible_state()
{	
	run_cmd "f 0 30 93 DB 07 00 5a 0e 00 00 00 $1" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
       echo_fail "set JavaKvm visible state $1 fail!"
       echo "set JavaKvm visible state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set JavaKvm visible state $1 success!"
       return 0
    fi
}

#操作定制化KVM超时时间
set_kvm_timeout()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set KVM timeout fail!"
       echo "set KVM timeout $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set KVM timeout $1 success!"
       return 0
    fi
}

#操作定制化VNC密码
set_vnc_password()
{
	pwd_str=$1
	if [ "$pwd_str" != "" ] ; then
		pwd_info=$(string2hex "$pwd_str")
		#整个字符串长度
		strlen=`echo ${#pwd_str}`
		#密码长度为1-8
		if [ $strlen -gt 8 ] || [ $strlen -lt 1 ]; then
			echo_and_save_fail "Length($strlen) of VNC password is too big!"
			error_quit
		fi
	else
		pwd_info=00
		strlen=1
	fi
	info_len=`printf "%02x" $strlen`
	run_cmd "0f 00 30 94 DB 07 00 67 00 01 00 00 00 $info_len $pwd_info" "0f 31 00 94 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_and_save_fail "set VNC password fail!"
		error_quit
	else
		echo_and_save_success "set VNC password $1 success!"
		return 0
	fi
}

#操作定制化VNC超时时间
set_vnc_timeout()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set VNC timeout fail!"
       echo "set VNC timeout $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set VNC timeout $1 success!"
       return 0
    fi
}

#操作定制化VNC SSL使能状态
set_vnc_ssl_state()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=01
	info_id=$(word2string 4)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set VNC SSL state $1 fail!"
       echo "set VNC SSL state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set VNC SSL state $1 success!"
       return 0
    fi
}


#操作定制化VNC监听端口号
set_vnc_port()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set VNC port fail!"
       echo "set VNC port $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set VNC port $1 success!"
       return 0
    fi
}

#操作定制化VNC键盘布局
set_vnc_keyboard_layout()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set VNC keyboard layout fail!"
       echo "set VNC keyboard layout $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set VNC keyboard layout $1 success!"
       return 0
    fi	
}

#操作定制化VNC登录规则
set_vnc_permit_rule()
{
	fun_id=102
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=01
	info_id=$(word2string 7)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set VNC PermitRuleIds $1 fail!"
       echo "set VNC PermitRuleIds $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set VNC PermitRuleIds $1 success!"
       return 0
    fi
}


#操作定制化语言显示信息的公共接口
set_about_languageSet()
{
	about_type=$1
	about_str=$2
	#所有语言集合
	all_language_set="en,ar,be,bg,ca,cs,da,de,el,es,et,fi,fr,hr,hu,is,it,iw,ja,ko,lt,lv,mk,nl,no,pl,pt,ro,ru,sk,sl,sq,sr,sv,th,tr,uk,zh"
	if [ "$about_str" != "" ] ; then
		OLD_IFS="$IFS"
		IFS=","
		arr=($about_str)
		IFS="$OLD_IFS"
		for s in ${arr[@]}
		do
			if [[ $all_language_set =~ $s ]]
			then
				continue
			else
			echo "$s is not a member of the all_language_set"
			return 1
			fi
		done
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		return 1	
   	else
		return 0
   	fi
}


#设置语言集合属性值
set_language_info()
{
	local languageSet_str=$1
	
	set_about_languageSet 8 "$languageSet_str"	
	if [ $? != 0 ] ; then
      	echo_fail "set languageSet to $1 fail"
		echo "set languageSet to $1 fail" >>$SAVEFILE
		error_quit		
    else
		echo_success "set languageSet to $1 success"
		echo "set languageSet to $1 success" >>$SAVEFILE
   	fi	
}


#设置版权信息
set_copyright_info()
{
	local copyright_str=$1
	
	set_about_info 3 "$copyright_str"	
	if [ $? != 0 ] ; then
       	echo_fail "set copyright to $1 fail"
		echo "set copyright to $1 fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "set copyright to $1 success"
		echo "set copyright to $1 success" >>$SAVEFILE
   	fi	
}

#设置support地址信息
set_officalweb_info()
{
	local officalweb_str=$1
	
	set_about_info 1 "$officalweb_str"	
	if [ $? != 0 ] ; then
       	echo_fail "set offical web to $1 fail"
		echo "set offical web to $1 fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "set offical web to $1 success"
		echo "set offical web to $1 success" >>$SAVEFILE
   	fi	
}

#设置自动发现状态  0 disable; 1 enable
set_autodiscovery_state()
{
	fun_id=17
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len=01
	key_val=01
	info_id=$(word2string 1) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		echo_fail "set_autodiscovery_enable $1 Fail"
		echo "set_autodiscovery_enable $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set_autodiscovery_enable $1 success!"
		echo "set_autodiscovery_enable $1 success!" >>$SAVEFILE
	fi
}

#设置自动发现广播网段 255.255.255.255/192.168.255.255等
set_autodiscovery_broadcast()
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
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		echo_fail "set_autodiscovery_broadcast $BROADCAST_INFO Fail"
		echo "set_autodiscovery_broadcast $BROADCAST_INFO Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set_autodiscovery_broadcast $BROADCAST_INFO success!"
		echo "set_autodiscovery_broadcast $BROADCAST_INFO success!" >>$SAVEFILE
	fi
}

#设置自动发现广播端口，默认26957
set_autodiscovery_port()
{
	fun_id=17
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 3) 
	info_type=75 #u
	info_len="04"
	info_val=$(dword2string $1)
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		echo_fail "set_autodiscovery_port $1 Fail"
		echo "set_autodiscovery_port $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set_autodiscovery_port $1 success!"
		echo "set_autodiscovery_port $1 success!" >>$SAVEFILE
	fi
}

# 设置diagnose模块数据采集服务状态
set_data_acquisition_service_status()
{
	# 不支持数据采集的产品没有相应的对象，所以设置前先判断对象是否存在
	command="f 0 30 93 db 07 00 36 72 00 01 ff 00 00 ff ff 79 01 00 01 00"
	result="0f 31 00 93 d3"
	run_cmd "$command" "$result"
	if [ $? == 0 ] ; then
		echo_success "Set data acquisition service status success!"
		echo "Set data acquisition service status success!" >>$SAVEFILE
		return 0   # 查询到设备不支持数据采集，返回成功
	fi

	# 关闭数据采集功能
	if [ 0 == $1 ] 
	then
		# 当前默认已经关闭，为将属性持久化，需要先开启，再关闭
		command="f 0 30 93 db 07 00 35 72 00 01 00 00 00 ff ff 79 01 00 01 00 79 01 01"
		result="0f 31 00 93 00 db 07 00"
		run_cmd "$command" "$result"
		if [ $? != 0 ] ; then
			echo_fail "Set data acquisition service status $1 failed!"
			echo "Set data acquisition service status $1 failed!" >>$SAVEFILE
			error_quit
		fi

		command="f 0 30 93 db 07 00 35 72 00 01 00 00 00 ff ff 79 01 00 01 00 79 01 00"
		result="0f 31 00 93 00 db 07 00"
		run_cmd "$command" "$result"
		if [ $? != 0 ] ; then
			echo_fail "Set data acquisition service status $1 failed!"
			echo "Set data acquisition service status $1 failed!" >>$SAVEFILE
			error_quit
		else
			echo_success "Set data acquisition service status $1 success!"
			echo "Set data acquisition service status $1 success!" >>$SAVEFILE
			return 0
		fi
	# 开启数据采集功能
	else
		command="f 0 30 93 db 07 00 35 72 00 01 00 00 00 ff ff 79 01 00 01 00 79 01 01"
		result="0f 31 00 93 00 db 07 00"
		run_cmd "$command" "$result"
		if [ $? != 0 ] ; then
			echo_fail "Set data acquisition service status $1 failed!"
			echo "Set data acquisition service status $1 failed!" >>$SAVEFILE
			error_quit
		else
			echo_success "Set data acquisition service status $1 success!"
			echo "Set data acquisition service status $1 success!" >>$SAVEFILE
			return 0
		fi
	fi
}

#设置机器型号
set_server_tag()
{
	servertag=$1
	fun_id=18
	key_id=$(word2string 1) 
	key_type=79
	key_len=1
	key_val=`printf "%02x" 1`
	info_id=$(word2string 3) 
	info_type=75
	info_len=4
	info_val=$(dword2string $servertag)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       		echo_fail "set Server tag to $1 fail!"
       		echo "set Server tag to $1 fail!" >>$SAVEFILE
       		error_quit
    	else 
       		echo_success "set Server tag to $1 success!"
       		return 0
    	fi
}



#设置web超时时间
set_web_timeout()
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
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		echo_fail "set_web_timeout $1 Fail"
		echo "set_web_timeout $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set_web_timeout $1 success!"
		echo "set_web_timeout $1 success!" >>$SAVEFILE
	fi
}

#设置web会话模式
set_web_session_mode()
{
	fun_id=51
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 2) 
	info_type=79
	info_len=1
	info_val=`printf "%02x" $1`

	if [ $1 == 0 ]; then
		mode_str='share'
	else
		mode_str='monopoly'
	fi

	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"

	if [ $? != 0 ] ; then
		echo_fail "set web session mode to $mode_str Fail"
		echo "set web session mode to $mode_str Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set web session mode to $mode_str success!"
		echo "set web session mode to $mode_str success!" >>$SAVEFILE
	fi
}

#设置syslog的总体启用状态,00 关闭,01 开启
set_bmc_syslog_general_enable_flag()
{
	checkInt $1
	if [ $? != 0 ] ; then
		echo "set syslog general enable state fail! (0-1)"
		error_quit
	fi
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
	info_len=1
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog general enable state $1 fail!"
       echo "set syslog general enable state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog general enable state $1 success!"
       return 0
    fi
}

#设置syslog的主机标识前需要确保msgformat是自定义模式
recover_bmc_syslog_msgformat() 
{
	fun_id=63
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 7)
	info_type=75
	info_len=1
	info_val=0
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_and_save_fail "recover bmc syslog msgformat fail!"
       error_quit
    else 
       echo_success "recover bmc syslog msgformat successfully!"
       return 0
    fi
}

#设置syslog的主机标识, 01单板序列号,02资产标签,03主机名
set_bmc_syslog_host_flag()
{
	checkInt $1
	if [ $? != 0 ] ; then
		echo "set syslog host flag fail! (1-3)"
		error_quit
	fi
	if [ "$1" -gt "3" -o "$1" -lt "1" ] ; then
		echo_fail "set syslog host flag fail! (1-3)"
		error_quit
	fi
	recover_bmc_syslog_msgformat
	fun_id=63
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 2)
	info_type=75
	info_len=4
	info_val=$(dword2string $1)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog host flag $1 fail!"
       echo "set syslog host flag $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog host flag $1 success!"
       return 0
    fi
}

#设置syslog的告警级别, 00反选所有,01正常,02轻微,04严重,08紧急
set_bmc_syslog_alarm_level()
{
	checkInt $1
	if [ $? != 0 ] ; then
		echo "set syslog alarm level fail! (0-15)"
		error_quit
	fi
	if [ "$1" -gt "15" -o "$1" -lt "0" ] ; then
		echo_fail "set syslog alarm level fail! (0-15)"
		error_quit
	fi
	fun_id=63
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 3)
	info_type=75
	info_len=4
	info_val=$(dword2string $1)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog alarm level $1 fail!"
       echo "set syslog alarm level $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog alarm level $1 success!"
       return 0
    fi
}

#设置syslog的传输协议, 01 UDP,02 TCP,03 TLS
set_bmc_syslog_trans_protocol()
{
	checkInt $1
	if [ $? != 0 ] ; then
		echo "set syslog trans protocol fail! (1-3)"
		error_quit
	fi
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
	info_len=1
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog transport protocol $1 fail!"
       echo "set syslog transport protocol $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog transport protocol $1 success!"
       return 0
    fi
}

#设置syslog单项的使能状态,id 0-3
set_bmc_syslog_enable_state()
{
	show_id=`expr $1 + 1`
	checkInt $2
	if [ $? != 0 ] ; then
		echo "set syslog $show_id enable state fail! (0-1)"
		error_quit
	fi
	if [ "$2" -gt "1" -o "$2" -lt "0" ] ; then
		echo_fail "set syslog $show_id enable state fail! (0-1)"
		error_quit
	fi
	fun_id=64
	key_id=$(word2string 1) 
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 2) 
	info_type=79 #y
	info_len=1
	info_val=`printf "%02x" $2`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog $show_id enable state to $2 fail!"
       echo "set syslog $show_id enable state to $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog $show_id enable state to $2 success!"
       return 0
    fi
}

#设置syslog单项的日志类型,id 0-3,1操作日志,2安全日志,4事件日志,7所有日志
set_bmc_syslog_log_type()
{
	show_id=`expr $1 + 1`
	checkInt $2
	if [ $? != 0 ] ; then
		echo "set syslog $show_id log type fail! (1-7)"
		error_quit
	fi
	if [ "$2" -gt "7" -o "$2" -lt "1" ] ; then
		echo_fail "set syslog $show_id log type fail! (1-7)"
		error_quit
	fi
	fun_id=64
	key_id=$(word2string 1) 
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 3) 
	info_type=75 #u
	info_len=4
	info_val=$(dword2string $2)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog $show_id log type to $2 fail!"
       echo "set syslog $show_id log type to $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog $show_id log type to $2 success!"
       return 0
    fi
}

#设置syslog单项的服务器地址
set_bmc_syslog_hostaddr()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog host addr fail!"
       echo "set syslog host addr fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog host addr success!"
       return 0
    fi
}


#设置syslog单项的端口号,id 0-3
set_bmc_syslog_dest_port()
{
	show_id=`expr $1 + 1`
	checkInt $2
	if [ $? != 0 ] ; then
		echo "set syslog $show_id dest port to $2 fail! portvalue(0~65535)"
		error_quit
	fi
	if [ "$2" -gt "65535" -o "$2" -lt "0" ] ; then
		echo_fail "set syslog $show_id dest port to $2 fail! portvalue(0~65535)"
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
	info_len=4
	info_val=$(dword2string $port)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set syslog $show_id dest port to $2 fail!"
       echo "set syslog $show_id dest port to $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set syslog $show_id dest port to $2 success!"
       return 0
    fi
}

#设置syslog总体定制项目入口
set_bmc_syslog_general_status()
{
	getini $CONFIGFILE BMCSet_CustomSyslog_State
	SYSLOG_GE_ENABLE_STATE=$parameter
	if [ "$SYSLOG_GE_ENABLE_STATE" == "" ] ; then
			SYSLOG_GE_ENABLE_STATE=0
	fi
	set_bmc_syslog_general_enable_flag $SYSLOG_GE_ENABLE_STATE
	
	getini $CONFIGFILE BMCSet_CustomSyslog_Host_Flag
	SYSLOG_HOST_FLAG=$parameter
	if [ "$SYSLOG_HOST_FLAG" == "" ] ; then
			SYSLOG_HOST_FLAG=1
	fi
	set_bmc_syslog_host_flag $SYSLOG_HOST_FLAG

    getini $CONFIGFILE BMCSet_CustomSyslog_Alarm_Level
	SYSLOG_ALARM_LEVEl=$parameter
	if [ "$SYSLOG_ALARM_LEVEl" == "" ] ; then
			SYSLOG_ALARM_LEVEl=15
	fi
	set_bmc_syslog_alarm_level $SYSLOG_ALARM_LEVEl
	
	getini $CONFIGFILE BMCSet_CustomSyslog_Trans_Protocol
	SYSLOG_TRANS_PROTOCOL=$parameter
	if [ "$SYSLOG_TRANS_PROTOCOL" == "" ] ; then
			SYSLOG_TRANS_PROTOCOL=3
	fi
	set_bmc_syslog_trans_protocol $SYSLOG_TRANS_PROTOCOL
}

#设置syslog单项定制项目入口
set_bmc_syslog_status()
{
	getini $CONFIGFILE BMCSet_CustomSyslog_${1}_State
	SYSLOG_ENABLE_STATE=$parameter
	if [ "$SYSLOG_ENABLE_STATE" == "" ] ; then
			SYSLOG_ENABLE_STATE=0
	fi
	set_bmc_syslog_enable_state $1 $SYSLOG_ENABLE_STATE
	
	getini $CONFIGFILE BMCSet_CustomSyslog_${1}_LogType
	SYSLOG_LOG_TYPE=$parameter
	if [ "$SYSLOG_LOG_TYPE" == "" ] ; then
			SYSLOG_LOG_TYPE=7
	fi
	set_bmc_syslog_log_type $1 $SYSLOG_LOG_TYPE

    getini $CONFIGFILE BMCSet_CustomSyslog_${1}_IpAddr
	SYSLOG_IP_ADDR=$parameter
	if [ "$SYSLOG_IP_ADDR" == "" ] ; then
			SYSLOG_IP_ADDR=""
	fi
	set_bmc_syslog_hostaddr $1 $SYSLOG_IP_ADDR
	
	getini $CONFIGFILE BMCSet_CustomSyslog_${1}_Port
	SYSLOG_PORT=$parameter
	if [ "$SYSLOG_PORT" == "" ] ; then
			SYSLOG_PORT=0
	fi
	set_bmc_syslog_dest_port $1 $SYSLOG_PORT
}


#设置传感器号动态分配时的起始号
set_bmc_dynamic_sensor_num_base()
{	
	fun_id=68
	key_id=$(word2string 0xffff) 
	key_type=0
	key_len="01"
	key_val="00"
	info_id=$(word2string 1) 
	info_type=0
	info_len=1
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set dynamic sensor num base $1 fail!"
       echo "set dynamic sensor num base $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set dynamic sensor num base $1 success!"
       return 0
    fi
}

set_Single_Node_Alarm()
{
	alarm_policy=`printf "0x%02x" $1`
	if [ $alarm_policy != 0x00 ] && [ $alarm_policy != 0x01 ]; then
		echo_fail "set single node alarm policy $SingleNodeAlarm fail! parameter out of range."
		echo "set single node alarm policy $SingleNodeAlarm fail!parameter out of range." >> $SAVEFILE
		error_quit
	fi
	echo "ipmitool raw 0x30 0x93 0xdb 0x07 0x00 0x35 0x13 0x00 0x01 0x00 0x00 0x00 0xff 0xff 0x00 0x01 0x00 0x0c 0x00 0x79 0x01 $alarm_policy" >>$IPMI_CMD_LOG_FILE
	#命令如果发送不成功则重试，重试间隔1秒，重试5次仍然不成功则复位bmc，最多复位3次，复位3次不成功则返回失败
	#复位重试3次，防止sendmsg转发失败和mm板复位命令转发不过去的场景
	for ((reset_times=0;reset_times<3;reset_times++))
	do
		set_alarm_policy_fail_times=0
		for((i=0;i<5;i++))
		do
			ipmitool raw 0x30 0x93 0xdb 0x07 0x00  0x35 0x13 0x00 0x01 0x00 0x00 0x00 0xff 0xff 0x00 0x01 0x00 0x0c 0x00 0x79 0x01  $alarm_policy > $IPMI_CMD_RETURN_FILE 2>&1
			cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
			grep -wq "db 07 00" $IPMI_CMD_RETURN_FILE
			#命令返回成功
			if [ $? = 0 ] ; then
				
				echo_success "set single node alarm policy $SingleNodeAlarm success!"
				echo "set single node alarm policy $SingleNodeAlarm success!" >> $SAVEFILE
				return 0
			fi
			#如果命令发送超时
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
					((set_alarm_policy_fail_times=set_alarm_policy_fail_times+1));
					echo "set_alarm_policy_fail_times:$set_alarm_policy_fail_times" 
				fi
				#连续3次设置告警策略命令发送返回0xff或0xc3，但是06 01可以发送成功，返回成功，打印设置失败
				if [ $set_alarm_policy_fail_times = 3 ] ;then
				{
					echo_fail "set single node alarm policy $SingleNodeAlarm fail!"
					echo "set single node alarm policy $SingleNodeAlarm fail!" >> $SAVEFILE
					return 0
				}
				fi
			#非c3和ff的其他错误码都设置失败，但是不会返回错误退出
			else
				echo_fail "set single node alarm policy $SingleNodeAlarm fail!"
				echo "set single node alarm policy $SingleNodeAlarm fail!" >> $SAVEFILE
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
	echo_fail "set single node alarm policy $SingleNodeAlarm fail!"
    echo "set single node alarm policy $SingleNodeAlarm fail!" >>$SAVEFILE
	error_quit
	fi
	

	

}

#设置启动项定制
set_boot_device()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set Boot device $1 fail!"
       echo "set Boot device $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set Boot device $1 success!"
       return 0
    fi
}

#设置启动生效模式定制
set_boot_effect_mode()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set Boot effect mode $1 fail!"
       echo "set Boot effect mode $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set Boot effect mode $1 success!"
       return 0
    fi
}

#设置启动模式切换功能的启用状态，00 关闭，01开启
set_boot_mode_changeover_status()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 4)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
	   echo_fail "set_boot_mode_changeover_status $1 fail!"
       echo "set_boot_mode_changeover_status $1 fail!" >>$SAVEFILE
	   error_quit
	else
       echo_success "set_boot_mode_changeover_status $1 success!"
	   return 0
	fi
}

#设置启动模式， 00 Legacy BIOS， 01 Unified Extensible Firmware Interface (UEFI)
set_boot_mode()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 5)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
	   echo_fail "set_boot_mode $1 fail!"
       echo "set_boot_mode $1 fail!" >>$SAVEFILE
	   error_quit
	else
       echo_success "set_boot_mode $1 success!"
	   return 0
	fi
}


#设置WOL的启用状态,00 关闭,01 开启
set_wake_on_lan_state()
{
	fun_id=48
	key_id=$(word2string 0xffff)
	key_type=00 #y
	key_len=01
	key_val=01
	info_id=$(word2string 5)
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set WOL state $1 fail!"
       echo "set WOL state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set WOL state $1 success!"
       return 0
    fi
}

#定制化Thermal Policy模式
set_thermal_mode()
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

	    echo_fail "Set thermal policy mode: $mode fail!"
            echo "Set thermal policy mode: $mode fail!" >>$SAVEFILE
            error_quit
	fi
		
	run_cmd "f 0 30 92 DB 07 00 13 01 $1" "0f 31 00 92 00 db 07 00" 
	if [ $? != 0 ] ; then
        echo_fail "Set thermal policy mode: $mode fail!"
        echo "Set thermal policy mode: $mode fail!" >>$SAVEFILE
        error_quit
    else
        echo_success "Set thermal policy mode: $mode success!"
        echo "Set thermal policy mode: $mode success!" >>$SAVEFILE
        return 0
   fi
}
set_Customized_InletTemp_Speed()
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

	run_cmd "f 0 30 93 DB 07 00 45 $InletTempSpeed" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
        echo_fail "Set Customized_InletTemp_Speed $InletTempSpeed fail!"
        echo "Set Customized_InletTemp_Speed $InletTempSpeed fail!" >>$SAVEFILE
        error_quit
    else
        echo_success "Set Customized_InletTemp_Speed $InletTempSpeed success!"
        echo "Set Customized_InletTemp_Speed $InletTempSpeed success!" >>$SAVEFILE
        return 0
   fi
}
set_Customized_CpuCoremTobj()
{
	fun_id=90
	key_id=$(word2string 0xffff)
	key_type=00 #y
	key_len=01
	key_val=00
	info_id=$(word2string 6)
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set Customized_CpuCoremTobj $1 fail!"
       echo "set Customized_CpuCoremTobj $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set Customized_CpuCoremTobj $1 success!"
       return 0
    fi
}

set_Customized_OutletTobj()
{
	fun_id=90
	key_id=$(word2string 0xffff)
	key_type=00 #y
	key_len=01
	key_val=00
	info_id=$(word2string 10)
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set Customized_CpuCoremTobj $1 fail!"
       echo "set Customized_OutletTobj $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set Customized_OutletTobj $1 success!"
       return 0
    fi
}


#AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑:获取拓扑配置状态
get_pcie_topo_config_status()
{
	fun_id=106					 #十进制的106表示编排类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 			     #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=$(word2string 4)     #属性Id为4表示默认拓扑配置
	key_string="$key_id $key_type $key_len $key_val"
	get_prop_cmd $fun_id "$key_string" $info_id
	if [ $? != 0 ] ; then
        topo_config_status=$((16#`echo $GET_PROP_INF_STRING|awk -F' ' '{print $NF}'`))   #取最后一个字符
       	return $topo_config_status
    else 
       	echo_fail "get default pcie topo config status fail!"
        echo "get default pcie topo config status fail!" >>$SAVEFILE
       	error_quit
    fi
}

#AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑:下发并查询拓扑配置结果
set_pcie_topo_mode()
{
    local topo_mode=$1
    
	fun_id=106
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2)
	info_type=73          #字符类型
	info_len=1
	info_val=$(word2string $topo_mode)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set topo mode to $topo_mode fail!"
       echo "set topo mode to $topo_mode fail!" >>$SAVEFILE
       error_quit
    fi
    
    local RETRY_COUNT=25   #软件上默认超过20秒视为超时，此处获取额外增加5秒
    for((i=0;i<$RETRY_COUNT;i++))
    do
        get_pcie_topo_config_status
        if [ $? = 2 ] ; then  #2表示拓扑完成
            echo_success "set topo mode to $topo_mode successfully!"
            echo "set topo mode to $topo_mode successfully!" >>$SAVEFILE
            return 0
		fi
        
        sleep 1
    done
    
    echo_fail "set topo mode to $topo_mode fail!"
    echo "set topo mode to $topo_mode fail!" >>$SAVEFILE
    
    error_quit
}

#配置Disk Slot定制化
set_diskslot_customize_id()
{
	customize_id=$1;
	set_prop_cmd 113 1 "ff ff 00 01 00 01 00 00 01 $customize_id"
    if [ $? != 0 ] ; then
        echo_fail "Set disk slot customize id to $customize_id fail!"
        echo "Set disk slot customize id to $customize_id fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "Set disk slot customize id to $customize_id success!"
        echo "Set disk slot customize id to $customize_id success!" >>$SAVEFILE
        return 0
    fi
	
}

#IPMI设置启动模式选项是否显示，0:不显示,1:显示(默认)
set_boot_mode_sw_enable()
{
	fun_id=82
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 10)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set Boot mode switch enable $1 fail!"
		echo "set Boot mode switch enable $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set Boot mode switch enable $1 success!"
		return 0
	fi
}

#设置sysname从客户定制化的配置文件中
set_customized_sysname()
{
	local max_strlen=64
	local min_strlen=0

	strlen=`echo ${#1}`
	strlen_02x=`printf %02x $strlen`
	if [ "$strlen" -le "$max_strlen" ] && [ "$strlen" -ge "$min_strlen" ]; then
		string2sysnamesubstring "$1" $max_strlen
		TMP_STRING=$(remove_space "$TMP_STRING")
		TMP_STRING=`echo $TMP_STRING > tmp_sysname_cfg.txt`
		sed -i 's/^/0x/g' tmp_sysname_cfg.txt
		sed -i 's/ / 0x/g' tmp_sysname_cfg.txt
		TMP_STRING=`cat tmp_sysname_cfg.txt`
		strlen_02x="0x$strlen_02x"
		ipmitool raw 0x30 0x93 0xdb 0x07 0x00 0x5a 0x10 0x00 0x00 ${strlen_02x} ${TMP_STRING}
		if [ $? != 0 ]; then
			echo_fail "set custom sysname fail"
			rm -rf tmp_sysname_cfg.txt
			echo "set custom sysname fail" >>$SAVEFILE
			error_quit
		fi
		
		echo_success "set custom sysname success"
		rm -rf tmp_sysname_cfg.txt
		echo "set custom sysname success" >>$SAVEFILE
	else 
		echo_fail "set custom sysname len longer than 64byte"
		echo "set custom sysname len longer than 64byte" >>$SAVEFILE
		error_quit
	fi
	
}

#设置是否同步BMA的hostname信息到BMC，0:不同步,1:同步(默认)
set_sync_hostname_enable()
{
	fun_id=142
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1)
	info_type=00
	info_len=01
	is_digit "$1"
	if [ $? != 0 ]; then
		echo_and_save_fail "set sync hostname enable $1 fail! valid value(0~1)"
		error_quit
	fi
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set sync hostname enable $1 fail!"         
		echo "set sync hostname enable $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set sync hostname enable $1 success!"
		return 0
	fi
}
#ipmitool elsit命令设置支持查询iBMC的SEL条数支持可配置定制:1~2000条(默认2000)
set_query_sel_max_num_value()
{
	fun_id=132
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=01
	info_id=$(word2string 1)
	info_type=00 #q的ascii码,16进制
	info_len=02
	info_val=$(word2string $1)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set SEL max num value $1 fail!"
		echo "set Sel max num value $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set Sel max num value $1 success!"
		return 0
	fi
}
#定制化BMC默认的证书加密算法 0:RSA 1:ECC 默认0
set_cert_algorithm()
{
	fun_id=13
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 10)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set certificate algorithm $1 fail!"
		echo "set certificate algorithm $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set certificate algorithm $1 success!"
		echo "set certificate algorithm $1 success!" >>$SAVEFILE
		return 0
	fi
}

#设置IPMI消息过滤总开关的状态：0：关闭(0ff); 1:开启(on); 
set_ipmi_fire_wall_status()
{
	fun_id=134
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set ipmi fire wall status $1 fail!"
		echo "set ipmi fire wall status $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set ipmi fire wall status $1 success!"
		return 0
	fi
}

#IPMI消息过滤总开关开启时，设置黑白名单模式的状态：0：黑名单; 1:白名单; 
#IPMI消息过滤总开关关闭时，设置黑白名单模式为无效状态：0xff
set_ipmi_fire_wall_mode()
{
	fun_id=134
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set ipmi fire wall mode $1 fail!"
		echo "set ipmi fire wall mode $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set ipmi fire wall mode $1 success!"
		return 0
	fi
}

#设置SysName同步HostName，显示为HostName.iBMC：0：关闭(0ff); 1:开启(on); 
set_sysname_sync_from_hostname_flag()
{
	fun_id=1
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 14)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set sysname synchronize from hostname flag $1 fail!"
		echo "set sysname synchronize from hostname flag $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set sysname synchronize from hostname flag $1 success!"
		return 0
	fi
}

#定制化IErr诊断失败时处理策略 0:不做处理 1:热复位业务侧 2：诊断后下电
set_fdm_diag_fail_policy()
{
	fun_id=54
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 4)
	info_type=00
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set FDM IErr warm reset state $1 fail!"
		echo "set FDM IErr warm reset state $1 fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set FDM IErr warm reset state $1 success!"
		echo "set FDM IErr warm reset state $1 success!" >>$SAVEFILE
		return 0
	fi
}

#设置redfish事件上报显示事件详细字段的定制化ID, 0: 定制化信息都不显示 1：显示CMCC定制字段
set_rf_eventdetail_customized_id()
{
	fun_id=99
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 3)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set redfish event detail customized id $1 fail!"
       echo "set redfish event detail customized id $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set redfish event detail customized id $1 success!"
       echo "set redfish event detail customized id $1 success!" >>$SAVEFILE
	   return 0
    fi
}

#设置按钮定制状态
set_power_button_lock_status()
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
		echo_fail "set_power_button_lock_status: para $1 is out of range!"
		echo "set_power_button_lock_status: para $1 is out of range!" >>$SAVEFILE
		error_quit
	fi	
	
	run_cmd "f 0 30 93 DB 07 00 5a 0a 00 03 00 00 00 $short_lock" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set_power_button_short_lock_status: $short_lock fail!"
		echo "set_power_button_short_lock_status: $short_lock  fail!" >>$SAVEFILE
		error_quit
	fi
	
	echo_success "set_power_button_lock_status: $short_lock success!"
	echo "set_power_button_lock_status: $short_lock success!" >>$SAVEFILE 
	return 0
	
}

#设置定制化厂商ID
set_custom_manu_id()
{
    local LONGB0=`printf "%08x" $1 | cut -b 7-8`
    local LONGB1=`printf "%08x" $1 | cut -b 5-6`
    local LONGB2=`printf "%08x" $1 | cut -b 3-4`
    run_cmd  "f 0 30 90 21 08 $LONGB0 $LONGB1 $LONGB2" "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo_fail "set custom manu id fail!"
        echo "set custom manu id fail!" >>$SAVEFILE      
	    error_quit		
    else 
        echo_success "set custom manu id $LONGB0 $LONGB1 $LONGB2 success!"
        echo "set custom manu id $LONGB0 $LONGB1 $LONGB2 success!" >>$SAVEFILE
        return 0
    fi
}

#设置系统软件名称
set_smsname()
{
	strlen=`echo ${#1}`
	strlen_02x=`printf %02x $strlen`
	string2substring "$1"
	TMP_STRING=$(remove_space "$TMP_STRING")
	run_cmd "f 0 30 93 db 07 00 5a 1e 00 $strlen_02x $TMP_STRING" "0f 31 00 93 00 db 07 00"
	if [ $? -eq 0 ] ; then
		echo_success "set SmsName:$1 success"
		echo "set SmsName:$1 success" >>$SAVEFILE
		return 0
	else
		echo_fail "set SmsName:$1 fail!"
		echo "set SmsName:$1 fail" >>$SAVEFILE
		echo_fail "Please check the validity of SmsName, it must be letters or digits, length must be 1~8"
		echo "Please check the validity of SmsName, it must be letters or digits, length must be 1~8" >>$SAVEFILE
		error_quit
	fi
}

# 设置AC上电通电开机场景风扇默认速率比  范围 50-100
set_fan_init_default_level()
{
	level=`printf %02x $1`
	run_cmd "f 0 30 93 DB 07 00 5a 1f 00 01 00 $level" "0f 31 00 93 00 db 07 00"
	if [ $? == 0 ] ; then
		echo_success "Set the default initial fan speed ratio to 0x$level success!"
		echo "Set the default initial fan speed ratio to 0x$level success!" >>$SAVEFILE
		return 0
	else
		echo_fail "Set the default initial fan speed ratio to 0x$level fail!"
		echo "Set the default initial fan speed ratio to 0x$level fail!" >>$SAVEFILE
		error_quit
	fi
}

getsysnameini()
{

    FILENAME=$1
    KEYWORD=$2
	
    tmp_param=`cat $FILENAME | grep "^${KEYWORD}\>"`
    parameter=`echo "${tmp_param#*=}"`

}
#判断并分割字符串
string2sysnamesubstring()
{
	STRING=$1
	local max_string_len=$2
	if [ "$max_string_len" = "" ] ; then
		max_string_len=48
	fi
	echo "$STRING" > temp.txt
	#对于STRING的最大长度为48位做判断
	if [ "${#STRING}" -gt $max_string_len ] ; then
		echo_fail "Surpasses the greatest length limit $max_string_len! write e_label $STRING failed!"
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

# 设置USB管理服务使能状态定制化
set_usb_mgmt_service_state()
{
	service_state=$1;
	run_cmd "f 00 30 93 db 07 00 5a 11 00 01 00 $service_state" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "Set USB management service state to $service_state fail!"
		echo "Set USB management service state to $service_state fail!" >>$SAVEFILE
		error_quit   
	else 
		echo_success "Set USB management service state to $service_state success!"
		echo "Set USB management service state to $service_state success!" >>$SAVEFILE
		return 0
	fi
}

# 设置SP设备信息收集功能开关状态
set_sp_device_info_collect_enable()
{
	service_state=$1;
	run_cmd "f 00 30 93 db 07 00 5a 24 00 01 00 $service_state" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "Set SP device info collect enable state to $service_state fail!"
		echo "Set SP device info collect enable state to $service_state fail!" >>$SAVEFILE
		error_quit   
	else 
		echo_success "Set SP device info collect enable state to $service_state success!"
		echo "Set SP device info collect enable state to $service_state success!" >>$SAVEFILE
		return 0
	fi
}

#设置SKUNumber
set_smbios_skunumber()
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

	#SKUNumber长度小于48
	if [ $strlen -gt 48 ]; then
		echo_and_save_fail "Length($strlen) of SKUNumber is too big!"
		error_quit
	fi

	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Set SmBios SKUNumber $1 fail!"
		error_quit
	else
		echo_and_save_success "Set SmBios SKUNumber $1 success!"
		return 0
	fi
}


#移动配置模型定制化
set_configuration_model()
{
	model_str=$1
	is_have_space "$model_str"
	if [ $? == 0 ] ; then
		echo_and_save_fail "The configuration model($1) contains spaces"
		error_quit
	fi
	model_info=$(string2hex "$model_str" | sed 's/^/0x/g' | sed 's/ / 0x/g')

	#整个字符串长度
	strlen=`echo ${#model_str}`
	if [ $strlen -gt 128 ] || [ $strlen -lt 1 ]; then
		echo_fail "Length($strlen) of configuration model is out of range!"
		echo "Length($strlen) of configuration model is out of range!" >> $SAVEFILE
		error_quit
	fi
	info_len=`printf "0x%02x" $strlen`

	run_ipmitool "raw 0x30 0x93 0xDB 0x07 0x00 0x5a 0x21 0x00 $info_len 0x00 $model_info" "db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set configuration model $1 fail!"
		echo "set configuration model $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "set configuration model $1 success!"
		return 0
	fi
}

#移动版本信息定制化
set_cmcc_version()
{
	version_str=$1
	is_have_space "$version_str"
	if [ $? == 0 ] ; then
		echo_and_save_fail "The cmcc version($1) contains spaces"
		error_quit
	fi
	version_info=$(string2hex "$version_str" | sed 's/^/0x/g' | sed 's/ / 0x/g')
	#整个字符串长度
	strlen=`echo ${#version_str}`
	if [ $strlen -gt 128 ] || [ $strlen -lt 1 ]; then
		echo_fail "Length($strlen) of cmcc version is out of range!"
		echo "Length($strlen) of cmcc version is out of range!" >> $SAVEFILE
		error_quit
	fi
	info_len=`printf "0x%02x" $strlen`
	run_ipmitool "raw 0x30 0x93 0xDB 0x07 0x00 0x5a 0x22 0x00 $info_len 0x00 $version_info" "db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set cmcc version $1 fail!"
		echo "set cmcc version $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "set cmcc version $1 success!"
		return 0
	fi
}


#设置redfish定制化厂商信息
set_redfish_custom_manu_name()
{
	manu_str=$1
	is_have_space "$manu_str"
	if [ $? == 0 ] ; then
		echo_and_save_fail "The custom manufacturer name($1) contains spaces"
		error_quit
	fi
	manu_info=$(string2hex "$manu_str")
	echo "$manu_info" > tmp_custom_manu.txt
	sed -i 's/^/0x/g' tmp_custom_manu.txt
	sed -i 's/ / 0x/g' tmp_custom_manu.txt
	manu_info=`cat tmp_custom_manu.txt`

	#整个字符串长度
	strlen=`echo ${#manu_str}`

	#厂商长度为1-128
	if [ $strlen -gt 128 ] || [ $strlen -lt 1 ]; then
		echo_fail "Length($strlen) of manufacturer is too big!"
		echo "Length($strlen) of manufacturer is too big!" >> $SAVEFILE
		error_quit
	fi

	info_len=`printf "0x%02x" $strlen`
	run_ipmitool "raw 0x30 0x93 0xDB 0x07 0x00 0x5a 0x23 0x00 $info_len 0x00 $manu_info" "db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set custom manufacturer name $1 fail!"
		echo "set custom manufacturer name $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "set custom manufacturer name $1 success!"
		return 0
	fi
}

main()
{
	local is_x86=0
	
	check_is_x86
	is_x86=$?
	
	#3.2时区定制化  //E6000不支持时区定制
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE  Custom_TIMEZONE
		TIMEZONE=$parameter
		if [ "$TIMEZONE" == "" ] ; then
			set_bmc_timezone 0
		else
			set_bmc_timezone "$TIMEZONE"
		fi
	fi
	
	#3.3开启Blackbox, V3产品默认关闭；V5产品默认打开
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
		set_blackbox_on
	else
		set_blackbox_off   
	fi
	sleep 1
	
	#Trap使能
	getini $CONFIGFILE  BMCSet_TrapEnable
	TrapEnable=$parameter
	if [ "$TrapEnable" != "" ] ; then
		if [ "$TrapEnable" == "on" ] ; then
			set_trap_enable 1
		elif [ "$TrapEnable" == "off" ]; then
			set_trap_enable 0
		else
			echo_and_save_fail "set Trap service state $TrapEnable fail!"
			error_quit
		fi
	else
		set_trap_enable 1
	fi
	#Trap版本
	getini $CONFIGFILE  BMCSet_TrapVersion
	TrapVersion=$parameter
	if [ "$TrapVersion" != "" ] ; then
		set_trap_version $TrapVersion
	else
		set_trap_version 0
	fi
	#Trap SNMPv3用户
	getini $CONFIGFILE  BMCSet_TrapSNMPv3UserID
	TrapSNMPv3UserID=$parameter
	if [ "$TrapSNMPv3UserID" != "" ] ; then
		set_trap_snmpv3_user_id $TrapSNMPv3UserID
	else
		set_trap_snmpv3_user_id 2
	fi
	#Trap团体名
	getini $CONFIGFILE  BMCSet_TrapCommunityName
	TrapCommunityName=$parameter
	if [ "$TrapCommunityName" != "" ] ; then
		set_trap_community_name "$TrapCommunityName"
	elif [ "$TrapVersion" != "3" ] ; then
		set_trap_community_name "TrapAdmin12#$"
	else
		echo_and_save_success "skip Trap community name set with TrapVersion=$TrapVersion"
	fi

	#3.4 TRAP功能使能 
	IDValue=0
	while [ "$IDValue" -lt "4" ]
	do 
		IDValue=`expr $IDValue + 1`
		getini $CONFIGFILE  BMCSet_TRAP_${IDValue}_Enable
		TRAPEnable=$parameter

		if [ "$TRAPEnable" = "on" ] ; then
			set_bmc_trap_on $IDValue
		else
			set_bmc_trap_off $IDValue
		fi
		sleep 1
	done
	
	#Trap告警发送级别
	getini $CONFIGFILE  BMCSet_TrapAlarmSeverity
	TrapSeverity=$parameter
	if [ "$TrapSeverity" != "" ] ; then
		set_bmc_trap_alarm_severity $TrapSeverity
	else
		set_bmc_trap_alarm_severity 15
	fi
	
	#自动发现定制化: E9000 不支持
	#使能: E9000 不支持
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE BMCSet_AutoDiscoveryEnable
		AutoDiscoveryEnable=$parameter
		
		if [ "$AutoDiscoveryEnable" = "1" ] ; then
			set_autodiscovery_state 1
			echo "set auto discovery state enable success"
		else
			set_autodiscovery_state 0
			echo "set auto discovery state disable success"
		fi
	fi
	
	#广播地址: E9000 不支持
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE BMCSet_AutoDiscoveryBroadcast
		AutoDiscoveryBroadcast=$parameter
		
		if [ "$AutoDiscoveryBroadcast" != "" ] ; then
			set_autodiscovery_broadcast "$AutoDiscoveryBroadcast"
		else
			set_autodiscovery_broadcast "255.255.255.255"
		fi
	fi
	
	#广播端口: E9000 不支持
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE BMCSet_AutoDiscoveryPort
		AutoDiscoveryPort=$parameter
		
		if [ "$AutoDiscoveryPort" != "" ] ; then
			set_autodiscovery_port "$AutoDiscoveryPort"
		else
			set_autodiscovery_port "26957"
		fi
	fi
	
	#定制化版权信息
	getini $CONFIGFILE BMCSet_Copyright
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

	set_copyright_info "$Copyright"
	
	#定制化语言集合信息
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		getini $CONFIGFILE BMCSet_LanguageSet
		languageSet=$parameter
		if [ "$languageSet" == "" ] ; then
			languageSet="en,zh,ja,fr,ru"		
		fi
		set_language_info "$languageSet"
	fi
	
	#定制化web界面JavaKvm入口显示
	getini $CONFIGFILE BMCSet_VisibleJavaKVM
	VisibleJavaKVM=$parameter
	if [ "$VisibleJavaKVM" != "" ] ; then
		if [ "$VisibleJavaKVM" = "on" ] ; then
			set_javakvm_visible_state 1
		elif [ "$VisibleJavaKVM" = "off" ] ; then
			set_javakvm_visible_state 0
		else
			echo_fail "set JavaKvm visible state $VisibleJavaKVM fail!"
			echo "set JavaKvm visible state $VisibleJavaKVM fail!" >>$SAVEFILE
			error_quit
		fi 
	else
		set_javakvm_visible_state 1
	fi
	
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
	set_officalweb_info "$OfficalWeb"
	sleep 1
	
	##配置KVM超时时间
	getini $CONFIGFILE  BMCSet_KVMTimeout
	KVMTimeout=$parameter
	if [ "$KVMTimeout" != "" ] ; then
		set_kvm_timeout $KVMTimeout
	else
		set_kvm_timeout 60
	fi
	
	#VNC服务V3默认关闭，V5默认打开
	if [ "$PRODUCT_VERSION_NUM" != "03" ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	#定制化VNC配置信息
	##配置VNC端口使能
	getini $CONFIGFILE BMCSet_VNCEnableState
	VNCEnableState=$parameter
	if [ "$VNCEnableState" != "" ] ; then
		if [ "$VNCEnableState" = "on" ] ; then
			set_vnc_enable_state 1
		else
			set_vnc_enable_state 0
		fi 
	else
		set_vnc_enable_state 0
	fi

	##配置VNC密码
	getini $CONFIGFILE  BMCSet_VNCPassword
	VNCPassword=$parameter
	set_vnc_password "$VNCPassword"

	##配置VNC超时时间
	getini $CONFIGFILE  BMCSet_VNCTimeout
	VNCTimeout=$parameter
	if [ "$VNCTimeout" != "" ] ; then
		set_vnc_timeout $VNCTimeout
	else
		set_vnc_timeout 60
	fi
	##配置VNC SSL使能状态
	getini $CONFIGFILE BMCSet_VNCSSLEnableState
	VNCSSLEnableState=$parameter
	if [ "$VNCSSLEnableState" != "" ] ; then
		if [ "$VNCSSLEnableState" = "on" ] ; then
			set_vnc_ssl_state 1
		else
			set_vnc_ssl_state 0
		fi 
	else
		set_vnc_ssl_state 1
	fi	
	##配置VNC端口号
	getini $CONFIGFILE BMCSet_VNCPort
	VNCPort=$parameter
	if [ "$VNCPort" != "" ] ; then
		set_vnc_port $VNCPort
	else
		set_vnc_port 5900
	fi
	##配置VNC键盘布局
	getini $CONFIGFILE BMCSet_VNCKeyboardLayout
	VNCKeyboardLayout=$parameter
	if [ "$VNCKeyboardLayout" != "" ] ; then
		set_vnc_keyboard_layout $VNCKeyboardLayout
	else
		set_vnc_keyboard_layout "jp"
	fi
	##配置VNC登录规则
	getini $CONFIGFILE BMCSet_VNCPermitRuleIds
	VNCPermitRuleIds=$parameter
	if [ "$VNCPermitRuleIds" != "" ] ; then
		set_vnc_permit_rule $VNCPermitRuleIds
	else
		set_vnc_permit_rule 0
	fi
	fi	
	
	# 配置diagnose模块数据采集功能
	getini $CONFIGFILE BMCSet_DataAcquisitionServiceStatus
	DataAcquisitionServiceStatus=$parameter
	if [ "$DataAcquisitionServiceStatus" == "on" ] ; then
		set_data_acquisition_service_status 1
	else
		set_data_acquisition_service_status 0
	fi
	
    if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ]; then
	#定制化机器型号
	getini $CONFIGFILE  BMCSet_ServerTag
	ServerTag=$parameter
	if [ "$ServerTag" != "" ] ; then
		set_server_tag $ServerTag   
	fi
	fi
	
	#web超时时间
	getini $CONFIGFILE BMCSet_SessionTimeout
	SessionTimeout=$parameter

	if [ "$SessionTimeout" = "" ] ; then
		set_web_timeout 300
		echo "restore web timeout 5 minutes success"
	else
		second=`expr $SessionTimeout \* 60`
		set_web_timeout $second
		echo "restore web timeout $SessionTimeout minutes success"
	fi

	#web会话模式
	getini $CONFIGFILE BMCSet_WebSessionMode
	WebSessionMode=$parameter

	if [ "$WebSessionMode" == "share" ] ; then
		set_web_session_mode 0
	else
		set_web_session_mode 1
	fi

	#syslog功能定制
	set_bmc_syslog_general_status
	IDValue=0
	while [ "$IDValue" -lt "4" ]
	do 
		set_bmc_syslog_status $IDValue
		IDValue=`expr $IDValue + 1`
		sleep 1
	done
	
	# 传感器号动态分配时的起始号的定制
	# 这个值给BMC定制了传感器后生效（有SensorOemConfig类对象）
	getini $CONFIGFILE BMCSet_CustomDynamicSensorNumBase
	DYNAMIC_SENSOR_NUM_BASE=$parameter
	if [ "$DYNAMIC_SENSOR_NUM_BASE" != "" ] ; then
		set_bmc_dynamic_sensor_num_base $DYNAMIC_SENSOR_NUM_BASE
	else
		set_bmc_dynamic_sensor_num_base 1
	fi
	
	#只有V5类型的单板才会进入如下Boot定制化配置
	if [ "$PRODUCT_VERSION_NUM" != "03" ] && [ $is_x86 = 1 ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ];then
		#设置启动模式切换功能的启用状态，默认情况打开
		getini $CONFIGFILE BMCSet_BootModeChangeoverStatus
		BootModeChangeoverStatus=$parameter
		if [ "$BootModeChangeoverStatus" == "off" ] ; then
			set_boot_mode_changeover_status 0
		else
			set_boot_mode_changeover_status 1
		fi
		
		#设置启动模式，默认情况为UEFI模式
		getini $CONFIGFILE BMCSet_BootMode
		BootMode=$parameter
		if [ "$BootMode" != "" ] ; then
			set_boot_mode $BootMode
		else
			set_boot_mode 1
		fi
	fi
	
	#设置启动项定制
	getini $CONFIGFILE BMCSet_BootDevice
	BootDevice=$parameter
	if [ "$BootDevice" == "" ] ; then
		set_boot_device 0
	else
		set_boot_device $BootDevice
	fi
	
	#设置启动生效模式定制
	getini $CONFIGFILE BMCSet_BootEffectMode
	BootEffectMode=$parameter
	if [ "$BootEffectMode" == "" ] ; then
		set_boot_effect_mode 1
	else
		set_boot_effect_mode $BootEffectMode
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
			set_Single_Node_Alarm $SingleNodeAlarm
		fi
	elif [ $PRODUCT_ID == $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID != $BOARD_ATLAS880_ID ] && [ $BOARD_ID != $BOARD_ATLAS800_9010_ID ] && [ $BOARD_ID != $BOARD_G2500_ID ] ; then
		getini $CONFIGFILE BMCSet_SingleNodeAlarm
		SingleNodeAlarm=$parameter
		#如果无此定制化项则默认值0
		if [ "$SingleNodeAlarm" == "" ] ;then
			SingleNodeAlarm=0
		fi
		set_Single_Node_Alarm $SingleNodeAlarm	
	fi
	
	#只有OSCA部分刀片支持WOL功能，因此先判断该产品是否支持WOL功能，再进行定制化 DTS2018032601574
	get_wol_config_enable_state
	wol_config_enable=$? 
	if [ "$wol_config_enable" == 1 ];then
		#设置WOL的启用状态,00 关闭,01 开启
		getini $CONFIGFILE BMCSet_WOLState
		WOLState=$parameter

		if [ "$WOLState" == "on" ] ; then
			set_wake_on_lan_state 1
		else
			set_wake_on_lan_state 0
		fi
	fi
    
	#定制化trap模式, V3产品0-事件码模式；V5产品默认2-精准告警模式	
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
	set_bmc_trap_mode $TrapMode
	
	#设置trap模式定制，默认：0，腾讯定制：1
	getini $CONFIGFILE  BMCSet_TrapModeCustom
	BMCSet_TrapModeCustom=$parameter
	if [ "$BMCSet_TrapModeCustom" != "" ] ; then
		set_trap_mode_custom $BMCSet_TrapModeCustom
	else
		set_trap_mode_custom 0
	fi

	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		#定制化Thermal Policy
		getini $CONFIGFILE BMCSet_Thermal_Policy
		THERMAL=$parameter
		#获取当前单板是否支持SmartCooling功能
		get_SmartCooling_Enable_state
		SMARTCOOLING_ENABLE=$? 
		#获取当前单板处于支持液冷模式
		get_Cooling_Medium_Liquid
		COOLING_MEDIUM_LIQUID=$?
		if [ "$THERMAL" != "" ];then
			#当单板支持SmartCooling模式时
			THERMAL=`printf "%02x" $THERMAL`

			if [ "$SMARTCOOLING_ENABLE" == 1 ];then		
				if [ "$THERMAL" != 10 ] &&  [ "$THERMAL" != 11 ] && [ "$THERMAL" != 12 ] && [ "$THERMAL" != 13 ] && [ "$THERMAL" != 14 ] && [ "$THERMAL" != 00 ] && [ "$THERMAL" != 01 ] && [ "$THERMAL" != 02 ];then 
					echo_fail "Set thermal policy parameter "$THERMAL" error!"
					echo "Set thermal policy parameter "$THERMAL" error!" >>$SAVEFILE
									error_quit
				else
					set_thermal_mode $THERMAL
					
				fi
			#当单板不支持SmartCooling模式时,只支持设置BIOS高中低模式
			else
				if [ "$THERMAL" != 00 ] && [ "$THERMAL" != 01 ] && [ "$THERMAL" != 02 ];then
					echo_fail "Set thermal policy parameter $THERMAL error!"
					echo "Set thermal policy parameter $THERMAL error!" >>$SAVEFILE
					error_quit
				else
					set_thermal_mode $THERMAL
				fi
			fi
				
		else
			#支持SmartCooling的单板，风冷默认还原成节能模式，液冷默认还原成液冷模式, 不支持SmartCooling的单板默认还原成BIOS低功耗模式
				if [ "$SMARTCOOLING_ENABLE" == 1 ];then
					if [ "$COOLING_MEDIUM_LIQUID" == 1 ];then
						#液冷默认还原成液冷模式
						set_thermal_mode 14
					else
						#恢复节能模式
						set_thermal_mode 10
					fi				
										
			else
				#恢复低功耗模式
				set_thermal_mode 00
			fi
		fi

		#如果单板支持SmartCooling功能
		if [ "$SMARTCOOLING_ENABLE" == 1 ];then	
		    
			get_SmartCooling_Custom_Setting_Mask
			CUSTOM_SETTING_MASK=$CustomSettingMask
			
			if [ $(($CUSTOM_SETTING_MASK&0x01)) != 0 ];then
			    getini $CONFIGFILE Custom_Inlet_Temp_Level
			    Inlet_Temp_Level=$parameter
			    #定制进风口温度调速
			    set_Customized_InletTemp_Speed $Inlet_Temp_Level
			fi
			
			if [ $(($CUSTOM_SETTING_MASK&0x02)) != 0 ];then			
				#定制CpuCoremTobj调速，ARM CPU无Tjmax概念，无需换算，直接65度。
				getini $CONFIGFILE Custom_CoreRemObjTem
				CoreRemObjTem=$parameter
				if [ "$CoreRemObjTem" == "" ] ;then
					if [ $is_x86 = 1 ];then
						get_CpuCoreRemObjTem
						CoreRemObjTem=$?
					else
						CoreRemObjTem=65					
					fi
				fi
				set_Customized_CpuCoremTobj $CoreRemObjTem	
			fi

			if [ $(($CUSTOM_SETTING_MASK&0x04)) != 0 ];then			
			#定制出风口调速
				getini $CONFIGFILE Custom_OutletObjTem
				OutletObjTem=$parameter
				if [ "$OutletObjTem" == "" ] ;then
					OutletObjTem=50
				fi
				set_Customized_OutletTobj $OutletObjTem	
			fi				
			
		fi
	fi
    
    #6.1 设置拓扑模式
    if [ $PRODUCT_ID == $PRODUCT_ATLAS_TYPE ] ; then	
		if [ $BOARD_ID == $BOARD_G560_ID ] || [ $BOARD_ID == $BOARD_G560V5_ID ] || [ $BOARD_ID == $BOARD_G530V5_ID ] ; then
            
            #GP308不支持拓扑切换，需要排除 DTS2018060707573
            if [ $GPU_BOARD_ID != $GPU_BOARD_GP308_ID ] ; then
                getini $CONFIGFILE BMCSet_TopoMode
                pcie_topo_mode=$parameter
                #获取模式为空，获取默认拓扑模式
                if [ "$pcie_topo_mode" == "" ] ; then
                    get_default_pcie_topo_mode
                    pcie_topo_mode=$?
                fi
                set_pcie_topo_mode $pcie_topo_mode
            fi
        fi
    fi
	
	####DISK SLOT Tencent定制化配置########################################################################
    echo "PRODUCT_VERSION_NUM=$PRODUCT_VERSION_NUM"
    if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE  DiskSlotCustomize
	DiskSlot_Customize=$parameter
	getini $CONFIGFILE  DiskSlotCustomizeId
	DiskSlot_CustomizeId=$parameter
	if [ "$DiskSlot_Customize" = "on" ] ; then
	
			if [ "$DiskSlot_CustomizeId" == "Tencent" ] ;then
				set_diskslot_customize_id 01
			else
				set_diskslot_customize_id 00
			fi
	else
		set_diskslot_customize_id 00  
	fi
    fi
	#定制化IPMI设置启动模式选项是否显示
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE BMCSet_BootModeSwEnable
	BootModeSwEnable=$parameter
	if [ "$BootModeSwEnable" == "off" ] ; then
		set_boot_mode_sw_enable 0
	else
		set_boot_mode_sw_enable 1
		fi
	fi
	#ipmitool elsit命令设置支持查询iBMC的SEL条数支持可配置定制:1~2000条(默认2000)
	getini $CONFIGFILE BMCSet_QuerySELMaxNumValue
	QuerySELMaxNumValue=$parameter
	if [ "$QuerySELMaxNumValue" != "" ] ; then
		set_query_sel_max_num_value $QuerySELMaxNumValue
	else
		set_query_sel_max_num_value 2000
	fi
	#定制化BMC默认的证书加密算法
	getini $CONFIGFILE BMCSet_CertAlgorithm
	CertAlgorithm=$parameter
	if [ "$CertAlgorithm" != "" ] ; then
		set_cert_algorithm $CertAlgorithm
	else
		set_cert_algorithm 0
	fi		
	
	#只有V5类型的单板才会进入如下定IPMI消息过滤总开关以及黑白名单模式定制化配置
	if [ "$PRODUCT_VERSION_NUM" != "03" ];then
		#定制化IPMI消息过滤总开关的状态：0：关闭; 1:开启
		getini $CONFIGFILE BMCSet_FireWallStatus
		FireWallStatus=$parameter
		if [ "$FireWallStatus" != "" ] ; then
			set_ipmi_fire_wall_status $FireWallStatus
		else
			#默认开关状态：防火墙状态打开(1)
			set_ipmi_fire_wall_status 1
		fi
		
		#定制化IPMI消息过滤总开关开启时，设置黑白名单模式的状态：0：黑名单; 1:白名单; 
		#定制化IPMI消息过滤总开关关闭时，设置黑白名单模式为无效状态：0xff
		getini $CONFIGFILE BMCSet_FireWallMode
		FireWallMode=$parameter
		if [ "$FireWallMode" != "" ] ; then
			set_ipmi_fire_wall_mode $FireWallMode
		else
			#默认模式0：黑名单
			set_ipmi_fire_wall_mode 0
		fi
	fi
	
	#设置sysname从hostname同步的标志
	getini $CONFIGFILE BMCSet_SysNameSyncHostName
	QuerySysNameSyncHostName=$parameter
	if [ "$QuerySysNameSyncHostName" != "" ] ; then
		set_sysname_sync_from_hostname_flag $QuerySysNameSyncHostName
	else
		set_sysname_sync_from_hostname_flag 0
	fi
	
	#定制化IErr诊断失败时处理策略 默认热复位业务侧
	getini $CONFIGFILE BMCSet_FDMIErrWarmResetEnable
	FDMDiagFailPolicy=$parameter
	if [ "$FDMDiagFailPolicy" != "" ] ; then
		set_fdm_diag_fail_policy $FDMDiagFailPolicy
	else
		set_fdm_diag_fail_policy 1
	fi

	#校验是否同步iBMA上报的hostname属性信息到BMC
	getini $CONFIGFILE BMCSet_ComSysHostnameSyncEnable
	hostSyncEnable=$parameter
	if [ "$hostSyncEnable" != "" ] ; then
		set_sync_hostname_enable $hostSyncEnable
	else
        #默认同步开关状态： 打开(1)
		set_sync_hostname_enable 1
	fi

	#设置sysname从配置文件中
	getsysnameini $CONFIGFILE BMCSet_CustomSysName
	QueryCustomSysName=$parameter
	if [ "$QueryCustomSysName" != "" ] ; then
		set_customized_sysname "$QueryCustomSysName"
	elif [ "$QueryCustomSysName" = "" ]; then
		getsysnameini $CONFIGFILE BMCSet_Cooperation_CustomSysName
		Cooperation_QueryCustomSysName=$parameter
		if [ "$Cooperation_QueryCustomSysName" != "" ] ; then
			set_customized_sysname "$Cooperation_QueryCustomSysName"
		else
			set_customized_sysname "iBMC"
		fi
	fi

	#设置redfish事件上报显示事件详细字段的定制化ID
	getini $CONFIGFILE BMCSet_EventDetailCustomizedID
	EventDetailCustomizedID=$parameter
	if [ "$EventDetailCustomizedID" != "" ] ; then
		set_rf_eventdetail_customized_id $EventDetailCustomizedID
	else
		set_rf_eventdetail_customized_id 0
	fi
	#设置IPMI定制化厂商ID
	getini $CONFIGFILE BMCSet_IpmiCustomManuID
	CustomManuID=$parameter
	if [ "$CustomManuID" == "" ] ; then
		set_custom_manu_id 2011
	elif [ "$CustomManuID" -gt "16777215" -o "$CustomManuID" -lt "0" ] ; then
	    echo_fail "set ipmi custom manu id parameter out of range! ID value(0~16777215)"
		echo "set ipmi custom manu id parameter out of range! ID value(0~16777215)" >>$SAVEFILE
		error_quit
    else
	    set_custom_manu_id $CustomManuID
	fi
	
	#设置按钮定制状态,只有TaiShan的单板才会进入按钮(长按或短按)屏蔽定制化配置
	if [ "$PRODUCT_VERSION_NUM" != "03" ] && [ $is_x86 = 0 ];then
		#获取按钮定制状态：0:不屏蔽; 1:屏蔽短按
		getini $CONFIGFILE BMCSet_PowerButtonLockStatus
		PowerButtonLockStatus=$parameter
		if [ "$PowerButtonLockStatus" != "" ] ; then
			set_power_button_lock_status $PowerButtonLockStatus
		else
			set_power_button_lock_status 0
		fi
	fi

	# 设置USB管理服务功能使能状态
	PRODUCT_NAME_ID=$PRODUCT_ID$BOARD_ID
	if [ "$PRODUCT_VERSION_NUM" == "06" ] ; then
		if [ "$PRODUCT_NAME_ID" == "$PRODUCT_2488HV6_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_2288HV6_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_1288HV6_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_TAISHAN2280V2_1711_ID" ] || [ "$PRODUCT_NAME_ID" == "$PRODUCT_TIANCHI_ID" ] ; then
            if [ "$PRODUCT_UNIQUE_ID" != "$PRODUCT_DA122C_UNIQUEID" ] ; then
			    getini $CONFIGFILE BMCSet_USBMgmtState
			    USBMgmtState=$parameter
			    if [ "$USBMgmtState" == "" ] || [ "$USBMgmtState" == "on" ] ; then
				    set_usb_mgmt_service_state 1
			    else
				    set_usb_mgmt_service_state 0
                fi
			fi
		fi
	fi

	#设置系统软件名称
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
	set_smsname "$SmsName"

	# 设置AC上电通电开机场景风扇默认速率比
	getini $CONFIGFILE BMCSet_FanInitDefaultLevel
	FanInitDefaultLevel=$parameter
	if [ "$FanInitDefaultLevel" == "" ] ; then
		set_fan_init_default_level 100
	elif [ $FanInitDefaultLevel -ge 50 -a $FanInitDefaultLevel -le 100 ] ; then
		set_fan_init_default_level $FanInitDefaultLevel
	else
		echo_fail "Set the default initial fan speed ratio($FanInitDefaultLevel) fail! The number must range from 50 to 100."
		error_quit
	fi
	
	# 设置SP设备信息收集功能开关状态
	getini $CONFIGFILE BMCSet_SPDeviceInfoCollectEnable
	SPDeviceInfoCollectEnable=$parameter
	if [ "$PRODUCT_VERSION_NUM" == "06" ] && ([ $PRODUCT_ID == $PRODUCT_RACK_TYPE ] || [ $PRODUCT_ID == $PRODUCT_XSERIALS_TYPE ] || [ $PRODUCT_ID = $PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE ] || [ $PRODUCT_ID == $PRODUCT_OSCA_TYPE ]) ; then
		# V6 X86，X系列，E9000系列空定制化值为开启
		if [ "$SPDeviceInfoCollectEnable" == "" ] || [ "$SPDeviceInfoCollectEnable" == "on" ] ; then
			set_sp_device_info_collect_enable 1
		elif [ "$SPDeviceInfoCollectEnable" == "off" ] ; then
		    set_sp_device_info_collect_enable 0
		else
		    echo_fail "Set SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable"
		    echo "Set SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable" >>$SAVEFILE
		    error_quit
		fi
	else
		# 其他产品空定制化值为关闭
		if [ "$SPDeviceInfoCollectEnable" == "" ] || [ "$SPDeviceInfoCollectEnable" == "off" ] ; then
			set_sp_device_info_collect_enable 0
		elif [ "$SPDeviceInfoCollectEnable" == "on" ] ; then
			set_sp_device_info_collect_enable 1
		else
		    echo_fail "Set SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable"
		    echo "Set SP device info collect enable state fail! Invalid param:$SPDeviceInfoCollectEnable" >>$SAVEFILE
		    error_quit
		fi
	fi

	#设置SKUNumber
	getini $CONFIGFILE BMCSet_SmBiosSKUNumber
	SKUNumber=$parameter
	if [ "$SKUNumber" != "" ] ; then
		set_smbios_skunumber "$SKUNumber"
	else
		set_smbios_skunumber ""
	fi
    
	#移动配置模型定制化
	getini $CONFIGFILE  BMCSet_ComSysConfigurationModel
	ConfigurationModel=$parameter
	if [ "$ConfigurationModel" != "" ] ; then
		set_configuration_model "$ConfigurationModel"
	else
		set_configuration_model "N/A"
	fi
    
	#移动版本信息定制化
	getini $CONFIGFILE  BMCSet_ComSysCmccVersion
	CmccVersion=$parameter
	if [ "$CmccVersion" != "" ] ; then
		set_cmcc_version "$CmccVersion"
	else
		set_cmcc_version "N/A"
	fi

	#设置redfish定制化厂商信息
	getini $CONFIGFILE BMCSet_RedfishCustomManuName
	CustomManufacturerName=$parameter
	if [ "$CustomManufacturerName" != "" ] ; then
		set_redfish_custom_manu_name "$CustomManufacturerName"
	else
		set_redfish_custom_manu_name "Huawei"
	fi
}

main
