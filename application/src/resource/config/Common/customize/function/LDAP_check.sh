#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#LDAP使能校验
check_LDAP_enable()
{
	fun_id=58
	key_id=$(word2string 0xffff)
	key_type=79 #y
	key_len=01
	key_val=01
	info_id=$(word2string 1) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 1 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAP Enable is $info_val fail"
		echo "Check LDAP Enable is $info_val fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAP Enable is $info_val success"
		echo "Check LDAP Enable is $info_val success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器地址校验
check_LDAP_hostaddr()
{
	if [ "$2" != "" ] ; then
		hostaddr=$2
		strlen=`echo ${#hostaddr}`
		hostaddr=$(string2hex "$hostaddr")
	else
		#删除服务器地址
		hostaddr=""
		strlen=0
	fi
	
	fun_id=59
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len=01
	key_val=`printf "%02x" $1`
	info_id=$(word2string 2) 
	info_type=73 #s
	info_len=`printf "%02x" $strlen` 
	info_val=$hostaddr
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 2 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAP$key_val host addr fail"
		echo "Check LDAP$key_val host addr fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAP$key_val host addr success"
		echo "Check LDAP$key_val host addr success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器域名校验
check_LDAP_userdomain()
{
	if [ "$2" != "" ] ; then
		userdomain=$2
		strlen=`echo ${#userdomain}`
		userdomain=$(string2hex "$userdomain")
	else
		#删除服务器地址
		userdomain=""
		strlen=0
	fi
	
	fun_id=59
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 3)
	info_type=73 #s
	info_len=`printf "%02x" $strlen`
	info_val=$userdomain
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 3 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAP$key_val userdomain fail"
		echo "Check LDAP$key_val userdomain fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAP$key_val userdomain success"
		echo "Check LDAP$key_val userdomain success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器用户文件加
check_LDAP_userfolder()
{
	if [ "$2" != "" ] ; then
		userfolder=$2
		strlen=`echo ${#userfolder}`
		userfolder=$(string2hex "$userfolder")
	else
		#删除服务器地址
		userfolder=""
		strlen=0
	fi
	
	fun_id=59
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 6)
	info_type=73 #s
	info_len=`printf "%02x" $strlen`
	info_val=$userfolder
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 6 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAP$key_val userfolder fail"
		echo "Check LDAP$key_val userfolder fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAP$key_val userfolder success"
		echo "Check LDAP$key_val userfolder success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器端口校验
check_LDAP_port()
{
	fun_id=59
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 4)
	info_type=69 #i
	info_len="04"
	info_val=$(dword2string $2)
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 4 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check  LDAP$key_val port fail"
		echo "Check LDAP$key_val port fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAP$key_val port success"
		echo "Check LDAP$key_val port success" >>$SAVEFILE
		return 0
   	fi	
}

#LDAP服务器证书状态校验
check_LDAP_certstatus()
{
	fun_id=59
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 5)
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $2`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 5 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAP$key_val certstatus fail"
		echo "Check LDAP$key_val certstatus fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAP$key_val certstatus success"
		echo "Check LDAP$key_val certstatus success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器组名校验
check_LDAP_groupname()
{
	if [ "$2" != "" ] ; then
		groupname=$2
		strlen=`echo ${#groupname}`
		groupname=$(string2hex "$groupname")
	else
		#删除LDAP组名
		groupname=""
		strlen=0
	fi
	
	fun_id=53
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 3)
	info_type=73 #s
	info_len=`printf "%02x" $strlen`
	info_val=$groupname
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 3 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAPGroup $1 groupname fail"
		echo "Check LDAPGroup $1 groupname fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAPGroup $1 groupname success"
		echo "Check LDAPGroup $1 groupname success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器组应用文件夹校验
check_LDAP_groupfolder()
{
	if [ "$2" != "" ] ; then
		groupfolder=$2
		strlen=`echo ${#groupfolder}`
		groupfolder=$(string2hex "$groupfolder")
	else
		#删除LDAP组应用文件夹
		groupfolder=""
		strlen=0
	fi
	
	fun_id=53
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 7)
	info_type=73 #s
	info_len=`printf "%02x" $strlen`
	info_val=$groupfolder
	
	expect_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 7 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check LDAPGroup $1 groupfolder fail"
		echo "Check LDAPGroup $1 groupfolder fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check LDAPGroup $1 groupfolder success"
		echo "Check LDAPGroup $1 groupfolder success" >>$SAVEFILE
		return 0
	fi
}

#LDAP服务器登陆规则校验
check_LDAP_permitruleids()
{
	fun_id=53
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 2)
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $2`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 2 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAPGroup $1 permitruleids fail"
		echo "Check LDAPGroup $1 permitruleids fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAPGroup $1 permitruleids success"
		echo "Check LDAPGroup $1 permitruleids success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器权限校验
check_LDAP_privilege()
{
	fun_id=53
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 4)
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $2`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 4 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAPGroup $1 permitruleids fail"
		echo "Check LDAPGroup $1 permitruleids fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAPGroup $1 permitruleids success"
		echo "Check LDAPGroup $1 permitruleids success" >>$SAVEFILE
		return 0
   	fi
}

#LDAP服务器UserRoleId
check_LDAP_user_role_id()
{
	fun_id=53
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 8)
	info_type=79 #y
	info_len="01"
	info_val=`printf "%02x" $2`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 8 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAPGroup $1 UserRoleId fail"
		echo "Check LDAPGroup $1 UserRoleId fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAPGroup $1 UserRoleId success"
		echo "Check LDAPGroup $1 UserRoleId success" >>$SAVEFILE
		return 0
   	fi
}


#检查LDAP组登录接口
check_bmc_ldapgroup_logininterface()
{
	#把其他接口设置为1， 4294967158=0xFFFFFF76
	tmp=$(($2 + 4294967158))
	
	fun_id=53
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 5)
	info_type=75 #u
	info_len="04"
	info_val=$(dword2string $tmp)
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 5 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check LDAPGroup $1 login interface fail"
		echo "Check LDAPGroup $1 login interface fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check LDAPGroup $1 login interface success"
		echo "Check LDAPGroup $1 login interface success" >>$SAVEFILE
		return 0
   	fi
}

main()
{
	#设置LDAP使能开关
	getini $CONFIGFILE BMCSet_LDAPEnable
	LDAPEnable=$parameter
	
	check_LDAP_enable $LDAPEnable
	if [ $? != 0 ]; then
		echo_fail "Check LDAP Enable $LDAPEnable failed"
		echo "Check LDAP Enable $LDAPEnable failed">>$SAVEFILE
		error_quit
	else
		echo_success "Check LDAP Enable $LDAPEnable success"
		echo "Check LDAP Enable $LDAPEnable success">>$SAVEFILE
	fi
		
	for((loopi=1;loopi<=6;loopi++))
	{
		#设置LDAP域服务器地址
		getini $CONFIGFILE BMCSet_LDAPHostAddr$loopi
		LDAPHostAddr=$parameter
		
		check_LDAP_hostaddr $loopi $LDAPHostAddr
		if [ $? != 0 ]; then
			echo_fail "Check LDAP$loopi host addr $LDAPHostAddr failed"
			echo "Check LDAP$loopi host addr $LDAPHostAddr failed">>$SAVEFILE
			error_quit
		else
			echo_success "Check LDAP$loopi host addr $LDAPHostAddr success"
			echo "Check LDAP$loopi host addr $LDAPHostAddr success">>$SAVEFILE
		fi
		
		#设置LDAP域名
		getini $CONFIGFILE BMCSet_LDAPUserDomain$loopi
		LDAPUserDomain=$parameter
		
		check_LDAP_userdomain $loopi $LDAPUserDomain
		if [ $? != 0 ]; then
			echo_fail "Check LDAP$loopi userdomain $LDAPUserDomain failed"
			echo "Check LDAP$loopi userdomain $LDAPUserDomain failed">>$SAVEFILE
			error_quit
		else
			echo_success "Check LDAP$loopi userdomain $LDAPUserDomain success"
			echo "Check LDAP$loopi userdomain $LDAPUserDomain success">>$SAVEFILE
		fi
		
		#设置LDAP用户文件夹
		getini $CONFIGFILE BMCSet_LDAPUserFolder$loopi
		LDAPUserFolder=$parameter
		
		check_LDAP_userfolder $loopi $LDAPUserFolder
		if [ $? != 0 ]; then
			echo_fail "Check LDAP$loopi userdomain $LDAPUserFolder failed"
			echo "Check LDAP$loopi userdomain $LDAPUserFolder failed">>$SAVEFILE
			error_quit
		else
			echo_success "Check LDAP$loopi userdomain $LDAPUserFolder success"
			echo "Check LDAP$loopi userdomain $LDAPUserFolder success">>$SAVEFILE
		fi
		
		#设置LDAP端口号
		getini $CONFIGFILE BMCSet_LDAPPort$loopi
		LDAPPort=$parameter
		if [ "$LDAPPort" == "" ]; then
			LDAPPort=636
		fi

		check_LDAP_port $loopi $LDAPPort
		if [ $? != 0 ]; then
			echo_fail "Check LDAP$loopi port $LDAPPort failed"
			echo "Check LDAP$loopi port $LDAPPort failed">>$SAVEFILE
			error_quit
		else
			echo_success "Check LDAP$loopi port $LDAPPort success"
			echo "Check LDAP$loopi port $LDAPPort success">>$SAVEFILE
		fi
		
		#设置证书状态
		getini $CONFIGFILE BMCSet_LDAPCertStatus$loopi
		LDAPCertStatus=$parameter
		if [ "$LDAPCertStatus" == "" ]; then
			#为空使用默认值 关闭 0
			LDAPCertStatus=0
		fi
		
		check_LDAP_certstatus $loopi $LDAPCertStatus
		if [ $? != 0 ]; then
			echo_fail "Check LDAP$loopi certstatus $LDAPCertStatus failed"
			echo "Check LDAP$loopi certstatus $LDAPCertStatus failed">>$SAVEFILE
			error_quit
		else
			echo_success "Check LDAP$loopi certstatus $LDAPCertStatus success"
			echo "Check LDAP$loopi certstatus $LDAPCertStatus success">>$SAVEFILE
		fi
	}
	
	for((j=0;j<=4;j++));do
		for((k=1;k<=6;k++));do
			#groupid_serverid = 0
			let "groupid_serverid=j*16+k"
			
			#设置LDAP组名
			getini $CONFIGFILE BMCSet_LDAPGroupName$groupid_serverid
			LDAPGroupName=$parameter
			if [ "$LDAPGroupName" == "" ]; then
				#为空使用默认值 关闭 0
				BMCSet_LDAPGroupName=""
			fi
			
			check_LDAP_groupname $groupid_serverid "$LDAPGroupName"
			if [ $? != 0 ]; then
				echo_fail "Check LDAPGroup$groupid_serverid groupname $LDAPGroupName failed"
				echo "Check LDAPGroup$groupid_serverid groupname $LDAPGroupName failed">>$SAVEFILE
				error_quit
			else
				echo_success "Check LDAPGroup$groupid_serverid groupname $LDAPGroupName success"
				echo "Check LDAPGroup$groupid_serverid groupname $LDAPGroupName success">>$SAVEFILE
			fi
			
			#设置LDAP组应用文件夹
			getini $CONFIGFILE BMCSet_LDAPGroupFolder$groupid_serverid
			LDAPGroupFolder=$parameter
			if [ "$LDAPGroupFolder" == "" ] ; then
				#为空使用默认值 关闭 0
				BMCSet_LDAPGroupFolder=""
			fi
			
			check_LDAP_groupfolder $groupid_serverid "$LDAPGroupFolder"
			if [ $? != 0 ]; then
				echo_fail "Check LDAPGroup$groupid_serverid groupfolder $LDAPGroupFolder failed"
				echo "Check LDAPGroup$groupid_serverid groupfolder $LDAPGroupFolder filed">>$SAVEFILE
				error_quit
			else
				echo_success "Check LDAPGroup$groupid_serverid groupfolder $LDAPGroupFolder success"
				echo "Check LDAPGroup$groupid_serverid groupfolder $LDAPGroupFolder success">>$SAVEFILE
			fi
			
			#设置LDAP组权限
			getini $CONFIGFILE BMCSet_LDAPGroupUserRoleId$groupid_serverid
			BMCSet_LDAPGroupUserRoleId=$parameter;
			if [ "$BMCSet_LDAPGroupUserRoleId" == "" ]; then
				#为空使用默认值 关闭 0
				BMCSet_LDAPGroupUserRoleId=0
			else
				#校验输入RoleId
				checkInt $BMCSet_LDAPGroupUserRoleId
				if [ $? != 0  ]; then
					echo_fail "BMCSet_LDAPGroupUserRoleId$groupid_serverid'value($BMCSet_LDAPGroupUserRoleId) is wrong";
					echo "BMCSet_LDAPGroupUserRoleId$groupid_serverid'value($BMCSet_LDAPGroupUserRoleId) is wrong">>$SAVEFILE;
					error_quit
				fi		
				
				if [ $BMCSet_LDAPGroupUserRoleId -lt 2 ] || [ $BMCSet_LDAPGroupUserRoleId -gt 8 ]; then
					echo_fail "BMCSet_LDAPGroupUserRoleId$groupid_serverid'value($BMCSet_LDAPGroupUserRoleId) is wrong";
					echo "BMCSet_LDAPGroupUserRoleId$groupid_serverid'value($BMCSet_LDAPGroupUserRoleId) is wrong">>$SAVEFILE;
					error_quit
				fi
			fi
			#普通用户     GroupUserRoleId : 2 = GroupPrivilege : 2
			#操作员       GroupUserRoleId : 3 = GroupPrivilege : 3
			#管理员       GroupUserRoleId : 4 = GroupPrivilege : 4
			#自定义用户1  GroupUserRoleId : 5 = GroupPrivilege : 3
			#自定义用户2  GroupUserRoleId : 6 = GroupPrivilege : 3
			#自定义用户3  GroupUserRoleId : 7 = GroupPrivilege : 3
			#自定义用户4  GroupUserRoleId : 8 = GroupPrivilege : 3
			if [ "$BMCSet_LDAPGroupUserRoleId" -ge 5 ]; then
				LDAPGroupPrivilege=3
			else
				LDAPGroupPrivilege=$BMCSet_LDAPGroupUserRoleId;
			fi
			
			#检验LDAPGroupPrivilege
			check_LDAP_privilege $groupid_serverid $LDAPGroupPrivilege
			if [ $? != 0 ]; then
				echo_fail "Check LDAPGroup$groupid_serverid groupprivilege $LDAPGroupPrivilege failed"
				echo "Check LDAPGroup$groupid_serverid groupprivilege $LDAPGroupPrivilege failed">>$SAVEFILE
				error_quit
			else
				echo_success "Check LDAPGroup$groupid_serverid groupprivilege $LDAPGroupPrivilege success"
				echo "Check LDAPGroup$groupid_serverid groupprivilege $LDAPGroupPrivilege success">>$SAVEFILE
			fi
			
			#检验BMCSet_LDAPGroupUserRoleId
			check_LDAP_user_role_id $groupid_serverid $BMCSet_LDAPGroupUserRoleId
			if [ $? != 0 ]; then
				echo_fail "Check LDAPGroup$groupid_serverid groupuserroleid $BMCSet_LDAPGroupUserRoleId failed"
				echo "Check LDAPGroup$groupid_serverid groupuserroleid $BMCSet_LDAPGroupUserRoleId failed">>$SAVEFILE
				error_quit
			else
				echo_success "Check LDAPGroup$groupid_serverid groupuserroleid $BMCSet_LDAPGroupUserRoleId success"
				echo "Check LDAPGroup$groupid_serverid groupuserroleid $BMCSet_LDAPGroupUserRoleId success">>$SAVEFILE
			fi

	
			#设置LDAP组登陆规则
			getini $CONFIGFILE BMCSet_LDAPGroupPermitRuleIds$groupid_serverid
			LDAPGroupPermitRuleIds=$parameter
			if [ "$LDAPGroupPermitRuleIds" == "" ]; then
				#为空使用默认值 关闭 0
				LDAPGroupPermitRuleIds=0
			fi
			
			check_LDAP_permitruleids $groupid_serverid $LDAPGroupPermitRuleIds
			if [ $? != 0 ]; then
				echo_fail "Check LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds failed"
				echo "Check LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds failed">>$SAVEFILE
				error_quit
			else
				echo_success "Check LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds success"
				echo "Check LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds success">>$SAVEFILE
			fi
			
			#设置LDAP组登录接口
			getini $CONFIGFILE BMCSet_LDAPGroupLoginInterface$groupid_serverid
			LDAPGroupLoginInterface=$parameter
			if [ "$LDAPGroupLoginInterface" == "" ]; then
				#默认全部支持
				LDAPGroupLoginInterface=137
			fi
			check_bmc_ldapgroup_logininterface $groupid_serverid $LDAPGroupLoginInterface
		done
	done
}

main