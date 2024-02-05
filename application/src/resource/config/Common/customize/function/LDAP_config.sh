#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#设置LDAP的使能状态
set_bmc_ldap_enable()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set ldap enable state $1 fail!"
       echo "set ldap enable state $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set ldap enable state $1 success!"
       return 0
    fi
}

#设置LDAP域服务器地址
set_bmc_ldapserver_hostaddr()
{
	if [ "$2" != "" ] ; then
		hostaddr=$2
		strlen=`echo ${#hostaddr}`
		if [ $strlen -gt 255 ] ; then
			echo_fail "LDAP$1 host addr'len($strlen) is too big!"
			echo "LDAP$1 host addr'len($strlen) is too big!" >>$SAVEFILE
			error_quit
		fi	
		hostaddr=$(string2hex "$hostaddr")
	else
		#删除服务器地址
		hostaddr=""
		strlen=0
	fi
		
	fun_id=59
	key_id=$(word2string 0x01)
	key_type=79 #y
	key_len=1
	key_val=`printf "%02x" $1`
	info_id=$(word2string 2)
	info_type=73 #s
	info_len=`printf "%02x" $strlen`
	info_val=$hostaddr
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAP$key_val host addr fail!"
       echo "set LDAP$key_val host addr fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAP$key_val host addr success!"
       return 0
    fi
}

#设置LDAP域服务器域名
set_bmc_ldapserver_userdomain()
{
	if [ "$2" != "" ] ; then
		userdomain=$2
		strlen=`echo ${#userdomain}`
		if [ $strlen -gt 255 ] ; then
			echo_fail "LDAP$1 userdomain'len($strlen) is too big!"
			echo "LDAP$1 userdomain'len($strlen) is too big!" >>$SAVEFILE
			error_quit
		fi	
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAP$key_val userdomain fail!"
       echo "set LDAP$key_val userdomain fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAP$key_val userdomain success!"
       return 0
    fi
}

#设置LDAP域服务器域名
set_bmc_ldapserver_userfolder()
{
	if [ "$2" != "" ] ; then
		userfolder=$2
		strlen=`echo ${#userfolder}`
		if [ $strlen -gt 255 ] ; then
			echo_fail "LDAP$1 userfolder'len($strlen) is too big!"
			echo "LDAP$1 userfolder'len($strlen) is too big!" >>$SAVEFILE
			error_quit
		fi	
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAP$key_val userfolder fail!"
       echo "set LDAP$key_val userfolder fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAP$key_val userfolder success!"
       return 0
    fi
}

#设置LDAP域服务器端口
set_bmc_ldapserver_port()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAP$key_val port fail!"
       echo "set LDAP$key_val port fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAP$key_val port success!"
       return 0
    fi
}

#设置LDAP域证书使能
set_bmc_ldapserver_certstatus()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAP$key_val certstatus fail!"
       echo "set LDAP$key_val certstatus fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAP$key_val certstatus success!"
       return 0
    fi
}

#设置LDAP组组名
set_bmc_ldapgroup_groupname()
{	
	if [ "$2" != "" ] ; then
		groupname=$2
		strlen=`echo ${#groupname}`
		if [ $strlen -gt 32 ] ; then
			echo_fail "LDAPGroup$1 groupname'len($strlen) is too big!"
			echo "set LDAPGroup$1 groupname'len($strlen) is too big" >>$SAVEFILE
			error_quit
		fi	
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAPGroup $1 groupname fail!"
       echo "set LDAPGroup $1 groupname fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAPGroup $1 groupname success!"
       return 0
    fi
}

#设置LDAP组应用文件夹
set_bmc_ldapgroup_groupfolder()
{
	if [ "$2" != "" ] ; then
		groupfolder=$2
		strlen=`echo ${#groupfolder}`
		if [ $strlen -gt 255 ] ; then
			echo_fail "LDAPGroup$1 groupfolder'len($strlen) is too big!"
			echo "LDAPGroup$1 groupfolder'len($strlen) is too big" >>$SAVEFILE
			error_quit
		fi
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set LDAPGroup $1 groupfolder fail!"
		echo "set LDAPGroup $1 groupfolder fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "set LDAPGroup $1 groupfolder success!"
		return 0
	fi	
}

#设置LDAP组登录规则
set_bmc_ldapgroup_permitruleids()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAPGroup $1 permitruleids fail!"
       echo "set LDAPGroup $1 permitruleids fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAPGroup $1 permitruleids success!"
       return 0
    fi
}

#设置LDAP组权限
set_bmc_ldapgroup_privilege()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAPGroup $1 privilege fail!"
       echo "set LDAPGroup $1 privilege fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAPGroup $1 privilege success!"
       return 0
    fi
}

set_bmc_ldapgroup_user_role_id()
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
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAPGroup $1 UserRoleId fail!"
       echo "set LDAPGroup $1 UserRoleId fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAPGroup $1 UserRoleId success!"
       return 0
    fi
}

#设置LDAP组登录接口
set_bmc_ldapgroup_logininterface()
{
	#检查$2
	checkInt $2
	if [ $? != 0  ]; then
		echo_fail "LDAPGroupLoginInterface$1'value($2) is wrong";
		echo "LDAPGroupLoginInterface$1'value($2) is wrong">>$SAVEFILE;
		error_quit
	fi
	
	if [ $2 -lt 0 ] || [ $2 -gt 137 ]; then
		echo_fail "LDAPGroupLoginInterface$1'value($2) is wrong";
		echo "LDAPGroupLoginInterface$1'value($2) is wrong">>$SAVEFILE;
		error_quit
	fi
	

	fun_id=53
	key_id=$(word2string 0x1)
	key_type=79 #y
	key_len="01"
	key_val=`printf "%02x" $1`
	info_id=$(word2string 5)
	info_type=75 #u
	info_len="04"
	info_val=$(dword2string $2)
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"	
	if [ $? != 0 ] ; then
       echo_fail "set LDAPGroup $1 login interface $2 fail!"
       echo "set LDAPGroup $1 login interface $2 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set LDAPGroup $1 login interface $2 success!"
       return 0
    fi
}

main()
{
	#设置LDAP使能开关
	getini $CONFIGFILE BMCSet_LDAPEnable
	LDAPEnable=$parameter
	if [ "$LDAPEnable" == "" ]; then
		#为空使用默认值 关闭 0
		LDAPEnable=0
	fi
	
	#校验LDAP使能入参
	checkInt $LDAPEnable
	if [ $? != 0  ]; then
		echo_fail "LDAPEnable'value($LDAPEnable) is wrong";
		echo "LDAPEnable'value($LDAPEnable) is wrong">>$SAVEFILE;
		error_quit
	fi
	
	if [ $LDAPEnable != 0 ] && [ $LDAPEnable != 1 ]; then
		echo_fail "LDAPEnable'value($LDAPEnable) is wrong"
		echo "LDAPEnable'value($LDAPEnable) is wrong">>$SAVEFILE
		error_quit
	fi
	
	set_bmc_ldap_enable $LDAPEnable
	if [ $? != 0 ]; then
		echo_fail "Set LDAP Enable $LDAPEnable failed"
		echo "Set LDAP Enable $LDAPEnable failed">>$SAVEFILE
		error_quit
	else
		echo_success "Set LDAP Enable $LDAPEnable success"
		echo "Set LDAP Enable $LDAPEnable success">>$SAVEFILE
	fi
	
	local loopi=1
	for((loopi=1;loopi<=6;loopi++));do
		#设置LDAP域服务器地址
		getini $CONFIGFILE BMCSet_LDAPHostAddr$loopi;
		LDAPHostAddr=$parameter;
		if [ "$LDAPHostAddr" == "" ]; then
			#为空使用默认值 关闭 0
			LDAPHostAddr="";
		fi
		
		set_bmc_ldapserver_hostaddr $loopi $LDAPHostAddr;
		if [ $? != 0 ]; then
			echo_fail "Set LDAP$loopi host addr $LDAPHostAddr failed";
			echo "Set LDAP$loopi host addr $LDAPHostAddr failed">>$SAVEFILE;
			error_quit;
		else
			echo_success "Set LDAP$loopi host addr $LDAPHostAddr success";
			echo "Set LDAP$loopi host addr $LDAPHostAddr success">>$SAVEFILE;
		fi
			
		#设置LDAP域名
		getini $CONFIGFILE BMCSet_LDAPUserDomain$loopi;
		LDAPUserDomain=$parameter;
		if [ "$LDAPUserDomain" == "" ]; then
			#为空使用默认值 关闭 0
			LDAPUserDomain="";
		fi
		
		set_bmc_ldapserver_userdomain $loopi $LDAPUserDomain;
		if [ $? != 0 ]; then
			echo_fail "Set LDAP$loopi userdomain $LDAPUserDomain failed";
			echo "Set LDAP$loopi userdomain $LDAPUserDomain failed">>$SAVEFILE;
			error_quit;
		else
			echo_success "Set LDAP$loopi userdomain $LDAPUserDomain success";
			echo "Set LDAP$loopi userdomain $LDAPUserDomain success">>$SAVEFILE;
		fi
			
		#设置LDAP域名
		getini $CONFIGFILE BMCSet_LDAPUserFolder$loopi;
		LDAPUserFolder=$parameter;
		if [ "$LDAPUserFolder" == "" ]; then
			#为空使用默认值 关闭 0
			LDAPUserFolder="";
		fi
		
		set_bmc_ldapserver_userfolder $loopi $LDAPUserFolder;
		if [ $? != 0 ]; then
			echo_fail "Set LDAP$loopi userfolder $LDAPUserFolder failed";
			echo "Set LDAP$loopi userfolder $LDAPUserFolder failed">>$SAVEFILE;
			error_quit;
		else
			echo_success "Set LDAP$loopi userfolder $LDAPUserFolder success";
			echo "Set LDAP$loopi userfolder $LDAPUserFolder success">>$SAVEFILE;
		fi	
			
		#设置LDAP端口号
		getini $CONFIGFILE BMCSet_LDAPPort$loopi;
		LDAPPort=$parameter;
		if [ "$LDAPPort" == "" ]; then
			#为空使用默认值 关闭 636
			LDAPPort=636;
		fi
		
		#校验LDAP端口号
		checkInt $LDAPPort
		if [ $? != 0  ]; then
			echo_fail "LDAPPort$loopi'value($LDAPPort) is wrong";
			echo "LDAPPort$loopi'value($LDAPPort) is wrong">>$SAVEFILE;
			error_quit
		fi
		
		if [ $LDAPPort -lt 1 ] || [ $LDAPPort -gt 65535 ]; then
			echo_fail "LDAPPort$loopi'value($LDAPPort) is wrong";
			echo "LDAPPort$loopi'value($LDAPPort) is wrong">>$SAVEFILE;
			error_quit
		fi
		
		set_bmc_ldapserver_port $loopi $LDAPPort;
		if [ $? != 0 ]; then
			echo_fail "Set LDAP$loopi port $LDAPPort failed";
			echo "Set LDAP$loopi port $LDAPPort failed">>$SAVEFILE;
			error_quit
		else
			echo_success "Set LDAP$loopi port $LDAPPort success";
			echo "Set LDAP$loopi port $LDAPPort success">>$SAVEFILE;
		fi
			
		#设置证书状态
		getini $CONFIGFILE BMCSet_LDAPCertStatus$loopi;
		LDAPCertStatus=$parameter;
		if [ "$LDAPCertStatus" == "" ]; then
			#为空使用默认值 关闭 0
			LDAPCertStatus=0
		fi
		
		#校验证书状态
		checkInt $LDAPCertStatus
		if [ $? != 0  ]; then
			echo_fail "LDAPCertStatus$loopi'value($LDAPCertStatus) is wrong";
			echo "LDAPCertStatus$loopi'value($LDAPCertStatus) is wrong">>$SAVEFILE;
			error_quit
		fi
		
		if [ $LDAPCertStatus -lt 0 ] || [ $LDAPCertStatus -gt 1 ]; then
			echo_fail "LDAPCertStatus$loopi'value($LDAPCertStatus) is wrong";
			echo "LDAPCertStatus$loopi'value($LDAPCertStatus) is wrong">>$SAVEFILE;
			error_quit
		fi
		
		set_bmc_ldapserver_certstatus $loopi $LDAPCertStatus;
		if [ $? != 0 ]; then
			echo_fail "Set LDAP$loopi certstatus $LDAPCertStatus failed";
			echo "Set LDAP$loopi certstatus $LDAPCertStatus failed">>$SAVEFILE;
			error_quit;
		else
			echo_success "Set LDAP$loopi certstatus $LDAPCertStatus success";
			echo "Set LDAP$loopi certstatus $LDAPCertStatus success">>$SAVEFILE;
		fi
	done
	
	for((j=0;j<=4;j++));do
		for((k=1;k<=6;k++));do
			groupid_serverid=0
			let "groupid_serverid=j*16+k";
			
			#设置LDAP组名
			getini $CONFIGFILE BMCSet_LDAPGroupName$groupid_serverid
			LDAPGroupName=$parameter
			if [ "$LDAPGroupName" == "" ]; then
				#为空使用默认值 关闭 0
				BMCSet_LDAPGroupName=""
			fi
			
			set_bmc_ldapgroup_groupname $groupid_serverid "$LDAPGroupName"
			if [ $? != 0 ]; then
				echo_fail "Set LDAPGroup$groupid_serverid groupname $LDAPGroupName failed"
				echo "Set LDAPGroup$groupid_serverid groupname $LDAPGroupName failed">>$SAVEFILE
				error_quit
			else
				echo_success "Set LDAPGroup$groupid_serverid groupname $LDAPGroupName success"
				echo "Set LDAPGroup$groupid_serverid groupname $LDAPGroupName success">>$SAVEFILE
			fi
			
			#设置LDAP组应用文件夹
			getini $CONFIGFILE BMCSet_LDAPGroupFolder$groupid_serverid
			LDAPGroupFolder=$parameter
			if [ "$LDAPGroupFolder" == "" ]; then
				#为空使用默认值 关闭 0
				BMCSet_LDAPGroupFolder=""
			fi
			
			set_bmc_ldapgroup_groupfolder $groupid_serverid "$LDAPGroupFolder"
			if [ $? != 0 ]; then
				echo_fial "Set LDAPGroup$groupid_serverid groupfolder $LDAPGroupFolder failed"
				echo "Set LDAPGroup$groupid_serverid groupfolder $LDAPGroupFolder failed">>$SAVEFILE
				error_quit
			else
				echo_success "Set LDAPGroup$goupid_serverid groupfolder $LDAPGroupFolder success"
				echo "Set LDAPGroup$goupid_serverid groupfolder $LDAPGroupFolder success">>$SAVEFILE
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
			#设置LDAPGroupPrivilege
			set_bmc_ldapgroup_privilege $groupid_serverid $LDAPGroupPrivilege
			if [ $? != 0 ]; then
				echo_fail "Set LDAPGroup$groupid_serverid grouprivilege $LDAPGroupPrivilege failed"
				echo "Set LDAPGroup$groupid_serverid grouprivilege $LDAPGroupPrivilege failed">>$SAVEFILE
				error_quit
			else
				echo_success "Set LDAPGroup$groupid_serverid grouprivilege $LDAPGroupPrivilege success"
				echo "Set LDAPGroup$groupid_serverid grouprivilege $LDAPGroupPrivilege success">>$SAVEFILE
			fi
			#BMCSet_LDAPGroupUserRoleId
			set_bmc_ldapgroup_user_role_id $groupid_serverid $BMCSet_LDAPGroupUserRoleId
			if [ $? != 0 ]; then
				echo_fail "Set LDAPGroup$groupid_serverid groupuserroleid $BMCSet_LDAPGroupUserRoleId failed"
				echo "Set LDAPGroup$groupid_serverid groupuserroleid $BMCSet_LDAPGroupUserRoleId failed">>$SAVEFILE
				error_quit
			else
				echo_success "Set LDAPGroup$groupid_serverid groupname $BMCSet_LDAPGroupUserRoleId success"
				echo "Set LDAPGroup$groupid_serverid groupname $BMCSet_LDAPGroupUserRoleId success">>$SAVEFILE
			fi			
			
			#设置LDAP组登陆规则
			getini $CONFIGFILE BMCSet_LDAPGroupPermitRuleIds$groupid_serverid
			LDAPGroupPermitRuleIds=$parameter
			if [ "$LDAPGroupPermitRuleIds" == "" ]; then
				#为空使用默认值 关闭 1
				LDAPGroupPermitRuleIds=0
			fi
			
			
			
			#校验LDAP组权限
			checkInt $LDAPGroupPermitRuleIds
			if [ $? != 0  ]; then
				echo_fail "LDAPGroupPermitRuleIds$groupid_serverid'value($LDAPGroupPermitRuleIds) is wrong";
				echo "LDAPGroupPermitRuleIds$groupid_serverid'value($LDAPGroupPermitRuleIds) is wrong">>$SAVEFILE;
				error_quit
			fi
			
			if [ $LDAPGroupPermitRuleIds -lt 0 ] || [ $LDAPGroupPermitRuleIds -gt 7 ]; then
				echo_fail "LDAPGroupPermitRuleIds$groupid_serverid'value($LDAPGroupPermitRuleIds) is wrong";
				echo "LDAPGroupPermitRuleIds$groupid_serverid'value($LDAPGroupPermitRuleIds) is wrong">>$SAVEFILE;
				error_quit
			fi
			
			set_bmc_ldapgroup_permitruleids $groupid_serverid $LDAPGroupPermitRuleIds
			if [ $? != 0 ]; then
				echo_fail "Set LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds failed"
				echo "Set LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds failed">>$SAVEFILE
				error_quit
			else
				echo_success "Set LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds success"
				echo "Set LDAPGroup$groupid_serverid groupname $LDAPGroupPermitRuleIds success">>$SAVEFILE;
			fi
			
			#设置LDAP组登录接口
			getini $CONFIGFILE BMCSet_LDAPGroupLoginInterface$groupid_serverid
			LDAPGroupLoginInterface=$parameter
			if [ "$LDAPGroupLoginInterface" == "" ]; then
				#默认全部支持
				LDAPGroupLoginInterface=137
			fi
			set_bmc_ldapgroup_logininterface $groupid_serverid $LDAPGroupLoginInterface
		done
	done
}

main