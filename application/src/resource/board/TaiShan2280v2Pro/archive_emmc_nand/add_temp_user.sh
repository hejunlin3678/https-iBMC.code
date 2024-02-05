#!/bin/sh

PASSWORD_WEAK_STATE=0
PASSWORD_WEAK_SUPPORT=0
PASSWORD_COMPLEXITY_STATE=0
PRODUCT_VERSION=03

error_quit()
{
    exit 1
}

ok_quit()
{
    exit 0
}

echo_fail()
{
	echo -e "\033[31;1m$1\033[0m"
}

echo_success()
{
	echo -e "\033[32;1m$1\033[0m"
}

#获取版本信息
get_product_version_no()
{
	ipmicmd -k "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 07 00" smi 0 > product_version_num.txt
	if [ $? -eq 0 ] ; then
		PRODUCT_VERSION=`cat product_version_num.txt | awk '{print $NF}'`
	fi
	
	#返回值ff表示获取产品版本的命令还不支持，属性还没有添加，这个肯定是V3的产品
	if [ "$PRODUCT_VERSION" == "ff"  -o "$PRODUCT_VERSION" == "FF" ] ; then
		PRODUCT_VERSION=03
	fi
	echo "PRODUCT VERSION : $PRODUCT_VERSION"
	echo "PRODUCT VERSION : $PRODUCT_VERSION" >>$SAVEFILE
	rm -f product_version_num.txt
}
#删除添加的临时用户
del_user_name()
{
	 userid=`printf %2x $1`
	 run_cmd "f 00 06 45 $userid 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" "0f 07 00 45 00"
	 if [ $? != 0 ] ; then
		run_cmd "f 00 06 45 $userid 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" "0f 07 00 45 00"
		if [ $? != 0 ] ; then
		    echo_fail "del user$1 name fail!"
			echo "del user$1 name fail!" >>$SAVEFILE
			exit 1
		fi
    else
        echo "del user$1 name success!" >>$SAVEFILE
        return 0
    fi
}

#校验指定User ID用户是否为空，如果不为空则直接删除
del_existing_user()
{
    userid=`printf %2x $1`
    run_cmd "f 00 06 46 $userid" "0f 07 00 46 00 00 00 00 00 00 00 00 00 00"
    if [ $? != 0 ] ; then
        echo_fail "user$1 is already, just delete it"
        echo "user$1 is already, just delete it" >>$SAVEFILE
        del_user_name $1
    fi
    return 0
}

#校验user2是否是默认用户名和密码
check_user2_default_name_pwd()
{ 
	user_name_result=1
   #获取版本信息
   get_product_version_no
   if [ "$PRODUCT_VERSION" != "03" ]; then
       #校验User2的用户名是否为Administrator
       #run_cmd "f 00 06 46 02" "0f 07 00 46 00 41 64 6d 69 6e 69 73 74 72 61 74 6f 72 00 00 00"
       #因为ipmicmd有问题，返回的字符串在最后的00与倒数第2个00之间有很多空格符，而run_cmd是完全匹配的
       #因此不能用run_cmd，改用保存ipmicmd的返回值再手动调用grep匹配的方式，且匹配的时候要写少一个0	   
	   ipmicmd -k "f 00 06 46 02" smi 0 > user_name_temp.txt
	   if [ $? -eq 0 ] ; then
		  cat user_name_temp.txt | grep -i "0f 07 00 46 00 41 64 6d 69 6e 69 73 74 72 61 74 6f 72 00 00"
		  user_name_result=$?
		  rm -f user_name_temp.txt
	   fi
	   if [ $user_name_result != 0 ] ; then
		  echo "user2 is not default name"
		  echo "user2 is not default name" >>$SAVEFILE
		  return 1
	   else
	     #校验密码是否为Admin@9000
	     run_cmd "f 00 06 47 02 03 41 64 6D 69 6E 40 39 30 30 30 00 00 00 00 00 00" "0f 07 00 47 00"
		 if [ $? != 0 ] ; then
		    echo "user2 is not default pwd"
		    echo "user2 is not default pwd" >>$SAVEFILE
			return 1
		 else
		    return 0
		 fi
	   fi
   else
       #校验User2的用户名是否为root
       #run_cmd "f 00 06 46 02" "0f 07 00 46 00 72 6F 6F 74 00 00 00 00 00 00 00 00 00 00 00"
	   #因为ipmicmd有问题，返回的字符串在最后的00与倒数第2个00之间有很多空格符，而run_cmd是完全匹配的
       #因此不能用run_cmd，改用保存ipmicmd的返回值再手动调用grep匹配的方式，且匹配的时候要写少一个0	   
	   ipmicmd -k "f 00 06 46 02" smi 0 > user_name_temp.txt
	   if [ $? -eq 0 ] ; then
		  cat user_name_temp.txt | grep -i "0f 07 00 46 00 72 6F 6F 74 00 00 00 00 00 00 00 00 00 00 00"
		  user_name_result=$?
		  rm -f user_name_temp.txt
	   fi
	   if [ $user_name_result != 0 ] ; then
		  echo "user2 is not default name"
		  echo "user2 is not default name" >>$SAVEFILE
		  return 1
	   else
	     #校验密码是否为Huawei12#$
	     run_cmd "f 00 06 47 02 03 48 75 61 77 65 69 31 32 23 24 00 00 00 00 00 00" "0f 07 00 47 00"
		 if [ $? != 0 ] ; then
		    echo "user2 is not default pwd"
		    echo "user2 is not default pwd" >>$SAVEFILE
			return 1
		 else
		    return 0
		 fi
	   fi
   fi
}

#设置临时用户名
set_user_name()
{
	#删除可能存在的user 17,（因为user 17不支持定制）
	del_existing_user 17

	#设置临时用户名DFTUpgradeTest
    userid=`printf %2x $1`
    run_cmd "f 00 06 45 $userid 44 46 54 55 70 67 72 61 64 65 54 65 73 74 00 00" "0f 07 00 45 00"
    if [ $? != 0 ] ; then
        echo_fail "Set user$1 name fail!"
        echo "Set user$1 name fail!" >>$SAVEFILE
        error_quit 
    else
        echo_success "Set user$1 name success!"
        echo "Set user$1 name success!" >>$SAVEFILE
        return 0
    fi
}

#设置用户密码
set_user_passwd()
{
    userid=`expr 128 + $1`
    userid=`printf %02x $userid`
    run_cmd "f 00 06 47 $userid 02 41 64 6D 69 6E 40 39 30 30 30 00 00 00 00 00 00 00 00 00 00 " "0f 07 00 47 00"

    if [ $? != 0 ] ; then
		echo_fail "Set user$1 passwd fail!"
		echo "Set user$1 passwd fail!" >>$SAVEFILE
		error_quit
    else
		echo_success "Set user$1 passwd success!"
		echo "Set user$1 passwd success!" >>$SAVEFILE
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
        error_quit
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

#使能用户的ssh接口和redfish接口
set_user_ssh_interface()
{
	key_value=`printf %2x $1`
	run_cmd "0f 00 30 93 db 07 00 35 34 00 01 00 00 00 01 00 79 01 $key_value 03 00 75 04 88 00 00 00" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user ssh interface fail"
		echo "set user ssh interface fail"  >>$SAVEFILE
		error_quit
	fi
}

#配置默认管理员帐户信息
add_admin_user()
{
	#/* BEGIN: Modified for PN:DTS2013051500991 by w00208253, 2013/5/21 */
	USER_NAME="root"
	USER_PASSWD="Huawei12#$"
	USER_ACCESS="admin"
	
	if [ "$PRODUCT_VERSION" != "03" ]; then
		USER_NAME="Administrator"
		USER_PASSWD="Admin@9000"
	fi
	echo "add_admin_user"
	set_user_name 17
	sleep 1
	set_user_passwd 17
	sleep 2
	set_user_access 17
	sleep 1
	#/* END:   Modified for PN:DTS2013051500991 by w00208253, 2013/5/21 */
	
	set_user_ssh_interface 17
	sleep 1

	#禁用firstlogin
    set_user_first_login_password_reset_policy 17 0
    sleep 1
}

# 获取弱口令使能
get_password_weak_enable_state()
{
	#ipmicmd -k "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 09 00" smi 0 > passwd_weak_state_temp.txt
	run_cmd  "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 09 00" "0f 31 00 93 00 db 07 00 00 09 00 79 01 01"
	if [ $? -eq 0 ] ; then
		PASSWORD_WEAK_SUPPORT=01
		echo " password weak support $PASSWORD_WEAK_SUPPORT"
	fi
	
	if [ $PASSWORD_WEAK_SUPPORT -eq 01 ] ; then
	    echo " get the password weak state"
	    #ipmicmd -k "0f 00 30 93 db 07 00 36 02 00 01 20 00 00 ff ff 00 01 00 0f 00" smi 0 > passwd_weak_state_temp.txt
		run_cmd  "0f 00 30 93 db 07 00 36 02 00 01 20 00 00 ff ff 00 01 00 0f 00" "0f 31 00 93 00 db 07 00 00 0f 00 79 01 01"
		if [ $? -eq 0 ] ; then
			PASSWORD_WEAK_STATE=01
			echo " password weak enable $PASSWORD_WEAK_STATE"
		fi
	fi
	rm -f passwd_weak_state_temp.txt
}
#获取密码复杂度检查使能状态
get_LocalUserPasswdComplexity()
{
	ipmicmd -k "0f 00 30 93 db 07 00 21" smi 0 > pwsd_complexity_temp.txt
	if [ $? -eq 0 ] ; then
		cat pwsd_complexity_temp.txt
		PASSWORD_COMPLEXITY_STATE=`cat pwsd_complexity_temp.txt | awk '{print $NF}'`
		echo " password weak support $PASSWORD_COMPLEXITY_STATE"
	fi
	rm -f pwsd_complexity_temp.txt
}

#设置密码复杂度检查为disable状态
set_LocalUserPasswdComplexity_disable()
{
	run_cmd  "0f 00 30 93 db 07 00 22 00"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user passwd complexity disable fail"
		echo "set user passwd complexity disable fail" >>$SAVEFILE
		error_quit
	fi
}

#恢复原有的密码使能状态
restore_LocalUserPasswdComplexity()
{
	value=`printf %2x $PASSWORD_COMPLEXITY_STATE`
	run_cmd  "f 0 30 93 db 07 00 22 $value"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "restore user passwd complexity fail"
		echo "restore user passwd complexity fail" >>$SAVEFILE
		exit 1
	fi
}

#设置弱口令开关为enable
set_password_weak_enable()
{
	run_cmd "0f 00 30 93 db 07 00 35 02 00 01 00 00 00 ff ff 00 01 00 0f 00 00 01 01" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user passwd weak enable fail"
		echo "set user passwd weak enable fail" >>$SAVEFILE
		exit 1
	fi
}

#设置弱口令开关为disable
set_password_weak_disable()
{
	run_cmd "0f 00 30 93 db 07 00 35 02 00 01 00 00 00 ff ff 00 01 00 0f 00 00 01 00" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user passwd weak disable fail"
		echo "set user passwd weak disable fail" >>$SAVEFILE
		error_quit
	fi
}

#添加临时用户 
add_test_admin_user()
{    
	#校验设备是否支持弱口令字典,如果当前弱口令是enable的，则要disable
	get_password_weak_enable_state
	if [ $PASSWORD_WEAK_STATE -eq 01 -a $PASSWORD_WEAK_SUPPORT -eq 01 ] ; then
		set_password_weak_disable
	fi
	
	#检查密码复杂度检查开关，如果当前密码复杂度是enable的，则要disable
	get_LocalUserPasswdComplexity
	if [ $PASSWORD_COMPLEXITY_STATE != 00 ] ; then
		 set_LocalUserPasswdComplexity_disable
	fi
	
	#添加临时用户
	echo "add_test_admin_user"
	add_admin_user	
}

#删除临时用户
restore_test_admin_user()
{
	#恢复弱口令使能状态
	if [ $PASSWORD_WEAK_STATE -eq 01 -a $PASSWORD_WEAK_SUPPORT -eq 01 ] ; then
		set_password_weak_enable
	fi
	
	#恢复密码复杂度检查开关
	if [ $PASSWORD_COMPLEXITY_STATE != 00 ] ; then
		 restore_LocalUserPasswdComplexity 
	fi
	
	#删除用户名
	del_user_name 17

}
 
#设置用户首次登陆密码修改策略[不校验结果，仅记录日志]
set_user_first_login_password_reset_policy()
{
	userid=`printf %02x $1`
	value=`printf %02x $2`
	run_cmd "f 00 30 93 db 07 00 35 34 00 01 00 00 00 01 00 79 01 $userid 0b 00 79 01 $value" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "Set user$1 first login policy to $2 fail!"
        echo "Set userid is $userid!"
		echo "Set user$1 first login policy to $2 fail!" >>$SAVEFILE
    else
        echo_success "Set user$1 first login policy to $2 success!"
        echo "Set userid is $userid!"
        echo "Set user$1 first login policy to $2 success!" >>$SAVEFILE
	fi
 
    return 0
}