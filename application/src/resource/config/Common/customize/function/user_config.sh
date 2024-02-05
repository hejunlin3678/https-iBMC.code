#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#关闭密码复杂度检查，修改用户密码前使用
restore_LocalUserPasswdCheck_disable()
{
	run_cmd  "f 0 30 93 db 07 00 22 00"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "restore LocalUserPasswdCheck disable fail"
		echo "restore LocalUserPasswdCheck disable fail"  >>$SAVEFILE
	else
		return 0
	fi
}

#写用户安全加固参数(需要按照新的ipmi进行封装)
write_security_enhance_config()
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
		echo_fail "write_security_enhance_config parameter fail"
		echo "write_security_enhance_config parameter fail">>$SAVEFILE
		error_quit
	fi

	fun_id=2
	key_id=$(word2string 8) 
	key_type=75
	key_len=4
	key_val="22 00 00 00"
	info_id=$(word2string $type) 

	echo "write_security_enhance_config: args:$1  $2" >> $SAVEFILE
	
	if [ "$type" == "2" ] || [ "$type" == "1" ] || [ "$type" == "9" ] ; then
		info_type=71
		info_len=2
		info_val=$(word2string $value)
	elif [ "$type" == "3" ] || [ "$type" == "10" ] || [ "$type" == "17" ] ; then
		info_type=75
		info_len=4
		info_val=$(dword2string $value)
	elif [ "$type" == "13" ] ; then
		length=`echo ${#value}`	
		string2substring "$value" $length
		info_type=73
		info_len=`printf "%02x" $length`
		info_val=$TMP_STRING
	else
		info_type=79
		info_len=1
		info_val=`printf "%02x" $value`
	fi
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       	echo_fail "Set $TypeString to $value fail"
		echo "Set $TypeString to $value fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Set $TypeString to $value success"
		echo "Set $TypeString to $value success" >>$SAVEFILE
   	fi
}

#还原默认用户
restore_user_name()
{
    #用户id为可变化
    userid=`printf %2x $1`
    run_cmd  "f 00 06 45 $userid 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"  "0f 07 00 45 00"
    if [ $? != 0 ] ; then
		echo_fail "restore user$1:NULL fail!"
		echo "restore user$1:NULL fail!" >> $SAVEFILE
		error_quit
    else
		return 0
    fi
}

#设置用户名
set_user_name()
{
    echo $USER_NAME > temp.txt 
    ./conver temp.txt 0 1 > temp_tmp.txt
    if [ $? != 0 ] ; then
       echo_fail  "Load user$1 name:$USER_NAME Fail"
       echo  "Load user$1 name:$USER_NAME Fail"  >>$SAVEFILE
       error_quit 
    fi

    user_name_assci=`cat temp_tmp.txt`
    strlenlen=`echo ${#USER_NAME}`
    zerolen=`expr 16 - $strlenlen `
    echo -n "$user_name_assci" > temp.txt

    for((j=0;j<$zerolen;j++))
    {
	    echo -n " 00" >> temp.txt
    }

    user_name_assci=`cat temp.txt`
    userid=`printf %2x $1`
    run_cmd "f 00 06 45 $userid $user_name_assci" "0f 07 00 45 00"
    if [ $? != 0 ] ; then
        echo_fail "Set user$1 name:$USER_NAME fail!"
        echo "Set user$1 name:$USER_NAME fail!" >>$SAVEFILE
        error_quit 
    else
        echo_success "Set user$1 name:$USER_NAME success!"
        echo "Set user$1 name:$USER_NAME success!" >>$SAVEFILE
        return 0
    fi
}

#设置用户首次登陆密码修改策略
set_user_first_login_password_reset_policy()
{
	userid=`printf %02x $1`
	value=`printf %02x $2`
	run_cmd "f 00 30 93 db 07 00 35 34 00 01 00 00 00 01 00 79 01 $userid 0b 00 79 01 $value" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "Set user$1 first login policy to $2 fail!"
        echo "Set userid is $userid!"
		echo "Set user$1 first login policy to $2 fail!" >>$SAVEFILE
		error_quit
    else
        echo_success "Set user$1 first login policy to $2 success!"
        echo "Set userid is $userid!"
	    echo "Set user$1 first login policy to $2 success!" >>$SAVEFILE
        return 0
	fi
}

#设置用户密码
set_user_passwd()
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
    if [ "$USER_PASSWD" = "null" ] ; then
	run_cmd "f 00 06 47 02 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" "0f 07 00 47 00"
    else
        run_cmd "f 00 06 47 $userid 02 $user_passwd_assci" "0f 07 00 47 00"
    fi
	
    if [ $? != 0 ] ; then
		echo_fail "Set user$1 passwd:$USER_PASSWD fail!"
		echo "Set user$1 passwd:$USER_PASSWD fail!" >>$SAVEFILE
		error_quit
    else
		echo_success "Set user$1 passwd:$USER_PASSWD success!"
		echo "Set user$1 passwd:$USER_PASSWD success!" >>$SAVEFILE
		return 0
    fi
}

#设置用户SNMP加密密码为初始状态
set_user_snmp_privacy_passwd_initial_state()
{
    if [ $PRODUCT_VERSION_NUM -le 03 ];then
		return 0
    fi

    #通过设置属性的IPMI命令设置SNMP加密密码状态为初始状态
    local userid=`printf %02x $1`
    run_cmd "f 00 30 93 db 07 00 35 34 00 01 00 00 00 01 00 79 01 $userid 0a 00 79 01 01" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
		echo_fail "Set user$1 snmp privacy passwd initial state fail!"
		echo "Set user$1 snmp privacy passwd initial state fail!" >>$SAVEFILE
		error_quit
    else
		echo_success "Set user$1 snmp privacy passwd initial state success!"
		echo "Set user$1 snmp privacy passwd initial state success!" >>$SAVEFILE
		return 0
    fi
}

#设置用户权限
set_user_access()
{
    #useraccess=0
    if [ "$USER_ACCESS" = "admin" ] ; then
        useraccess=4
    elif [ "$USER_ACCESS" = "operator" ] ; then
        useraccess=3
    elif [ "$USER_ACCESS" = "user" ] ; then
        useraccess=2
    elif [ "$USER_ACCESS" = "callback" ] ; then
        useraccess=1
    elif [ "$USER_ACCESS" = "noaccess" ] ; then
        useraccess=f
    else
        echo_fail "Set user$1 access:$USER_ACCESS fail!"
	echo "Set user$1 access:$USER_ACCESS fail!" >>$SAVEFILE
	exit
    fi
     
    userid=`printf %2x $1`
    run_cmd "f 00 06 43 01 $userid $useraccess" "0f 07 00 43 00"
    if [ $? != 0 ] ; then
		echo_fail "Set user$1 access:$USER_ACCESS fail!"
		echo "Set user$1 access:$USER_ACCESS fail!" >>$SAVEFILE
		error_quit
    fi
	
    #此命令不需要补充00,但是IPMI那边对该命令长度做了判断，使能用户
    run_cmd "f 00 06 47 $userid 1 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" "0f 07 00 47 00"
	
    if [ $? != 0 ] ; then
		echo_fail "Set user$1 access:$USER_ACCESS fail!"
		echo "Set user$1 access:$USER_ACCESS fail!" >>$SAVEFILE
		error_quit
    else 
		echo_success "Set user$1 access:$USER_ACCESS success!"
		echo "Set user$1 access:$USER_ACCESS success!" >>$SAVEFILE
		return 0
    fi
}

#清除用户锁定状态
set_user_unlock()
{
    userid=`printf %2x $1`
    run_cmd "f 00 30 93 db 07 00 35 34 00 01 00 00 00 01 00 00 01 $userid 09 00 79 01 00" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
		echo_fail "Set user$1 unlock fail!"
		echo "Set user$1 unlock fail!" >>$SAVEFILE
		error_quit
    fi	

    return 0
}

#配置默认管理员帐户信息
config_admin_user()
{
	#/* BEGIN: Modified for PN:DTS2013051500991 by w00208253, 2013/5/21 */
	USER_NAME="root"
	USER_PASSWD="Huawei12#$"
	USER_ACCESS="admin"
	
	if [ $PRODUCT_VERSION_NUM -gt 03 ]; then
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
		# 未定制修改预置账户时，需将其密码策略重置为仅提示修改
		# $1 管理员帐户ID，固定值2
		# $2 设置密码策略FIRST_LOGIN_PLCY_PROMPT 1
		set_user_first_login_password_reset_policy 2 1
	fi
	
	set_user_name 2
	sleep 1
	set_user_interface 2
	sleep 1
	set_user_passwd 2
	sleep 1
	set_user_access 2
	sleep 1
	#/* END:   Modified for PN:DTS2013051500991 by w00208253, 2013/5/21 */
	
	
	#解锁默认管理员用户，防止返修单板出现默认管理员用户不能登录
	set_user_unlock 2
	
	#通过设置属性万能命令设置SNMP加密密码状态为初始状态
	set_user_snmp_privacy_passwd_initial_state 2
	sleep 1
}

#设置团体名(使用新的命令进行封装)
config_snmp_mib_Community()
{
 	COMMUNITY=$2
	string2substring "$COMMUNITY"
	strlenlen=`echo ${#COMMUNITY}`
	fun_id=1
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len=1
	key_val=1
	info_id=$(word2string $1) 
	info_type=73
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

#用户密码复杂度永久启用使能设置
set_LocalUserPasswdCheck_ForeverEnable()
{
	#永久启用密码复杂度
	run_cmd  "f 0 30 93 db 07 00 22 02"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "Forever_enable LocalUserPasswdCheck fail"
		echo "Forever_enable LocalUserPasswdCheck fail"  >>$SAVEFILE
		error_quit
	else
		echo_success "Forever_enable LocalUserPasswdCheck success"
		echo "Forever_enable LocalUserPasswdCheck success"  >>$SAVEFILE	
			
	fi
	sleep 1
}

#用户密码复杂度使能设置
set_LocalUserPasswdCheck_enable()
{
	run_cmd  "f 0 30 93 db 07 00 22 01"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "Enable LocalUserPasswdCheck fail"
		echo "Enable LocalUserPasswdCheck fail"  >>$SAVEFILE
		error_quit
	else
		echo_success "Enable LocalUserPasswdCheck success"
		echo "Enable LocalUserPasswdCheck success"  >>$SAVEFILE	
		return 0
	fi
}
#用户密码复杂度去使能设置
set_LocalUserPasswdCheck_disable()
{
	run_cmd  "f 0 30 93 db 07 00 22 00"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "Disable LocalUserPasswdCheck fail"
		echo "Disable LocalUserPasswdCheck fail"  >>$SAVEFILE
		error_quit
	else
		echo_success "Disable LocalUserPasswdCheck success"
		echo "Disable LocalUserPasswdCheck success"  >>$SAVEFILE	
		return 0
	fi
}

#关闭超长口令开关
disable_long_passwd()
{
	fun_id=1
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 12) 
	info_type=79 #y
	info_len="01"
	info_val="00"
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "disable_long_passwd Fail"
		echo "disable_long_passwd Fail" >>$SAVEFILE
		error_quit
	fi
}

#设置超长口令属性
set_long_passwd_enable()
{
	fun_id=1
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len="01"
	key_val="01"
	info_id=$(word2string 12) 
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $1`
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set_long_passwd_enable $1 Fail"
		echo "set_long_passwd_enable $1 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "set_long_passwd_enable $1 success!"
		echo "set_long_passwd_enable $1 success!" >>$SAVEFILE
	fi
}

#设置用户接口
# web 		(offset=0)(val=1)
# snmp 		(offset=1)(val=2)
# ipmi 		(offset=2)(val=4)
# ssh 		(offset=3)(val=8)
# sftp 		(offset=4)(val=16)
# local 	(offset=6)(val=64)
# redfish 	(offset=7)(val=128)
# 因此所有接口就是 127
set_user_interface()
{
	num=$1
	default=223 #默认全接口支持 223 = 0xDF
    operation=1
	
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
        operation=0
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

    iana_id="db 07 00"
    sub_cmd=68
    user_id=`printf %02x $num`
    operation=`printf %02x $operation`
    info_val=`printf %02x $default`
    reserved="00"

    getini  $CONFIGFILE  BMCSet_CustomNamePassword$num
    USER_PASSWORD=$parameter
    if [ "$USER_PASSWORD" != "" ] ; then
        pwd_len=`echo ${#USER_PASSWORD}`
        pwd_len=`printf %02x $pwd_len`
        echo $USER_PASSWORD >temp.txt 
        ./conver temp.txt 0 1 >temp_tmp.txt
        pwd_assci=`cat temp_tmp.txt`
        set_data="$iana_id $sub_cmd $user_id $operation $info_val $reserved $pwd_len $pwd_assci"
    else
        # 不设置密码
        operation=`printf %02x 0`
        set_data="$iana_id $sub_cmd $user_id $operation $info_val"
    fi

    run_cmd "f 0 30 93 $set_data" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
        # 老版本返回码为00 00 00,新版本返回码为db 07 00,为保证兼容性,此处校验两种返回码
        run_cmd "f 0 30 93 $set_data" "0f 31 00 93 00 00 00 00"
        if [ $? != 0 ] ; then
            echo_fail "set_user_interface $default Fail"
            echo "set_user_interface $default Fail" >>$SAVEFILE
            error_quit
        fi
    fi
    echo_success "set_user_interface $default success!"
    echo "set_user_interface $default success!" >>$SAVEFILE
}
#升级白牌包
#传输文件
send_file_adduser()
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
	#先校验默认用户名和密码是否可用，若不可用，添加临时用户名和密码
	check_user2_default_name_pwd
	if [ $? != 0 ] ; then
	    echo "need to add a test user"
	    add_test_admin_user
	    user="DFTUpgradeTest"
	    pwd="Admin@9000"
	    FLAG_ADD_USER=1
	fi
	sleep 10
	expect << EOF
	set timeout 300
	spawn scp -6r -l 4096 ${src_file} ${user}@${bmc_ip}:${des_path}
	expect {
	"*yes/no" { send "yes\r"; exp_continue }
	"*password:" { send "$pwd\r" }
	}

	expect eof
EOF

	#若前面添加了临时用户，则要删除
	if [ $FLAG_ADD_USER == 1 ] ; then
	    restore_test_admin_user 17
	    echo " del the test user "
	fi
	return 0
}
#传输文件，升级，查询进度
start_wbdimg_upgrade()
{
    #查询升级文件是否存在
	getini $CONFIGFILE WBD_IMAGE_FILE
	hpm_file_path=$parameter
	if [ ! -f "$hpm_file_path" ] ; then
	   echo "hpm file $hpm_file_path is not present!"
	   echo "hpm file $hpm_file_path is not present!" >>$SAVEFILE
	   return 1
	fi
	
    #传输文件
	send_file_adduser "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" $hpm_file_path /tmp/image.hpm
	if [ $? != 0 ] ; then	 
	   echo "send hpm file fail!"
	   echo "send hpm file fail!" >>$SAVEFILE
	   return 1
	fi
	
	#启动升级
	initiate_bmc_upgrade
	if [ $? != 0 ] ; then
       echo "init bmc upgrade fail" >>$SAVEFILE
	   return 1	
	fi
	
	#查询进度
	get_bmc_upgrade_status
	if [ $? != 0 ] ; then	   
	   return 1	
	fi
	
	return 0
}
#hpm加载bmc软件
hpm_upgrade()
{
	date >>$SAVEFILE
	#EQUIPMENT_SEPARATION_SUPPORT_STATE为0表示不支持装备分离，保持原有流程，开启dft验证
	if [ "$EQUIPMENT_SEPARATION_SUPPORT_STATE" == 0 ];then
		DFT_enable
		sleep 1
	fi
	getini $CONFIGFILE WBD_CUSTOMIZE
	wbdcustomize=$parameter
	if [ "$wbdcustomize" != "on" ] ; then
	   echo "wbdcustomize is off!"
	   echo "wbdcustomize is off!" >>$SAVEFILE
	   return 0
	fi
	#安装驱动
	load_driver

	for((k=0;k<$REPEAT;k++))
	{
		start_wbdimg_upgrade
		
		if [ $? == 0 ] ; then
			echo "hpm load BMC ok"
			return 0
		fi
	}
	echo_fail "hpm load BMC fail!"
	echo "hpm load BMC fail!" >>$SAVEFILE	
	error_quit
}


main()
{
	chmod +x *
	
	#白牌包升级，防止添加用户失败，hpm_upgrade从sys_config.sh移动到此处
	hpm_upgrade	
	#启用系统侧对BMC用户管理
	write_security_enhance_config 7 1
	#特殊ID,历史密码检查,弱口令检查
	write_security_enhance_config 4 0
	write_security_enhance_config 5 0	
    
	#校验设备是否支持弱口令字典
	get_prop_cmd 48 "ff ff 00 01 00" 9 "09 00 79 01 01"
	if [ $? == 0 ] ; then
		write_security_enhance_config 15 0
	fi
	
	#2.1、设置用户列表 
	#取消密码复杂度
	restore_LocalUserPasswdCheck_disable	
	
	#还原root密码
	config_admin_user

    #由于user17被用作临时用户，因此固定还原此用户
    restore_user_name 17
	
	#2.2、配置其他用户
	for((m=3;m<17;m++))
	{
		getini  $CONFIGFILE  BMCSet_CustomName$m
		USER_NAME=$parameter
		
		
		if [ "$USER_NAME" = "" ]; then
			restore_user_name $m
			sleep 1
		else
			set_user_name $m
			sleep 1
			# 设置用户接口支持情况
			set_user_interface $m
			sleep 1
			getini  $CONFIGFILE  BMCSet_CustomNamePassword$m
			USER_PASSWD=$parameter
			if [ "$USER_PASSWD" != "" ]; then
			   set_user_passwd $m
			   sleep 1
			fi
		
			getini  $CONFIGFILE  BMCSet_CustomNameAccess$m
			USER_ACCESS=$parameter
			if [ "$USER_ACCESS" != "" ]; then
				set_user_access $m
				sleep 1
			fi
			
			
			# 清除用户锁定状态
			set_user_unlock $m
			
			#通过设置属性万能命令设置SNMP加密密码状态为初始状态
			set_user_snmp_privacy_passwd_initial_state $m	
            
			# 密码策略重置为仅提示修改
			set_user_first_login_password_reset_policy $m 1		
		fi	
	}
	echo_success "restore other users success!"
	echo "restore other users success!" >>$SAVEFILE
    
	#配置userid=2用户权限
	getini  $CONFIGFILE  Custom_AdminAccess
	USER_ACCESS=$parameter
	if [ "$USER_ACCESS" != "" ] ; then
		set_user_access 2
		sleep 1
	fi

	# 设置用户权限会同步snmp，会导致snmp重启，需等待5秒
	sleep 5
	#设置团体名前先禁用超长口令
	disable_long_passwd
	#团体名与密码复杂度关联
	#设置只读团体名
	getini $CONFIGFILE Custom_MIBCommunity_RO
	MIBCommunity=$parameter
	if [ "$MIBCommunity" != "" ]; then
		config_snmp_mib_Community 1 $MIBCommunity
		if [ $? != 0 ] ; then
			echo_fail "Set MIBCommunity_RO $MIBCommunity failed"
			echo "Set MIBCommunity_RO $MIBCommunity failed">>$SAVEFILE
			error_quit
		else
			echo_success "Set MIBCommunity_RO $MIBCommunity success"
			echo "Set MIBCommunity_RO $MIBCommunity success">>$SAVEFILE
		fi
	fi
	
	# 设置完只读团体名，snmp模块会重启，需等待5秒
	sleep 5
	#设置读写团体名
	getini $CONFIGFILE Custom_MIBCommunity_RW
	MIBCommunity_RW=$parameter
	if [ "$MIBCommunity_RW" != "" ] ;then
		config_snmp_mib_Community 2 $MIBCommunity_RW
		if [ $? != 0 ] ; then
			echo_fail "Set MIBCommunity_RW $MIBCommunity_RW failed"
			echo "Set MIBCommunity_RW $MIBCommunity_RW failed">>$SAVEFILE
			error_quit
		else
			echo_success "Set MIBCommunity_RW $MIBCommunity_RW success"
			echo "Set MIBCommunity_RW $MIBCommunity_RW success">>$SAVEFILE
		fi
	fi

	#超长口令
	getini $CONFIGFILE BMCSet_LongPasswordEnable
	LongPasswordEnable=$parameter

	if [ "$LongPasswordEnable" = "" ] ; then
		if [ "$PRODUCT_VERSION_NUM" == "03" ] ; then
			set_long_passwd_enable 0  #V3产品默认关闭
			echo "set_long_passwd_enable 0 success"			
		else
			set_long_passwd_enable 1  #V5及以上产品默认开启
			echo "set_long_passwd_enable 1 success"			
		fi
	else
		set_long_passwd_enable $LongPasswordEnable
		echo "set_long_passwd_enable $LongPasswordEnable success"
	fi

	#2.3、本地用户密码复杂度检查
	#先判断是否为开启不可取消密码复杂度检查
	getini  $CONFIGFILE  BMCSet_UserPasswdComplexityForeverEnable
	USER_PASSWD_COMPLEXITY_CHECK=$parameter
	if [ "$USER_PASSWD_COMPLEXITY_CHECK" = "on" ] ; then
		set_LocalUserPasswdCheck_ForeverEnable
		sleep 1
	else		
		#判断是否为开启可配密码复杂度检查
		getini  $CONFIGFILE  BMCSet_UserPasswdComplexityCheckEnable
		USER_PASSWD_COMPLEXITY_CHECK=$parameter
		
		if [ "$USER_PASSWD_COMPLEXITY_CHECK" = "off" ] ; then
			set_LocalUserPasswdCheck_disable
			sleep 1
		else			
			set_LocalUserPasswdCheck_enable
			sleep 1
		fi
	fi

	#配置密码有效期
	getini $CONFIGFILE BMCSet_CustomExpiredTime
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_ExpiredTime
		VALUE=$parameter
		if  [ "$VALUE" = "" ] || [ "$VALUE" -gt "365" ] ; then
			echo_fail "Custom_ExpiredTime invaild! set Custom_ExpiredTime $VALUE failed!"
			echo "Custom_ExpiredTime invaild! set Custom_ExpiredTime $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=0
	fi
	
	write_security_enhance_config 3 $VALUE

	#配置重复密码保留个数
	getini $CONFIGFILE BMCSet_CustomSamePwdCheckCount
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_SamePwdCheckCount
		VALUE=$parameter
		if  [ "$VALUE" = "" ] || [ "$VALUE" -gt "5" ] ; then
			echo_fail "Custom_SamePwdCheckCount invaild! set Custom_SamePwdCheckCount $VALUE failed!"
			echo "Custom_SamePwdCheckCount invaild! set Custom_SamePwdCheckCount $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=5
	fi
	write_security_enhance_config 5 $VALUE

	#配置失败锁定次数
	getini $CONFIGFILE BMCSet_CustomFailedLoginCount
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_FailedLoginCount
		VALUE=$parameter
		if [ "$VALUE" = "" ] || [ "$VALUE" -lt "0" ] || [ "$VALUE" -gt "6" ] ; then
			echo_fail "Custom_FailedLoginCount invaild! set Custom_FailedLoginCount $VALUE failed!"
			echo "Custom_FailedLoginCount invaild! set Custom_FailedLoginCount $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=5
	fi
	write_security_enhance_config 2 $VALUE

	#配置失败锁定时间
	getini $CONFIGFILE BMCSet_CustomForbidLoginTime
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_ForbidLoginTime
		VALUE=$parameter
		if [ "$VALUE" = "" ] || [ "$VALUE" -lt "1" ] || [ "$VALUE" -gt "30" ] ; then
			echo_fail "Custom_ForbidLoginTime invaild! set Custom_ForbidLoginTime $VALUE failed!"
			echo "Custom_ForbidLoginTime invaild! set Custom_ForbidLoginTime $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=5
	fi
	VALUE=`expr 60 \* $VALUE`
	write_security_enhance_config 1 $VALUE

	#配置TLS协议
	getini $CONFIGFILE BMCSet_CustomTLSProtocol
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_TLSProtocol
		VALUE=$parameter
		if [[ !("$VALUE" =~ ^[4-5]$) ]] ; then
			echo_fail "Custom_TLSProtocol invaild! set Custom_TLSProtocol $VALUE failed!"
			echo "Custom_TLSProtocol invaild! set Custom_TLSProtocol $VALUE failed!" >>$SAVEFILE
			error_quit
        #TLS1.2 TLS1.3
        elif [ "$VALUE" -eq "4" ] ; then
            VALUE=0xfffc
        #TLS1.3            
        elif [ "$VALUE" -eq "5" ] ; then
            VALUE=0xfff8
		fi
    #空定制化 TLS1.2 TLS1.3
	else
		VALUE=0xfffc
	fi
	write_security_enhance_config 9 $VALUE
	
	#配置特殊用户
	getini $CONFIGFILE BMCSet_CustomExcludeUserID
	STATUS=$parameter
	if [ "$STATUS" = "on" ] ; then
		getini $CONFIGFILE Custom_ExcludeUserID
		VALUE=$parameter
		if [ "$VALUE" = "" ] || [ "$VALUE" -gt "17" ] ; then
			echo_fail "Custom_ExcludeUserID invaild! set Custom_ExcludeUserID $VALUE failed!"
			echo "Custom_ExcludeUserID invaild! set Custom_ExcludeUserID $VALUE failed!" >>$SAVEFILE
			error_quit
		fi
	else
		VALUE=0
	fi

	write_security_enhance_config 4 $VALUE
	
	#配置密码最短使用期限
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
		VALUE=0
	fi

	write_security_enhance_config 10 $VALUE
	
	#配置安全公告使能
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
	write_security_enhance_config 12 $VALUE

	#配置默认安全公告
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
	
	write_security_enhance_config 13 "$VALUE"
    
	#配置二次认证使能
	getini $CONFIGFILE BMCSet_DoubleCertificationEnable
	STATUS=$parameter
	if [ "$STATUS" != "on" ] && [ "$STATUS" != "off" ] && [ "$STATUS" != "" ] ; then
		echo_and_save_fail "Invalid Double Certification Enable switch($STATUS)"
		error_quit
	fi
	if [ "$STATUS" = "off" ] ; then
		VALUE=0
	else
		VALUE=1
	fi
	
	write_security_enhance_config 14 $VALUE
    
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

		write_security_enhance_config 15 $VALUE
	else
		echo "Device does not support weak password dictionary check."
	fi
	
	#校验是否提示初始密码
	getini $CONFIGFILE BMCSet_InitialPwdPrompt
	STATUS=$parameter
	if [ "$STATUS" = "off" ] ; then
		VALUE=0
	else
		VALUE=1
	fi    
	write_security_enhance_config 16 $VALUE
	
	#配置禁用不活动用户期限
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
		VALUE=0
	fi
	write_security_enhance_config 17 $VALUE
}

main
