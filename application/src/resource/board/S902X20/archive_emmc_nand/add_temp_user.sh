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

#��ȡ�汾��Ϣ
get_product_version_no()
{
	ipmicmd -k "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 07 00" smi 0 > product_version_num.txt
	if [ $? -eq 0 ] ; then
		PRODUCT_VERSION=`cat product_version_num.txt | awk '{print $NF}'`
	fi
	
	#����ֵff��ʾ��ȡ��Ʒ�汾�������֧�֣����Ի�û����ӣ�����϶���V3�Ĳ�Ʒ
	if [ "$PRODUCT_VERSION" == "ff"  -o "$PRODUCT_VERSION" == "FF" ] ; then
		PRODUCT_VERSION=03
	fi
	echo "PRODUCT VERSION : $PRODUCT_VERSION"
	echo "PRODUCT VERSION : $PRODUCT_VERSION" >>$SAVEFILE
	rm -f product_version_num.txt
}
#ɾ����ӵ���ʱ�û�
del_user_name()
{
	 userid=`printf %2x $1`
	 run_cmd "f 00 06 45 $userid 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" "0f 07 00 45 00"
	 if [ $? != 0 ] ; then
		run_cmd "f 00 06 45 $userid 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" "0f 07 00 45 00"
		if [ $? != 0 ] ; then
		    echo_fail "del user$1 name fail!"
			echo "del user$1 name fail!" >>$SAVEFILE
			error_quit
		fi
    else
        echo "del user$1 name success!" >>$SAVEFILE
        return 0
    fi
}

#У��ָ��User ID�û��Ƿ�Ϊ�գ������Ϊ����ֱ��ɾ��
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

#У��user2�Ƿ���Ĭ���û���������
check_user2_default_name_pwd()
{ 
	user_name_result=1
   #��ȡ�汾��Ϣ
   get_product_version_no
   if [ "$PRODUCT_VERSION" != "03" ]; then
       #У��User2���û����Ƿ�ΪAdministrator
       #��Ϊipmicmd�����⣬���ص��ַ���������00�뵹����2��00֮���кܶ�ո������run_cmd����ȫƥ���
       #��˲�����run_cmd�����ñ���ipmicmd�ķ���ֵ���ֶ�����grepƥ��ķ�ʽ����ƥ���ʱ��Ҫд��һ��0	   
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
	     #У�������Ƿ�ΪAdmin@9000
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
       #У��User2���û����Ƿ�Ϊroot
	   #��Ϊipmicmd�����⣬���ص��ַ���������00�뵹����2��00֮���кܶ�ո������run_cmd����ȫƥ���
       #��˲�����run_cmd�����ñ���ipmicmd�ķ���ֵ���ֶ�����grepƥ��ķ�ʽ����ƥ���ʱ��Ҫд��һ��0	   
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
	     #У�������Ƿ�ΪHuawei12#$
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

#������ʱ�û���
set_user_name()
{
	#ɾ�����ܴ��ڵ�user 17,����Ϊuser 17��֧�ֶ��ƣ�
	del_existing_user 17
	sleep 5
	#������ʱ�û���DFTUpgradeTest
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

#�����û�����
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

#�����û�Ȩ��
set_user_access()
{
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
	
    #�������Ҫ����00,����IPMI�Ǳ߶Ը�����������жϣ�ʹ���û�
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

#�����û��״ε�½�����޸Ĳ���
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


#ʹ���û���ssh�ӿ�
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

#����Ĭ�Ϲ���Ա�ʻ���Ϣ
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
	sleep 5
	set_user_passwd 17
	sleep 5
	set_user_access 17
	sleep 5
	#/* END:   Modified for PN:DTS2013051500991 by w00208253, 2013/5/21 */
	
	set_user_ssh_interface 17
	sleep 5
	# δ�����޸�Ԥ���˻�ʱ���轫�������������Ϊ����ʾ�޸�
	set_user_first_login_password_reset_policy 17 1
	sleep 5
}

# ��ȡ������ʹ��
get_password_weak_enable_state()
{
	run_cmd  "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 09 00" "0f 31 00 93 00 db 07 00 00 09 00 79 01 01"
	if [ $? -eq 0 ] ; then
		PASSWORD_WEAK_SUPPORT=01
		echo " password weak support $PASSWORD_WEAK_SUPPORT"
	fi
	
	if [ $PASSWORD_WEAK_SUPPORT -eq 01 ] ; then
	    echo " get the password weak state"
		run_cmd  "0f 00 30 93 db 07 00 36 02 00 01 20 00 00 ff ff 00 01 00 0f 00" "0f 31 00 93 00 db 07 00 00 0f 00 79 01 01"
		if [ $? -eq 0 ] ; then
			PASSWORD_WEAK_STATE=01
			echo " password weak enable $PASSWORD_WEAK_STATE"
		fi
	fi
	rm -f passwd_weak_state_temp.txt
}
#��ȡ���븴�Ӷȼ��ʹ��״̬
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

#�������븴�Ӷȼ��Ϊdisable״̬
set_LocalUserPasswdComplexity_disable()
{
	run_cmd  "0f 00 30 93 db 07 00 22 00"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user passwd complexity disable fail"
		echo "set user passwd complexity disable fail" >>$SAVEFILE
		error_quit
	fi
}

#�ָ�ԭ�е�����ʹ��״̬
restore_LocalUserPasswdComplexity()
{
	value=`printf %2x $PASSWORD_COMPLEXITY_STATE`
	run_cmd  "f 0 30 93 db 07 00 22 $value"  "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "restore user passwd complexity fail"
		echo "restore user passwd complexity fail" >>$SAVEFILE
		error_quit
	fi
}

#�����������Ϊenable
set_password_weak_enable()
{
	run_cmd "0f 00 30 93 db 07 00 35 02 00 01 00 00 00 ff ff 00 01 00 0f 00 00 01 01" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user passwd weak enable fail"
		echo "set user passwd weak enable fail" >>$SAVEFILE
		error_quit
	fi
}

#�����������Ϊdisable
set_password_weak_disable()
{
	run_cmd "0f 00 30 93 db 07 00 35 02 00 01 00 00 00 ff ff 00 01 00 0f 00 00 01 00" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user passwd weak disable fail"
		echo "set user passwd weak disable fail" >>$SAVEFILE
		error_quit
	fi
}

#�����ʱ�û� 
add_test_admin_user()
{    
	#У���豸�Ƿ�֧���������ֵ�,�����ǰ��������enable�ģ���Ҫdisable
	get_password_weak_enable_state
	if [ $PASSWORD_WEAK_STATE -eq 01 -a $PASSWORD_WEAK_SUPPORT -eq 01 ] ; then
		set_password_weak_disable
	fi
	
	#������븴�Ӷȼ�鿪�أ������ǰ���븴�Ӷ���enable�ģ���Ҫdisable
	get_LocalUserPasswdComplexity
	if [ $PASSWORD_COMPLEXITY_STATE != 00 ] ; then
		 set_LocalUserPasswdComplexity_disable
	fi
	
	#�����ʱ�û�
	echo "add_test_admin_user"
	add_admin_user	
}

#ɾ����ʱ�û�
restore_test_admin_user()
{
	#�ָ�������ʹ��״̬
	if [ $PASSWORD_WEAK_STATE -eq 01 -a $PASSWORD_WEAK_SUPPORT -eq 01 ] ; then
		set_password_weak_enable
	fi
	
	#�ָ����븴�Ӷȼ�鿪��
	if [ $PASSWORD_COMPLEXITY_STATE != 00 ] ; then
		 restore_LocalUserPasswdComplexity 
	fi
	
	#ɾ���û���
	del_user_name 17

}
