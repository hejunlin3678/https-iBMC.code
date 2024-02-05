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
	value=$2
	
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
	elif [ "$type" == "14" ] ;then
		TypeString="DoubleCertification"	
	elif [ "$type" == "15" ] ;then
		TypeString="WeakPwdDictEnable"		
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
	elif [ "$type" == "3" ] || [ "$type" == "10" ] ; then
		info_type=75
		info_len=4
		info_val=$(dword2string $value)
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
    if [ "$PRODUCT_VERSION_NUM" == "03" ];then
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
	
	if [ "$PRODUCT_VERSION_NUM" != "03" ]; then
		USER_NAME="Administrator"
		USER_PASSWD="Admin@9000"
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
restore_admin_user()
{
	chmod +x *
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
	
	#还原Aministrator密码
	config_admin_user	
}
