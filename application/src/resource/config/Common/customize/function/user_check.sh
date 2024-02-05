#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#设置超长口令状态
check_long_passwd_enable()
{
	fun_id=1
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 12) 
	info_type=79 #y
	info_len="01"
	info_val="0$1"
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 12 "$exp_data"
	if [ $? != 0 ] ; then
		echo_fail "check_long_passwd_enable $1 Fail"
		echo "check_long_passwd_enable $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_long_passwd_enable $1 success!"
		echo "check_long_passwd_enable $1 success!" >>$SAVEFILE
	fi
}

#检测用户名设置
check_set_user_name()
{
    echo $USER_NAME >$IPMI_CMD_RETURN_FILE 
    ./conver $IPMI_CMD_RETURN_FILE 0 1 >temp_tmp.txt
    if [ $? != 0 ] ; then
       echo_fail  "Load user$1 name:$USER_NAME Fail"
       echo  "Load user$1 name:$USER_NAME Fail"  >>$SAVEFILE
       error_quit 
    fi

    user_name_assci=`cat temp_tmp.txt`
    strlenlen=`echo ${#USER_NAME}`
    zerolen=`expr 16 - $strlenlen `

    echo -n "$user_name_assci" > $IPMI_CMD_RETURN_FILE
    for((j=0;j<$zerolen;j++))
    {
	    echo -n " 00" >> $IPMI_CMD_RETURN_FILE
    }

    user_name_assci=`cat $IPMI_CMD_RETURN_FILE`
    userid=`printf %2x $1`

    #校验用户名设置是否成功
    run_cmd "f 00 06 46 $userid" "0f 07 00 46 00"
    if [ $? != 0 ] ; then
        echo_fail "verify user$1 name:$USER_NAME fail!"
        echo "verify user$1 name:$USER_NAME fail!" >>$SAVEFILE
        error_quit
    fi
    hextochar $IPMI_CMD_RETURN_FILE 15 temp_tmp.txt > /dev/null
    grep -wqF "$USER_NAME" temp_tmp.txt
    if [ $? != 0 ] ; then
        echo_fail "verify user$1 name:$USER_NAME fail!"
        echo "verify user$1 name:$USER_NAME fail!" >>$SAVEFILE
        error_quit
    else 
        echo_success "verify user$1 name:$USER_NAME success!"
        echo "verify user$1 name:$USER_NAME success!" >>$SAVEFILE
        return 0
    fi
}

#校验用户账户首次登陆时是否强制修改密码
check_user_first_login_password_reset_policy()
{
	userid=`printf %02x $1`
	option=`printf %02x $2`
	run_cmd "f 0 30 93 db 07 00 36 34 00 01 ff 00 00 01 00 00 01 $userid 0b 00" "0f 31 00 93 00 db 07 00 00 0b 00 79 01 $option"
	if [ $? != 0 ] ; then
		echo_fail "verify user first login policy $2 fail!"
        echo "verify userid is $userid!"
		echo "verify user first login policy $2 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "verify user first login policy $2 success!"
        echo "verify userid is $userid!"
		echo "verify user first login policy $2 success!" >>$SAVEFILE
		return 0
	fi
}

#检测用户密码
check_set_user_passwd()
{
    echo $USER_PASSWD >temp.txt 
    ./conver temp.txt 0 1 >temp_tmp.txt
    if [ $? != 0 ] ; then
		echo_fail  "Load user$1 passwd:$USER_PASSWD Fail"
        echo  "Load user$1 passwd:$USER_PASSWD Fail"  >>$SAVEFILE
        error_quit
    fi

    user_passwd_assci=`cat temp_tmp.txt`
    strlenlen=`echo ${#USER_PASSWD}`
    zerolen=`expr 20 - $strlenlen`

    echo -n "$user_passwd_assci" > temp.txt
    for((k=0;k<$zerolen;k++))
    {
	    echo -n " 00" >> temp.txt
    }

    user_passwd_assci=`cat temp.txt`
    userid=`expr 128 + $1`
    userid=`printf %02x $userid`

    #校验用户密码设置是否成功
    if [ "$USER_PASSWD" = "null" ] ; then
	run_cmd "f 00 06 47 02 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" "0f 07 00 47 00"
    else
        run_cmd "f 00 06 47 $userid 03 $user_passwd_assci" "0f 07 00 47 00"
    fi	
    
    if [ $? != 0 ] ; then
        echo_fail "verify user$1 passwd:$USER_PASSWD fail!"
        echo "verify user$1 passwd:$USER_PASSWD fail!" >>$SAVEFILE
        error_quit
    else 
        echo_success "verify user$1 passwd:$USER_PASSWD success!"
        echo "verify user$1 passwd:$USER_PASSWD success!" >>$SAVEFILE
        return 0
    fi
}

#检测用户权限
check_set_user_access()
{
    useraccess=4
    if [ "$USER_ACCESS" = "admin" ] ; then
	    useraccess=4
    fi

    if [ "$USER_ACCESS" = "operator" ] ; then
	    useraccess=3
    fi

    if [ "$USER_ACCESS" = "user" ] ; then
	    useraccess=2
    fi

    if [ "$USER_ACCESS" = "callback" ] ; then
	    useraccess=1
    fi
    
    if [ "$USER_ACCESS" = "noaccess" ] ; then
	    useraccess=f
    fi
    
    userid=`printf %2x $1`

    #校验用户权限设置是否成功
    run_cmd "f 00 06 44 01 $userid" "0f 07 00 44 00"
    if [ $? != 0 ] ; then
        echo_fail "verify user$1 access:$USER_ACCESS fail!"
        echo "verify user$1 access:$USER_ACCESS fail!" >>$SAVEFILE
        error_quit
    fi

    temp=`cat  $IPMI_CMD_RETURN_FILE`
    temp=`echo $temp | cut -b 26-100`

    if [ "$useraccess" != "$temp" ] ; then
        echo_fail "verify user$1 access:$USER_ACCESS fail!"
        echo "verify user$1 access:$USER_ACCESS fail!" >>$SAVEFILE
        error_quit
    else 
        echo_success "verify user$1 access:$USER_ACCESS success!"
        echo "verify user$1 access:$USER_ACCESS success!" >>$SAVEFILE
        return 0
    fi
}

verify_admin_user()
{
	USER_NAME="root"
	USER_PASSWD="Huawei12#$"
	USER_ACCESS="admin"
	if [ "$PRODUCT_VERSION_NUM" != "03" ]; then
		USER_NAME="Administrator"
		USER_PASSWD="Admin@9000"
	fi
	getini  $CONFIGFILE  BMCSet_CustomAdminInfo
	ADMIN_INFO=$parameter
	if [ "$ADMIN_INFO" = "on" ] ; then
		getini  $CONFIGFILE  Custom_AdminName
		USER_NAME=$parameter
		if [ "$USER_NAME" = "" ] ; then
			echo_fail "get default admin name fail"
			echo "get default admin name fail" >>$SAVEFILE
			error_quit
		fi
	
		getini  $CONFIGFILE  Custom_AdminPassword
		USER_PASSWD=$parameter
		if [ "$USER_PASSWD" = "" ] ; then
			echo_fail "get default admin password fail"
			echo "get default admin password fail" >>$SAVEFILE
			error_quit
		fi	
	else
		# 未定制修改预置账户时，需校验其密码策略是否为仅提示修改
		# $1 管理员帐户ID，固定值2
		# $2 密码策略FIRST_LOGIN_PLCY_PROMPT 1
		check_user_first_login_password_reset_policy 2 1
	fi

	#验证修改权限时不受BMCSet_CustomAdminInfo影响，
	#USER_ACCESS为空时默认恢复为admin
	getini  $CONFIGFILE  Custom_AdminAccess
	USER_ACCESS=$parameter
	if [ "$USER_ACCESS" = "" ] ; then
		USER_ACCESS="admin"				
	fi
	
	check_set_user_name 2
	sleep 1
	check_set_user_passwd 2
	sleep 1
	check_set_user_access 2
	sleep 1 
	
	check_user_interface 2
	sleep 1
	
	echo_success "verify admin user success!"
	echo "verify admin user success!" >>$SAVEFILE
	return 0
}

check_set_LocalUserPasswdCheck_ForeverEnable()
{
	run_cmd  "f 0 30 93 db 07 00 21"  "0f 31 00 93 00 db 07 00 02"
	if [ $? != 0 ] ; then		
		echo_fail "Verfiy LocalUserPasswdCheckForeverEnable fail"
		echo "Verfiy LocalUserPasswdCheckForeverEnable fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Verfiy LocalUserPasswdCheckForeverEnable success"
		echo "Verfiy LocalUserPasswdCheckForeverEnable success" >>$SAVEFILE
	fi    	
}

#校验本地用户密码复杂度使能设置
check_set_LocalUserPasswdCheck_enable()
{
	run_cmd  "f 0 30 93 db 07 00 21"  "0f 31 00 93 00 db 07 00 01"
	if [ $? != 0 ] ; then
		
		echo_fail "Verfiy LocalUserPasswdCheckEnable fail"
		echo "Verfiy LocalUserPasswdCheckEnable fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Verfiy LocalUserPasswdCheckEnable success"
		echo "Verfiy LocalUserPasswdCheckEnable success" >>$SAVEFILE
	fi
}
#校验本地用户密码复杂度去使能设置
check_set_LocalUserPasswdCheck_disable()
{
	run_cmd  "f 0 30 93 db 07 00 21"  "0f 31 00 93 00 db 07 00 00"
	if [ $? != 0 ] ; then
		
		echo_fail "Verfiy LocalUserPasswdCheckDisable fail"
		echo "Verfiy LocalUserPasswdCheckDisable fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Verfiy LocalUserPasswdCheckDisable success"
		echo "Verfiy LocalUserPasswdCheckDisable success" >>$SAVEFILE
	fi
}

#检查用户安全加固参数配置
check_security_enhance_config()
{
	type=$1
	value="$2"
	
	if [ "$type" == "1" ] ;then
		TypeString="ForbidLoginTime"
	elif [ "$type" == "2" ] ;then
		TypeString="FailedLoginCount"
	elif [ "$type" == "3" ] ;then
		TypeString="ExpiredTime"
	elif [ "$type" == "4" ] ;then
		TypeString="ExcludeUserID"
	elif [ "$type" == "5" ] ;then
		TypeString="SamePwdCheckCount"
	elif [ "$type" == "7" ] ;then
		TypeString="EnableUserMgnt"
	elif [ "$type" == "9" ] ;then
		TypeString="TlsProtocol"
	elif [ "$type" == "10" ] ;then
		TypeString="MinimumPwdAge"
	elif [ "$type" == "12" ] ;then
		TypeString="SecurityBannerEnable"
	elif [ "$type" == "13" ] ;then
		TypeString="SecurityBanner"
	elif [ "$type" == "14" ] ;then
		TypeString="DoubleCertification"		
	elif [ "$type" == "15" ] ;then
		TypeString="WeakPwdDictEnable"	
	elif [ "$type" == "16" ] ;then
		TypeString="InitialPwdPrompt"
	elif [ "$type" == "17" ] ;then
		TypeString="UserInactTimeLimit"
	else
		echo_fail "check_security_enhance_config parameter fail"
		echo "check_security_enhance_config parameter fail">>$SAVEFILE
		error_quit
	fi
	
	fun_id=2
	key_id=$(word2string 8) 
	key_type=75
	key_len=4
	key_val="22 00 00 00"
	info_id=$(word2string $type) 

	if [ "$type" == "1" ] || [ "$type" == "2" ] || [ "$type" == "9" ]; then
		info_type=71
		info_len="02"
		info_val=$(word2string $value)
	elif [ "$type" == "3" ] || [ "$type" == "10" ] || [ "$type" == "17" ] ; then
		info_type=75
		info_len="04"
		info_val=$(dword2string $2)
	elif [ "$type" == "13" ] ; then
		length=`echo ${#value}`	
		string2substring "$value" $length
		info_type=73
		info_len=`printf "%02x" $length`
		info_val=$TMP_STRING
    else
		value=`printf "%02x" $2`
		info_type=79
		info_len="01"
		info_val=`printf "%02x" $2`
	fi
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" $type "$exp_data"
	if [ $? != 0 ] ; then
       	echo_fail "Check $TypeString $value fail"
		echo "Check $TypeString $value fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check $TypeString $value success"
		echo "Check $TypeString $value success" >>$SAVEFILE
   	fi
}

#校验空用户
check_restore_user_name()
{
    #用户id为可变化
    userid=`printf %2x $1`

    #校验用户名名称是否为空
    run_cmd "f 00 06 46 $userid"  "0f 07 00 46 00" 
    if [ $? != 0 ] ; then
        echo_fail "verify restore user$1:NULL fail!"
        echo "verify restore user$1:NULL fail!" >>$SAVEFILE
        error_quit
    fi

    #./hex2char.exe temp.txt 14 temp_tmp.txt > /dev/null
    hextochar $IPMI_CMD_RETURN_FILE 15 temp_tmp.txt
    grep -wq "" temp_tmp.txt
    if [ $? != 0 ] ; then
        echo_fail "verify restore user$1:NULL fail!"
        echo "verify restore user$1:NULL fail!" >>$SAVEFILE
        error_quit
    else
        return 0
    fi
}

#检查用户接口
# web 		(offset=0)(val=1)
# snmp 		(offset=1)(val=2)
# ipmi 		(offset=2)(val=4)
# ssh 		(offset=3)(val=8)
# sftp 		(offset=4)(val=16)
# telnet 	(offset=5)(val=32)
# local 	(offset=6)(val=64)
# redfish 	(offset=7)(val=128)
# 因此所有接口就是 127
check_user_interface()
{
	num=$1
	default=223 #默认全接口支持 223 = 0xdf 
	
	#下面处理：如果关闭此接口，则减掉此接口对应的掩码; 默认为开启
	getini  $CONFIGFILE  "BMCSet_CustomLoginInterface""$num""_web"
	PARAM=$parameter
	if [ "$PARAM" = "off" ]; then
		default=$(($default - 1))
	fi
	
	getini  $CONFIGFILE  "BMCSet_CustomLoginInterface""$num""_snmp"
	PARAM=$parameter
	if [ "$PARAM" = "off" ]; then
		default=$(($default - 2))
	fi
	
	getini  $CONFIGFILE  "BMCSet_CustomLoginInterface""$num""_ipmi"
	PARAM=$parameter
	if [ "$PARAM" = "off" ]; then
		default=$(($default - 4))
	fi
	
	getini  $CONFIGFILE  "BMCSet_CustomLoginInterface""$num""_ssh"
	PARAM=$parameter
	if [ "$PARAM" = "off" ]; then
		default=$(($default - 8))
	fi
	
	getini  $CONFIGFILE  "BMCSet_CustomLoginInterface""$num""_sftp"
	PARAM=$parameter
	if [ "$PARAM" = "off" ]; then
		default=$(($default - 16))
	fi
	
	getini  $CONFIGFILE  "BMCSet_CustomLoginInterface""$num""_local"
	PARAM=$parameter
	if [ "$PARAM" = "off" ]; then
		default=$(($default - 64))
	fi
	
	getini  $CONFIGFILE  "BMCSet_CustomLoginInterface""$num""_redfish"
	PARAM=$parameter
	if [ "$PARAM" = "off" ]; then
		default=$(($default - 128))
	fi
	
	#其他位默认设置为1    4294967040=0xFFFFFF00
	default=$(($default + 4294967040))
	
	fun_id=52
	key_id=$(word2string 1) #使用user id作为索引 
	key_type=79
	key_len="01"
	key_val=`printf %2x $num`
	info_id=$(word2string 3) 
	info_type=75 #u
	info_len="04"
	info_val=$(dword2string $default)
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "check_user_interface $default Fail"
		echo "check_user_interface $default Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_user_interface $default success!"
		echo "check_user_interface $default success!" >>$SAVEFILE
	fi
}

main()
{
	chmod +x *
	#2.1、校验默认管理用户
	verify_admin_user
	#2.2、校验用户列表
	for((m=3;m<17;m++))
	{
		getini  $CONFIGFILE  BMCSet_CustomName$m
		USER_NAME=$parameter
		if [ "$USER_NAME" = "" ] ; then
			check_restore_user_name $m
			sleep 1
		else
			check_set_user_name $m
			sleep 1
			
			getini  $CONFIGFILE  BMCSet_CustomNamePassword$m
			USER_PASSWD=$parameter
			if [ "$USER_PASSWD" != "" ] ; then
				check_set_user_passwd $m
				sleep 1
			fi

			getini  $CONFIGFILE  BMCSet_CustomNameAccess$m
			USER_ACCESS=$parameter
			if [ "$USER_ACCESS" != "" ] ; then
				check_set_user_access $m
				sleep 1
			fi
			
			# 检查用户接口支持情况
			check_user_interface $m

			# 校验其密码策略是否为仅提示修改
		    check_user_first_login_password_reset_policy $m 1
		fi	
	}
	echo_success "verify other users success!"
	echo "verify other users success!" >>$SAVEFILE

	#2.3、校验本地用户密码复杂度检查开启状态
	getini  $CONFIGFILE  BMCSet_UserPasswdComplexityForeverEnable
	USER_PASSWD_COMPLEXITY_CHECK=$parameter
	if [ "$USER_PASSWD_COMPLEXITY_CHECK" = "on" ] ; then
		check_set_LocalUserPasswdCheck_ForeverEnable
		echo "Forever_Enable UserPasswdComplexityCheck!" >>$SAVEFILE
		sleep 1
	else
		getini  $CONFIGFILE  BMCSet_UserPasswdComplexityCheckEnable
		USER_PASSWD_COMPLEXITY_CHECK=$parameter		
		if [ "$USER_PASSWD_COMPLEXITY_CHECK" = "off" ] ; then
			check_set_LocalUserPasswdCheck_disable
		else
			check_set_LocalUserPasswdCheck_enable
		fi
	fi
	
	#超长口令
	getini $CONFIGFILE BMCSet_LongPasswordEnable
	LongPasswordEnable=$parameter

	if [ "$LongPasswordEnable" = "" ] ; then
		if [ "$PRODUCT_VERSION_NUM" == "03" ] ; then
			check_long_passwd_enable 0  #V3产品默认关闭
			echo "check_long_passwd_enable 0 success"			
		else
			check_long_passwd_enable 1  #V5及以上产品默认开启
			echo "check_long_passwd_enable 1 success"			
		fi
	else
		check_long_passwd_enable $LongPasswordEnable
		echo "check_long_passwd_enable $LongPasswordEnable success"
	fi

	############################################################################################
	#校验密码有效期
	getini $CONFIGFILE BMCSet_CustomExpiredTime
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_ExpiredTime
		VALUE=$parameter
		if [ "$VALUE" -gt "365" ] || [ "$VALUE" = "" ] ; then
			echo_fail "Custom_ExpiredTime invaild! set Custom_ExpiredTime $VALUE failed!"
			echo "Custom_ExpiredTime invaild! set Custom_ExpiredTime $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=0;
	fi
	check_security_enhance_config 3 $VALUE

	#校验重复密码保留个数
	getini $CONFIGFILE BMCSet_CustomSamePwdCheckCount
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_SamePwdCheckCount
		VALUE=$parameter
		if [ "$VALUE" -gt "5" ] || [ "$VALUE" = "" ] ; then
			echo_fail "Custom_SamePwdCheckCount invaild! set Custom_SamePwdCheckCount $VALUE failed!"
			echo "Custom_SamePwdCheckCount invaild! set Custom_SamePwdCheckCount $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=5;
	fi
	check_security_enhance_config 5 $VALUE

	#校验失败锁定次数
	getini $CONFIGFILE BMCSet_CustomFailedLoginCount
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_FailedLoginCount
		VALUE=$parameter
		if [ "$VALUE" -lt "0" ] || [ "$VALUE" -gt "6" ] || [ "$VALUE" = "" ] ; then
			echo_fail "Custom_FailedLoginCount invaild! set Custom_FailedLoginCount $VALUE failed!"
			echo "Custom_FailedLoginCount invaild! set Custom_FailedLoginCount $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=5;
	fi

	check_security_enhance_config 2 $VALUE

	#校验失败锁定时间
	getini $CONFIGFILE BMCSet_CustomForbidLoginTime
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_ForbidLoginTime
		VALUE=$parameter
		if [ "$VALUE" -lt "1" ] || [ "$VALUE" -gt "30" ] || [ "$VALUE" = "" ] ; then
			echo_fail "Custom_ForbidLoginTime invaild! set Custom_ForbidLoginTime $VALUE failed!"
			echo "Custom_ForbidLoginTime invaild! set Custom_ForbidLoginTime $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=5;
	fi
	
	VALUE=`expr 60 \* $VALUE`
	check_security_enhance_config 1 $VALUE
    
	#校验TLS协议
	getini $CONFIGFILE BMCSet_CustomTLSProtocol
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_TLSProtocol
		VALUE=$parameter
		if [[ !("$VALUE" =~ ^[4-5]$) ]] ; then
			echo_fail "Custom_TLSProtocol invaild! set Custom_TLSProtocol $VALUE failed!"
			echo "Custom_TLSProtocol invaild! set Custom_TLSProtocol $VALUE failed!" >>$SAVEFILE
			error_quit
        elif [ "$VALUE" -eq "4" ] ; then
            VALUE=0xfffc           
        elif [ "$VALUE" -eq "5" ] ; then
            VALUE=0xfff8
		fi
	else
		VALUE=0xfffc;
	fi

	check_security_enhance_config 9 $VALUE

	#校验特殊用户
	getini $CONFIGFILE BMCSet_CustomExcludeUserID
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_ExcludeUserID
		VALUE=$parameter
		if [ "$VALUE" -gt "17" ] || [ "$VALUE" = "" ] ; then
			echo_fail "Custom_ExcludeUserID invaild! set Custom_ExcludeUserID $VALUE failed!"
			echo "Custom_ExcludeUserID invaild! set Custom_ExcludeUserID $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=0;
	fi

	check_security_enhance_config 4 $VALUE

	#校验密码最短使用期限
	getini $CONFIGFILE BMCSet_CustomMinimumPwdAge
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_MinimumPwdAge
		VALUE=$parameter
		is_digit $VALUE
		if [ $? != 0  ]; then
			echo_fail "Custom_MinimumPwdAge value($VALUE) is wrong";
			echo "Custom_MinimumPwdAge value($VALUE) is wrong">>$SAVEFILE;
			error_quit
		fi
		if [ "$VALUE" -lt "0" ] || [ "$VALUE" -gt "365" ] ; then
			echo_fail "Custom_MinimumPwdAge invaild! set Custom_MinimumPwdAge $VALUE failed!"
			echo "Custom_MinimumPwdAge invaild! set Custom_MinimumPwdAge $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=0;
	fi

	check_security_enhance_config 10 $VALUE
#DTS2015012800217，读写团体名和只读团体名由于安全红线原因禁止查询，不做校验，设置返回OK即表示设置成功

	#校验安全公告使能
	getini $CONFIGFILE BMCSet_SecurityBannerEnable
	STATUS=$parameter
	if [ "$STATUS" != "on" ] && [ "$STATUS" != "off" ] && [ "$STATUS" != "" ] ; then
		echo_and_save_fail "Invalid Security Banner Enable switch($STATUS)"
		error_quit
	fi
	if [ "$STATUS" = "off" ] ; then
		VALUE=0
	else
		VALUE=1
	fi    
	check_security_enhance_config 12 $VALUE

	#校验默认安全公告
	getini $CONFIGFILE BMCSet_CustomSecurityBanner
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_SecurityBanner
		VALUE=$parameter
		LENGTH=`echo ${#VALUE}`
		if [ "$LENGTH" -gt 1024 ] ; then
			echo_fail "SecurityBanner invaild! set SecurityBanner $VALUE failed!"
			echo "SecurityBanner invaild! set SecurityBanner $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE="WARNING! This system is PRIVATE and PROPRIETARY and may only be accessed by authorized users. Unauthorized use of the system is prohibited. The owner, or its agents, may monitor any activity or communication on the system. The owner, or its agents, may retrieve any information stored within the system. By accessing and using the system, you are consenting to such monitoring and information retrieval for law enforcement and other purposes."
	fi
	
	check_security_enhance_config 13 "$VALUE"
	
	#校验二次认证使能
	getini $CONFIGFILE BMCSet_DoubleCertificationEnable
	STATUS=$parameter
	if [ "$STATUS" != "on" ] && [ "$STATUS" != "off" ] && [ "$STATUS" != "" ] ; then
		echo_and_save_fail "Invalid Double Certification Enable switch($STATUS)"
		error_quit
	fi
	if [ "$STATUS" = "off" ] ; then
		VALUE=0
	else
		VALUE=1;
	fi

	check_security_enhance_config 14 $VALUE
    
	#校验设备是否支持弱口令字典
	get_prop_cmd 48 "ff ff 00 01 00" 9 "09 00 79 01 01"
	if [ $? == 0 ] ; then
		#配置弱口令字典检查使能
		getini $CONFIGFILE BMCSet_WeakPasswdDictionaryCheck
		STATUS=$parameter
		if [ "$STATUS" != "on" ] && [ "$STATUS" != "off" ] && [ "$STATUS" != "" ] ; then
			echo_and_save_fail "Invalid Weak Passwd Dictionary Check switch($STATUS)"
			error_quit
		fi
		if [ "$STATUS" = "off" ] ; then
			VALUE=0
		else
			VALUE=1
		fi
        
		check_security_enhance_config 15 $VALUE
	else
		echo "Device does not support weak password dictionary check."
	fi    
	
	#校验是否提示初始密码
	getini $CONFIGFILE BMCSet_InitialPwdPrompt
	STATUS=$parameter
	if [ "$STATUS" = "off" ] ; then
		VALUE=0
	else
		VALUE=1;
	fi
	check_security_enhance_config 16 $VALUE
	
	#校验禁用不活动用户期限
	getini $CONFIGFILE BMCSet_CustomInactiveTimelimit
	STATUS=$parameter
	if [ "$STATUS" != "on" ] && [ "$STATUS" != "off" ] && [ "$STATUS" != "" ] ; then
		echo_and_save_fail "Invalid Custom Inactive Timelimit switch($STATUS)"
		error_quit
	fi
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_InactiveTimelimit
		VALUE=$parameter
		if [ "$VALUE" -lt "30" ] || [ "$VALUE" -gt "365" ] || [ "$VALUE" = "" ] ; then
			echo_fail "Custom_InactiveTimelimit invaild! set Custom_InactiveTimelimit $VALUE failed!"
			echo "Custom_InactiveTimelimit invaild! set Custom_InactiveTimelimit $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=0;
	fi
	check_security_enhance_config 17 $VALUE
}

main
