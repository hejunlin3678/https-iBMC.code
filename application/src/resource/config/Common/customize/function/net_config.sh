#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#设置主机名称
set_host_name()
{
	string2substring "$HOSTNAME" 64	
	strlenlen=`echo ${#HOSTNAME}`
	local offset=0
	local send_data_len=0
	local remain_data_len=0
	local end_flag=0
	local PART_STRING
	local str_begin_offset=0
	local str_end_offset=0
	while true
	do
		if [ $strlenlen -gt $offset ] ; then
			remain_data_len=`expr $strlenlen - $offset `
			if [ $remain_data_len -gt $MAX_STRING_NUM ] ;then
				end_flag=1
				send_data_len=$MAX_STRING_NUM
			else
				end_flag=0
				send_data_len=$remain_data_len
			fi
					
			#该段字符串起始发送位置
			str_begin_offset=`expr $offset \* 3 + 1`
			
			#该段字符串结束发送位置
			str_end_offset=`expr $send_data_len \* 3 + $str_begin_offset - 1`
			#截取要发送的字符串
			PART_STRING=`echo $TMP_STRING | cut -b $str_begin_offset-$str_end_offset`

			run_cmd "f 0 30 93 db 07 00 05 01 $end_flag `printf %2x $offset` `printf %2x $send_data_len` $PART_STRING" "0f 31 00 93 00 db 07 00" 
			if [ $? != 0 ] ; then
				echo_fail "Set Hostname:$HOSTNAME fail!1"
				echo "Set Hostname:$HOSTNAME fail!1" >>$SAVEFILE
				error_quit 
			fi
			
			#下次发送的起始位置
			offset=`expr $offset + $send_data_len`
		else
		    break
		fi
	done
	
	echo_success "Set Hostname:$HOSTNAME success!"
	echo "Set Hostname:$HOSTNAME success!" >>$SAVEFILE	
}

#设ETH2_DHCP模式
set_eth2_dhcp_mode()
{
    run_cmd "f 0 0c 01 1 04 02" "0f 0d 00 01 00" 	
	if [ $? != 0 ] ; then
        echo_fail "Set Eth2 DHCP fail!"
        echo "Set Eth2 DHCP fail!" >>$SAVEFILE
        error_quit
   else
        echo_success "Set Eth2 DHCP success!"
        echo "Set Eth2 DHCP success!" >>$SAVEFILE
        return 0
   fi
}

#设置ip addr 参数：网络端口 ip地址例:192.168.100.1
set_ipaddr()
{
    echo "$2" > temp.txt
    dos2unix temp.txt > /dev/null 2&>1
    ipaddr=`cat temp.txt | grep "." |   awk -F "/" '{print $1}' | awk -F "." '{print $1,$2,$3,$4}'`
    if [ "$ipaddr" = "" ] ; then
	    echo_fail "Get IP:$2 fail"
	    echo "Get IP:$2 fail" >>$SAVEFILE
	    error_quit
    else

	    ip1=`echo $ipaddr | awk -F " " '{print $1}'`
	    ip2=`echo $ipaddr | awk -F " " '{print $2}'`
	    ip3=`echo $ipaddr | awk -F " " '{print $3}'`
	    ip4=`echo $ipaddr | awk -F " " '{print $4}'`

	    ipset1=`printf %02x $ip1`
	    ipset2=`printf %02x $ip2`
	    ipset3=`printf %02x $ip3`
	    ipset4=`printf %02x $ip4`

	    loadip="$ipset1 $ipset2 $ipset3 $ipset4"

	    run_cmd "f 0 c 1 $1 3 $loadip" "0f 0d 00 01 00"
    fi
}

#设置ip mask 参数：网络端口 ip掩码例:16
set_ipmask()
{
    ipmask=0
    case $2 in
	8)
	    ipmask="ff 00 00 00" 
	;;
	
	9)
	    ipmask="ff 80 00 00"  
	;;
	
	10)
	    ipmask="ff c0 00 00"  
	;;
	
	11)
	    ipmask="ff e0 00 00"  
	;;
	
	12)
	    ipmask="ff f0 00 00"  
	;;
	
	13)
	    ipmask="ff f8 00 00"  
	;;
	
	14)
	    ipmask="ff fc 00 00"  
	;;
	
	15)
	    ipmask="ff fe 00 00"  
	;;
	
	16)
	    ipmask="ff ff 00 00"  
	;;
	
	17)
	    ipmask="ff ff 80 00"  
	;;
	
	18)
	    ipmask="ff ff c0 00"  
	;;
	
	19)
	    ipmask="ff ff e0 00"  
	;;
	
	20)
	    ipmask="ff ff f0 00"  
	;;
	
	21)
	    ipmask="ff ff f8 00"  
	;;
	
	22)
	    ipmask="ff ff fc 00"  
	;;
	
	23)
	    ipmask="ff ff fe 00"  
	;;
	
	24)
	    ipmask="ff ff ff 00"  
	;;
	
	25)
	    ipmask="ff ff ff 80"  
	;;
	
	26)
	    ipmask="ff ff ff c0"  
	;;
	
	27)
	    ipmask="ff ff ff e0"  
	;;
	
	28)
	    ipmask="ff ff ff f0"  
	;;
	
	29)
	    ipmask="ff ff ff f8"  
	;;
	
	30)
	    ipmask="ff ff ff fc"  
	;;
	
	31)
	    ipmask="ff ff ff fe"  
	;;
	
	*)
	    echo_fail "Set eth$1 netmask $2 fail"
		echo "Set eth$1 netmask $2 fail" >>$SAVEFILE
		error_quit 
	;;
    esac
	
	run_cmd "f 0 c 1 $1 6 $ipmask" "0f 0d 00 01 00"
} 

# 设置网关
set_gateway()
{
	echo "$2" >temp.txt
	dos2unix temp.txt > /dev/null 2&>1
	gateway=`cat temp.txt | grep "." | awk -F "." '{print $1,$2,$3,$4}'`
	if [ "$gateway" = "" ] ; then
		echo_fail "GET gateway:$2 fail"
		echo "GET gateway:$2 fail" >>$SAVEFILE
		error_quit
	else
		ip1=`echo $gateway | awk -F " " '{print $1}'`
		ip2=`echo $gateway | awk -F " " '{print $2}'`
		ip3=`echo $gateway | awk -F " " '{print $3}'`
		ip4=`echo $gateway | awk -F " " '{print $4}'`
		ipset1=`printf %02x $ip1`
		ipset2=`printf %02x $ip2`
		ipset3=`printf %02x $ip3`
		ipset4=`printf %02x $ip4`

		loadgateway="$ipset1 $ipset2 $ipset3 $ipset4"

		run_cmd "f 0 c 1 $1 c $loadgateway" "0f 0d 00 01 00"
    fi
}


#配置管理网口手动IP地址
set_eth2_ipaddr()
{
	set_ipaddr 01 $Eth2_IPADDR
	if [ $? != 0 ] ; then
		echo_fail "Set Eth2 IP:$Eth2_IPADDR Fail"
		echo "Set Eth2 IP:$Eth2_IPADDR Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Set Eth2 IP:$Eth2_IPADDR success!"
		echo "Set Eth2 IP:$Eth2_IPADDR success!" >>$SAVEFILE
		return 0
	fi
}

#设置ip mask
set_eth2_ipmask()
{
	set_ipmask  01 $Eth2_IPMASK
	if [ $? != 0 ] ; then
	    echo_fail "Set Eth2 Netmask:$Eth2_IPMASK fail"
		echo "Set Eth2 Netmask:$Eth2_IPMASK fail" >>$SAVEFILE
		error_quit
	else
	    echo_success "Set Eth2 Netmask:$Eth2_IPMASK success"
        echo "Set Eth2 Netmask:$Eth2_IPMASK success" >>$SAVEFILE
	fi
}

# 设置 gateway
set_ipv4_gateway()
{
	getini $CONFIGFILE  Custom_ipv4_gateway
	ipv4_gateway=$parameter

	if [ "$ipv4_gateway" != "" ] ; then	    
		set_gateway 01 $ipv4_gateway
		if [ $? != 0 ] ; then
			echo_fail "set ipv4 gateway $ipv4_gateway fail"
			echo "set ipv4 gateway $ipv4_gateway fail" >>$SAVEFILE
			error_quit
		else
			echo_success "set ipv4 gateway $ipv4_gateway success"
			echo "set ipv4 gateway $ipv4_gateway success" >>$SAVEFILE
		fi
	else
		set_gateway 01 $Eth2_IPADDR
		if [ $? != 0 ] ; then
			echo_fail "set ipv4 gateway $Eth2_IPADDR fail"
			echo "set ipv4 gateway $Eth2_IPADDR fail" >>$SAVEFILE
			error_quit
		else
			echo_success "set ipv4 gateway $Eth2_IPADDR success"
			echo "set ipv4 gateway $Eth2_IPADDR success" >>$SAVEFILE
		fi
	fi
}

restore_eth2_ipaddr()
{
	Eth2_IPADDR=$(get_default_ip_by_product)
	
	set_eth2_ipaddr
}

#还原管理网口默认IP掩码
restore_eth2_ipmask()
{
	#如果是OSCA产品，ip恢复为空，不需要也无法设置掩码
	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
		return 0
    fi
    #设置掩码 软件有bug 可能失败 所以需延长到12秒
    #sleep 12
	ipmask="ff ff ff 00"
		
    run_cmd  "f 0 c 1 1 6 $ipmask"  "0f 0d 00 01 00"
    if [ $? != 0 ] ; then
		echo_fail "Restore Eth2 Netmask $ipmask fail"
		echo "Restore Eth2 netmask $ipmask fail" >>$SAVEFILE
		error_quit 
    else
		echo_success "Restore Eth2 Netmask $ipmask success"
		echo "Restore Eth2 Netmask $ipmask success" >>$SAVEFILE
		return 0
    fi
}

restore_gateway()
{
	#如果是OSCA产品，ip恢复为空，不需要也无法设置网关
	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
		return 0
	fi

	ipv4_gateway=$(get_default_ip_by_product)

	set_gateway 01 $ipv4_gateway
	if [ $? != 0 ] ; then
		echo_fail "Restore ipv4 gateway $ipv4_gateway fail"
		echo "Restore ipv4 gateway $ipv4_gateway fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Restore ipv4 gateway $ipv4_gateway success"
		echo "Restore ipv4 gateway $ipv4_gateway success" >>$SAVEFILE
	fi
}


#恢复backup ip为默认
restore_backup_ip_and_mask()
{
	#OSCA、TCE产品不支持backup ip
	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
		return 0
	fi
	
	IPADDR=$(get_default_ip_by_product)
	
	ipaddr=`echo $IPADDR | grep "." |   awk -F "/" '{print $1}' | awk -F "." '{print $1,$2,$3,$4}'`
	if [ "$ipaddr" = "" ] ; then
	    echo_fail "GET IP:$IPADDR error"
	    echo "GET IP:$IPADDR error" >>$SAVEFILE
	    error_quit 
	else
		ip1=`echo $ipaddr | awk -F " " '{print $1}'`
		ip2=`echo $ipaddr | awk -F " " '{print $2}'`
		ip3=`echo $ipaddr | awk -F " " '{print $3}'`
		ip4=`echo $ipaddr | awk -F " " '{print $4}'`
	
		ipset1=`printf %02x $ip1`
		ipset2=`printf %02x $ip2`
		ipset3=`printf %02x $ip3`
		ipset4=`printf %02x $ip4`
	
		loadip="$ipset1 $ipset2 $ipset3 $ipset4"
		ipmask="ff ff ff 00"
	
		#设置backup ip
		run_cmd  "f 0 30 93 db 07 00 05 1A 01 00 08 $loadip $ipmask"  "0f 31 00 93 00 db 07 00"	
			
		if [ $? != 0 ] ; then
			echo_fail "Restore back up ip $IPADDR fail"
			echo "Restore back up ip $IPADDR fail" >>$SAVEFILE
			error_quit
		else
			echo_success "Restore back up ip $IPADDR success"
			echo "Restore back up ip $IPADDR success" >>$SAVEFILE
		fi
	fi
}

#获取厂商id
get_custom_manu_id()
{
	custom_manu_id="db 07 00"
	cmd_return=$(ipmicmd -k "f 0 30 90 22 08" smi 0)
	custom_manu_id=${cmd_return:18}
	echo $custom_manu_id
}

#设置网口模式 
Set_NET_Mode()
{
	getini  $CONFIGFILE  BMCSet_NET_Mode
	NET_Mode=$parameter
	
	if [ "$NET_Mode" = "mgnt" ] ; then
		value=01
	elif [ "$NET_Mode" = "ncsi" ] ; then
		value=02
	elif [ "$NET_Mode" = "auto" ] ; then
		value=03
	elif [ "$NET_Mode" = "pcie" ] ; then
		value=04	
	elif [ "$NET_Mode" = "aggr" ] ; then
		value=05
	elif [ "$NET_Mode" = "lom2" ] ; then 
		value=06	
	else
		#默认配置强制管理网口 保持和以前发货的值一致 跑空定制化，x6000v6因为有汇聚网口没有管理网口所以默认配置汇聚网口
		if [ $PRODUCT_NAME_ID == $PRODUCT_XH321V6_ID ] || [ $PRODUCT_NAME_ID == $PRODUCT_XH321CV6_ID ] || [ $PRODUCT_NAME_ID == $PRODUCT_D320V2_ID ] || [ $PRODUCT_NAME_ID == $PRODUCT_D320CV2_ID ] ; then
			value=05
			NET_Mode="aggr"
		else
			value=01
			NET_Mode="mgnt"
		fi
	fi
	
	getini  $CONFIGFILE  BMCSet_NCSI_Enable
	if [ "$parameter" = "on" ] ; then
		NET_Mode="auto"
		value=3
	fi
	
	# 校验NetMode是否为欲设置的值
	run_cmd "f 0 0c 02 01 d7 0 0"  "0f 0d 00 02 00 11 $(get_custom_manu_id) $value"
	if [ $? != 0 ] ; then
		# 设置NetMode
		echo "Setting NET Mode: $NET_Mode">>$SAVEFILE
		run_cmd "f 0 0c 01 01 d7 db 07 00 $value"  "0f 0d 00 01 00"
		if [ $? != 0 ] ; then
			echo_fail "Set NET Mode: $NET_Mode fail"
			echo "Set NET Mode: $NET_Mode fail" >>$SAVEFILE
			error_quit
		else
			#设置NET_Mode为pcie时会取持久化的Eth1.NCSIPort数据下发，可能与欲定制的NCSI口冲突
			sleep 30
		fi
	fi
	echo_success "Set NET Mode: $NET_Mode success"
	echo "Set NET Mode: $NET_Mode success">>$SAVEFILE
	
	Set_NCSI_port $value
	
}

#设NCSI Port Num
Set_NCSI_port()
{
	if [ "$1" == "04" ] ; then
		net_mode=01
	elif [ "$1" == "06" ] ; then
		net_mode=03
    elif [ "$1" == "01" ] ; then
		net_mode=02
	else
		net_mode=00
	fi
	
	getini  $CONFIGFILE  Custom_NCSI_Port_Num
	NCSI_Port_Num=$parameter
	if [ "$NCSI_Port_Num" != "" ] ; then
		NCSIPortNum=`printf %02x $NCSI_Port_Num`
		echo "NCSIPortNum=$NCSI_Port_Num">>$SAVEFILE
		#NCSIPortNum <00/01/02/03>
		run_cmd "f 0 30 93 db 07 00 05 0D 00 00 02 $NCSIPortNum $net_mode"  "0f 31 00 93 00 db 07 00"
		if [ $? != 0 ] ; then
			echo_fail "Set NCSI Port Num fail"
			echo "Set NCSI Port Num fail">>$SAVEFILE
			error_quit			
		else
			echo_success "Set NCSI Port Num $NCSIPortNum success"
			echo "Set NCSI Port Num $NCSIPortNum success">>$SAVEFILE
			sleep 1
		fi
	fi
}

#配置NCSI VLAN ID
set_vlan_id()
{
	channel_id=1
	vlan_name="NCSI"

	if [ $# -gt 1 ]; then
		channel_id=${2}
		vlan_name="Rack Inner"
	fi

	if [ "$1" -gt "4094" -o "$1" -lt "1" ]  ; then
		echo_and_save_fail "set $vlan_name VLAN ID $1 invalid"
		error_quit
	else
		vlanid=`printf "%04x" $1`
		vlanidlow=${vlanid:2}
		vlanidhigh=${vlanid:0:2}
		vlanidhigh=`printf "%d" 0x$vlanidhigh`
		vlanidhigh=`expr $vlanidhigh + 128`
		vlanidhigh=`printf "%02x" $vlanidhigh`		
		run_cmd "f 0 0c 01 ${channel_id} 14 $vlanidlow $vlanidhigh"  "0f 0d 00 01 00"
		
		if [ $? != 0 ] ; then
			echo_fail "set $vlan_name VLAN ID $1 fail"
			echo "set $vlan_name VLAN ID $1 fail" >>$SAVEFILE
			error_quit
		else
			echo_success "set $vlan_name VLAN ID $1 success"
			echo "set $vlan_name VLAN ID $1 success">>$SAVEFILE
			sleep 1
		fi 
	fi
}

restore_NCSI_vlan_id()
{	
	run_cmd "f 0 0c 01 01 14 00 00"  "0f 0d 00 01 00"
	if [ $? != 0 ] ; then
		echo_fail "restore NCSI VLAN ID fail"
		echo "restore NCSI VLAN ID fail" >>$SAVEFILE
		error_quit
	else
		echo_success "restore NCSI VLAN ID success"
		echo "restore NCSI VLAN ID success">>$SAVEFILE
		sleep 1
	fi 
}

#配置NCSI
config_NCSI()
{
	#设置NCSI VLAN ID
	getini $CONFIGFILE  BMCSet_NCSI_VLAN_ID
	VLAN_ID=$parameter
	if [ "$VLAN_ID" != "" ] ; then
		set_vlan_id $VLAN_ID
	else
		restore_NCSI_vlan_id
	fi

    getini  $CONFIGFILE  BMCSet_NCSI_Enable
    if [ "$parameter" = "on" ] ; then	
	    #设置 NCSI IPV4或者设置DHCP模式
	    getini  $CONFIGFILE  BMCSet_NCSI_DHCPMode
	    DHCP=$parameter
		#是否开启DHCP
	    if [ "$DHCP" = "on" ] ; then    	
	    	set_eth2_dhcp_mode     	
	    	sleep 1	
	    else
			#是否手动配置IP
	        getini  $CONFIGFILE  BMCSet_NCSI_Ipv4
	        IPADDR=$parameter
	        if [ "$IPADDR" = "on" ] ; then
			    #无配置参数直接退出
				grep "Custom_NCSI_Ip_Addr" $CONFIGFILE | grep "." >ip.txt
				if [ $? != 0 ] ; then
					echo_fail "NCSI_IPADDR=NULL set ipaddr fail" 
					echo "NCSI_IPADDR=NULL set ipaddr fail" >>$SAVEFILE
					error_quit
				else
					dos2unix ip.txt > /dev/null 2&>1
					ipaddr=`cat ip.txt | grep "." |  awk -F "/" '{print $1}'`
					netmask=`cat ip.txt | grep "." |   awk -F "/" '{print $2}'`
					IPADDR=$ipaddr
					IPMASK=$netmask
					IPADDR=`echo $IPADDR  | awk -F "=" '{print $2}'`
					Eth2_IPADDR=$IPADDR
					Eth2_IPMASK=$IPMASK			    
					set_eth2_ipaddr
					sleep 5
					set_eth2_ipmask
					sleep 5
					set_ipv4_gateway
					sleep 5
				fi
	    	else
				sleep 1
	    	fi
	    fi
    fi
}

#配置DDNS为自动模式(使用新的ipmi进行替换)
set_ddns_on()
{
	set_prop_cmd 3 1 "09 00 75 04 05 00 00 00 06 00 79 01 01"
    if [ $? != 0 ] ; then
        echo_fail "Set ddns on fail!"
        echo "Set ddns on fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "set ddns on success!"
        echo "set ddns on success!" >>$SAVEFILE
        return 0
    fi
	
}

#设置DDNS为手动模式
set_ddns_off()
{
    set_prop_cmd 3 1 "09 00 75 04 05 00 00 00 06 00 79 01 00"
    if [ $? != 0 ] ; then
        echo_fail "Set ddns off fail!"
        echo "Set ddns off fail!" >>$SAVEFILE      
	error_quit		
    else 
        echo_success "set ddns off success!"
        echo "Set ddns off success!" >>$SAVEFILE
        return 0
    fi
}

#设置DDNS ip模式：0：IPV4；1：IPV6; 
set_ddns_ipmode()
{
	fun_id=3
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 5)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
			echo_fail "set DDSN_AutoIpMode $1 fail!"
			echo "set DDSN_AutoIpMode $1 fail!" >>$SAVEFILE
			error_quit
	else
			echo_success "set DDSN_AutoIpMode $1 success!"
			echo "set DDSN_AutoIpMode $1 success!" >>$SAVEFILE
			return 0
	fi
}

#设置DDNS域名
set_ddns_domain_name()
{
	getini $CONFIGFILE BMCSet_DDSN_DomainName
	strval=$parameter
	strlen=0
	
	if [ "$strval" == "" ] ; then
		strlen=0
	else
		strlen=`echo ${#strval}`
		if [ $strlen -gt 128 ] ; then
			echo_fail "BMCSet_DDSN_DomainName addr'len($strlen) is too big!"
			echo "BMCSet_DDSN_DomainName addr'len($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		strval=$(string2hex "$strval")
	fi
	
	fun_id=3
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1)
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$strval
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set DDSN_DomainName $strval fail!"
		echo "set DDSN_DomainName $strval fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set DDSN_DomainName $strval success!"
		return 0
	fi
}

#设置DDNS首选服务器
set_ddns_primary_domain()
{
	getini $CONFIGFILE BMCSet_DDSN_PrimaryDomain
	strval=$parameter
	strlen=0
	
	if [ "$strval" == "" ] ; then
		strlen=0
	else
		strlen=`echo ${#strval}`
		if [ $strlen -gt 128 ] ; then
			echo_fail "BMCSet_DDSN_PrimaryDomain addr'len($strlen) is too big!"
			echo "BMCSet_DDSN_PrimaryDomain addr'len($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		strval=$(string2hex "$strval")
	fi
	
	fun_id=3
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2)
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$strval
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set DDSN_PrimaryDomain $strval fail!"
		echo "set DDSN_PrimaryDomain $strval fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set DDSN_PrimaryDomain $strval success!"
		return 0
	fi
}

#设置DDNS备用服务器
set_ddns_backup_domain()
{
	getini $CONFIGFILE BMCSet_DDSN_BackupDomain
	strval=$parameter
	strlen=0
	
	if [ "$strval" == "" ] ; then
		strlen=0
	else
		strlen=`echo ${#strval}`
		if [ $strlen -gt 128 ] ; then
			echo_fail "BMCSet_DDSN_BackupDomain addr'len($strlen) is too big!"
			echo "BMCSet_DDSN_BackupDomain addr'len($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		strval=$(string2hex "$strval")
	fi
	
	fun_id=3
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 3)
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$strval
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set DDSN_BackupDomain $strval fail!"
		echo "set DDSN_BackupDomain $strval fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set DDSN_BackupDomain $strval success!"
		return 0
	fi
}

#设置DDNS第二备用服务器
set_ddns_tertiary_domain()
{
	getini $CONFIGFILE BMCSet_DDSN_TertiaryDomain
	strval=$parameter
	strlen=0
	
	if [ "$strval" == "" ] ; then
		strlen=0
	else
		strlen=`echo ${#strval}`
		if [ $strlen -gt 128 ] ; then
			echo_fail "DDSN_TertiaryDomain addr'len($strlen) is too big!"
			echo "DDSN_TertiaryDomain addr'len($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		strval=$(string2hex "$strval")
	fi
	
	fun_id=3
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 10)
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$strval
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
		echo_fail "set DDSN_TertiaryDomain $strval fail!"
		echo "set DDSN_TertiaryDomain $strval fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "set DDSN_TertiaryDomain $strval success!"
		return 0
	fi
}

#ssh/http/https等支持定制化使能
common_service_enable_config()
{
	OLD_IFS="$IFS"
	IFS=","
	array=($1)
	IFS="$OLD_IFS"
	SERVERICE_TYPE=${array[0]}
	INDEX_ID=${array[1]}
	FUNCTION_ID=${array[2]}
	KEY_ID=${array[3]}
	KEY_VALUE=${array[4]}
	INFO_ID=1
	
	get_enable_info_id $SERVERICE_TYPE $INDEX_ID

	getini $CONFIGFILE Custom_${SERVERICE_TYPE}_ServiceControl
	ServiceControl=$parameter
	
	if [ "$ServiceControl" != "off" ]  ; then
		enable_state=1
	else
		enable_state=0
	fi
		
	
	#http有特殊的选项进行深度判断是否打开
	if [ "$SERVERICE_TYPE" == "WEBServer" ] && [ "$INDEX_ID" == "1" ]  && [ "$enable_state" == "1" ] ; then
		get_webserver_http_state
		enable_state=$?
	fi
	
	#ipmilan有特殊的选项进行深度判断是否打开
	if [ "$SERVERICE_TYPE" == "IPMILAN" ] && [ "$INDEX_ID" == "1" ]  && [ "$enable_state" == "1" ] ; 
	then
		get_ipmilan_rmcp_state
		enable_state=$?
	fi	

	if [ "$enable_state" = "0" ] ; then
		ServiceControl=off
	else
		ServiceControl=on
	fi
	
	key_id=$(word2string $KEY_ID) 
	key_type=75   #'u'
	key_len=4
	key_val=$(dword2string $KEY_VALUE)
	info_id=$(word2string $INFO_ID) 
	info_type=79   #'y'
	info_len=1
	info_val=$enable_state

	service_name=$(get_net_service_name $SERVERICE_TYPE $INDEX_ID)

	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $FUNCTION_ID 1 "$set_data"
        if [ $? != 0 ] ; then
       	echo_fail "Set ${service_name}_ServiceControl:$ServiceControl fail"
		echo "Set ${service_name}_ServiceControl:$ServiceControl fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Set ${service_name}_ServiceControl:$ServiceControl success"
		echo "Set ${service_name}_ServiceControl:$ServiceControl success" >>$SAVEFILE
   	fi	
}

#ssh/http/https等支持定制化端口设置
common_service_port_config()
{
	OLD_IFS="$IFS"
	IFS=","
	array=($1)
	IFS="$OLD_IFS"
	SERVERICE_TYPE=${array[0]}
	INDEX_ID=${array[1]}
	FUNCTION_ID=${array[2]}
	KEY_ID=${array[3]}
	KEY_VALUE=${array[4]}
	INFO_ID=2

	service_name=$(get_net_service_name $SERVERICE_TYPE $INDEX_ID)
	
	#不同的对象采用不同的info id
	get_port_info_id $SERVERICE_TYPE $INDEX_ID

	#判断服务端口是否定制化，非定制还原默认端口号
	getini $CONFIGFILE BMCSet_${SERVERICE_TYPE}_Service_PortId${INDEX_ID}
	ServicePortId=$parameter
	if [ "$ServicePortId" = "on" ]  ; then			
		#获取配置服务端口的网络端口号
		getini $CONFIGFILE Custom_${SERVERICE_TYPE}_ServicePort${INDEX_ID}
		ServicePort=$parameter
		is_digit $ServicePort
		if [ $? != 0 ]; then
			echo_fail "Set $service_name ServicePort:$ServicePort failed! port value(1~65535)"
			error_quit
		fi
		#对于port的有效判断
		if [ "$ServicePort" -gt "65535" ] || [ "$ServicePort" -lt "1" ] ; then
			echo_fail "Set $service_name ServicePort:$ServicePort failed! port value(1~65535)"
			error_quit
		fi			
	else
		getini defaultserverport.ini ${SERVERICE_TYPE}_${INDEX_ID}_PORT
		ServicePort=$parameter
	fi
	
	# 设置RMCP和RMCP+的端口使用标准的方法
	if [ "$SERVERICE_TYPE" == "IPMILAN" ]; then
		info_val=$(word2string $ServicePort)
		subcmd="08"
		if [ "$INDEX_ID" == "2" ]; then
			subcmd="09"
		fi		
		
		run_cmd "f 0 0C 01 00 $subcmd $info_val" "0f 0d 00 01 00"
		if [ $? != 0 ] ; then
			echo_fail "Set ${service_name}_ServicePort to $ServicePort fail"
			echo "Set ${service_name}_ServicePort to $ServicePort fail" >>$SAVEFILE
			error_quit
		else
			echo_success "Set ${service_name}_ServicePort to $ServicePort success"
			echo "Set ${service_name}_ServicePort to $ServicePort success" >>$SAVEFILE
			return 0
		fi
	fi
	  
	key_id=$(word2string $KEY_ID) 
	key_type=75   #'u'
	key_len=4
	key_val=$(dword2string $KEY_VALUE)
	info_id=$(word2string $INFO_ID) 
	
	#kvm,vmm的端口数据类型为4个字节
	if [ "$SERVERICE_TYPE" == "RemoteControl" ] || [ "$SERVERICE_TYPE" == "Video" ] || [ "$SERVERICE_TYPE" == "SNMPAgent" ]; then
		info_type=69   #'i'
		info_len="04"
		info_val=$(dword2string $ServicePort)
	else
		info_type=71   #'q'
		info_len="02"
		info_val=$(word2string $ServicePort)
	fi
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	#设置SSH端口号使用标准IPMI命令
	if [ "$SERVERICE_TYPE" == "SSH" ] ; then
		run_cmd "0f 00 30 93 DB 07 00 0F 02 02 00 00 00 01 $info_val" "0f 31 00 93 00 db 07 00"
	else
		set_prop_cmd $FUNCTION_ID 1 "$set_data"
	fi
	if [ $? != 0 ] ; then
		echo_fail "Set ${service_name}_ServicePort to $ServicePort fail"
		echo "Set ${service_name}_ServicePort to $ServicePort fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Set ${service_name}_ServicePort to $ServicePort success"
		echo "Set ${service_name}_ServicePort to $ServicePort success" >>$SAVEFILE
	fi
}


#服务配置
set_service_configuration()
{
	#服务名，索引号---用来索引配置项
	#functionid，keyid，key_value
	SSH_CONFIG="SSH,1,6,3,2"
	HTTP_CONFIG="WEBServer,1,6,3,9"
	HTTPS_CONFIG="WEBServer,2,6,3,11"
	SNMP_CONFIG="SNMPAgent,1,1,10,33"
	RMCP_CONFIG="IPMILAN,1,5,6,32"
	RMCP_PLUS_CONFIG="IPMILAN,2,5,6,32"
	KVM_CONFIG="RemoteControl,1,10,65535,0"  #kvm
	VMM_CONFIG="RemoteControl,2,11,65535,0"  #vmm
	VIDEO_CONFIG="Video,1,12,65535,0"  		 #video
	#根据配置文件配置ftp
	for CONFIG_INFO in $SSH_CONFIG $HTTP_CONFIG $HTTPS_CONFIG $RMCP_CONFIG $SNMP_CONFIG $RMCP_PLUS_CONFIG $KVM_CONFIG $VMM_CONFIG $VIDEO_CONFIG;
	do		
		#服务启用控制
		common_service_enable_config $CONFIG_INFO
		#服务启用控制
		common_service_port_config $CONFIG_INFO	
	done

}

#设置MIB版本
config_snmp_mib_version()
{
	if [ "$1" == "4" ] ;then
		VersionType="V1"
	elif [ "$1" == "5" ] ;then
		VersionType="V2"
	else
		echo_fail "MIBVersion parameter fail"
		echo "MIBVersion parameterfail">>$SAVEFILE
		error_quit
	fi
	
	if [ "$2" == "0" ] ;then
		VersionState="off"
	elif [ "$2" == "1" ] ;then
		VersionState="on"
	else
		echo_fail "MIBVersion parameter fail"
		echo "MIBVersion parameterfail">>$SAVEFILE
		error_quit
	fi
	
	fun_id=01
	key_id=$(word2string 0xffff) 
	key_type=79
	key_len=01
	key_val=01
	info_id=$(word2string $1) 
	info_type=79 #y
	info_len=01
	info_val=`printf "%02x" $2`
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		echo_fail "config_snmp_status Fail"
		echo "config_snmp_status Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "config_snmp_status $VersionType $VersionState success!"
		echo "config_snmp_status $VersionType $VersionState success!" >>$SAVEFILE
	fi
}

#设置MIB OID
config_snmp_mib_oid()
{
	mib_oid_str="$1"
	
	if [ "$mib_oid_str" != "" ] ; then
		mib_oid_info=$(string2hex "$mib_oid_str")
		strlen=`echo ${#mib_oid_str}`
	else
		mib_oid_info="" 
		strlen=0
	fi

	fun_id=62
	key_id=$(word2string 0xffff) 
	key_type=0
	key_len=01
	key_val=00
	info_id=$(word2string 3) 
	info_type=0
	info_len=`printf "%02x" $strlen`
	info_val="$mib_oid_info"
	
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
    if [ $? != 0 ] ; then
		echo_fail "config_mib_oid Fail"
		echo "config_mib_oid Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "config_mib_oid $mib_oid_str success!"
		echo "config_mib_oid $mib_oid_str success!" >>$SAVEFILE
	fi
}

#开启snmpv3引擎ID更新功能
enable_snmp_engineID_update()
{
    command="f 0 30 93 db 07 00 35 01 00 01 00 00 00 ff ff 79 01 01 0f 00 79 01 01"
    result="0f 31 00 93 00 db 07 00"
    run_cmd "$command" "$result"
    if [ $? != 0 ] ; then
        echo_fail "Enable snmp engine id update failed!"
        echo "Enable snmp engine id update failed!" >>$SAVEFILE
        error_quit
    else
        echo_success "Enable snmp engine id update success!"
        echo "Enable snmp engine id update success!" >>$SAVEFILE
        return 0
    fi
}

#设置NTP的使能状态
set_ntp_enable_status()
{	
	fun_id=81
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
       echo_fail "set NTP enable state $1 fail!"
       echo "set NTP enable status $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set NTP enable status $1 success!"
       return 0
    fi
}

#设置NTP主服务器地址
set_ntp_preferred_server()
{
	if [ "$1" == "" ] ; then
		preferredserver=""
		strlen=0
	else
		preferredserver=$1
		strlen=`echo ${#preferredserver}`
		if [ $strlen -gt 128 ] ; then
			echo_fail "Preferred server addr'len($strlen) is too big!"
			echo "Preferred server addr'len($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		preferredserver=$(string2hex "$preferredserver")
	fi
	
	fun_id=81
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2)
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$preferredserver
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set NTP preferred server $1 fail!"
       echo "set NTP preferred server $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set NTP preferred server $1 success!"
       return 0
    fi
}

#设置NTP备服务器地址
set_ntp_alternative_server()
{
	if [ "$1" == "" ] ; then
		alternativeserver=""
		strlen=0
	else
		alternativeserver=$1
		strlen=`echo ${#alternativeserver}`
		if [ $strlen -gt 128 ] ; then
			echo_fail "Alternative server addr'len($strlen) is too big!"
			echo "Alternative server addr'len($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		alternativeserver=$(string2hex "$alternativeserver")
	fi
	
	fun_id=81
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 3)
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$alternativeserver
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
       echo_fail "set NTP alternative server $1 fail!"
       echo "set NTP alternative server $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set NTP alternative server $1 success!"
       return 0
    fi
}

#设置拓展的NTP服务器地址，ip个数为1至4个，以逗号作间隔
set_ntp_extra_server()
{
	extraserver_arr=(`echo $1 | tr ',' ' '`)
	SUCCESS_STATUS="db 07 00"
	
	#获取拓展NTP服务器可设置个数
	run_cmd "f 0 30 93 DB 07 00 5b 05 00 00 00" "0f 31 00 93 00 db 07 00 06"
    	if [ $? != 0 ] ; then
		extra_ntp_cnt=1
	else 
		extra_ntp_cnt=4
	fi
	
	echo "extra ntp count is $extra_ntp_cnt"
	
	for ((i=0;i<$extra_ntp_cnt;i++))
	do
		if [ $i -lt ${#extraserver_arr[@]} ] ; then
			tmp_ntp_str="${extraserver_arr[$i]}"
		else
			tmp_ntp_str="0.0.0.0"
		fi
		tmp_ntp_hex=$(string2hex "$tmp_ntp_str")
		tmp_ntp_hex=`echo "$tmp_ntp_hex" >tmp_extra_ntp.txt`
		sed -i 's/^/0x/g' tmp_extra_ntp.txt
		sed -i 's/ / 0x/g' tmp_extra_ntp.txt
		tmp_ntp_hex=`cat tmp_extra_ntp.txt`
		for((j=0;j<6;j++))
		do
			ipmitool raw 0x30 0x93 0xdb 0x07 0x00 0x5a 0x06 0x00 0x00 0x00 0x0`expr $i + 2` 0x00 0x00 $tmp_ntp_hex 0x00 >temp_ntpserver.txt 
			
			if [ $? != 0 ] ; then
				echo "set NTP extra server `expr $i + 1` $tmp_ntp_str cmd send fail!" >>$SAVEFILE
				sleep 2
				continue;
			fi
			grep -wq "$SUCCESS_STATUS" temp_ntpserver.txt
			if [ $? != 0 ] ; then
				echo "set NTP extra server `expr $i + 1` $tmp_ntp_str fail!" >>$SAVEFILE
				sleep 2
				continue;
			fi	
			echo_success "set NTP extra server `expr $i + 1` $tmp_ntp_str success!"
			break
		done
		if [ $j == 6 ] ; then
			echo_fail "set NTP extra server $NTPExtraServerAddr fail!"
			echo "set NTP extra server $NTPExtraServerAddr fail!" >>$SAVEFILE
			return 1
		fi
	done
	
	echo_success "set NTP extra server $NTPExtraServerAddr success!"
	return 0
}
#设置NTP服务器个数
set_ntp_count()
{	
	run_cmd "f 0 30 93 DB 07 00 5a 05 00 00 00 $1" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
       echo_fail "set NTP count $1 fail!"
       echo "set NTP count $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set NTP count $1 success!"
       return 0
    fi
}
#设置NTP工作模式
set_ntp_mode()
{	
	fun_id=81
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
       echo_fail "set NTP mode $1 fail!"
       echo "set NTP mode $1 fail!" >>$SAVEFILE
       error_quit
    else 
       echo_success "set NTP mode $1 success!"
       return 0
    fi
}

#设置NTP服务器身份认证使能状态
set_ntp_auth_status()
{
	fun_id=81
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
       echo_fail "set auth enable status $1 fail!"
       echo "set auth enable status $1 fail!" >> $SAVEFILE
       error_quit
    else 
       echo_success "set auth enable status $1 success!"
       return 0
    fi
}

#定制IPv6为DHCP模式
set_ipv6_dhcp_mode()
{
	run_cmd "f 0 0c 01 1 D0 DB 07 00 02" "0f 0d 00 01 00" 
	if [ $? != 0 ] ; then
        echo_fail "Set ipv6 addr DHCP mode fail!"
        echo "Set ipv6 addr DHCP mode fail!" >>$SAVEFILE
        error_quit
   else
        echo_success "Set ipv6 addr DHCP mode success!"
        echo "Set ipv6 addr DHCP mode success!" >>$SAVEFILE
        return 0
   fi
}

set_ipv6_default_addr()
{
	run_cmd "f 0 0c 01 1 CC DB 07 00 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0" "0f 0d 00 01 00"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Set ipv6 default address fail!"
		error_quit
	else
		echo_and_save_success "Set ipv6 default address success!"
	fi

    # 增加清空ipv6默认网关的功能
	run_cmd "f 0 0c 01 1 CE DB 07 00 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0" "0f 0d 00 01 00"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Set ipv6 default gateway fail!"
		error_quit
	else
		echo_and_save_success "Set ipv6 default gateway success!"
        return 0
	fi
}

#查询当前是否支持ipv6协议
check_support_ipv6_mode()
{
	run_cmd "f 0 30 93 DB 07 00 36 58 00 1 ff 00 00 01 80 0 1 0 0c 00" "0f 31 00 93 00 db 07 00 00 0c 00 79 01 00"
	if [ $? == 0 ] ; then		
		echo "BMC can't support ipv6 mode!" >>$SAVEFILE
		return 1
	else
		return 0
	fi
}

# 为避免返厂设备的BMC IP Version被设置为支持IPv4导致设置IPv6地址模式失败的问题，
# 在设置IPv6地址模式之前，先将IP Version设置为默认的IPv4和IPv6都支持
set_default_ip_version()
{
	run_cmd "f 0 30 93 DB 07 00 35 58 00 01 00 00 00 01 00 00 01 00 0C 00 00 01 02" "0f 31 00 93 00 db 07 00"
	if [ $? == 0 ] ; then
		sleep 1
		run_cmd "f 0 30 93 DB 07 00 36 58 00 1 ff 00 00 01 00 0 1 0 0c 00" "0f 31 00 93 00 db 07 00 00 0c 00 79 01 02"
		if [ $? == 0 ]; then				
			return 0
		fi
	fi 
	return 1
}

set_subnet()
{
	echo "$2" > temp.txt
	dos2unix temp.txt > /dev/null 2&>1
	ipaddr=`cat temp.txt | grep "." | awk -F "." '{print $1,$2,$3,$4}'`
	ip_segment_count=`echo $ipaddr | awk '{print split($0, a)}'`
	if [ "$ipaddr" = "" ] || [ "$ip_segment_count" != "4" ]; then
	    echo_fail "Get subnet:$2 fail"
	    echo "Get subnet:$2 fail" >>$SAVEFILE
	    error_quit
	else
	    ip1=`echo $ipaddr | awk -F " " '{print $1}'`
	    ip2=`echo $ipaddr | awk -F " " '{print $2}'`
	    ip3=`echo $ipaddr | awk -F " " '{print $3}'`
	    ip4=`echo $ipaddr | awk -F " " '{print $4}'`

	    ipset1=`printf %02x $ip1`
		if [ $? != 0 ]; then
			echo_fail "Get subnet:$2 fail"
			echo "Get subnet:$2 fail" >>$SAVEFILE
			error_quit
		fi

	    ipset2=`printf %02x $ip2`
		if [ $? != 0 ]; then
			echo_fail "Get subnet:$2 fail"
			echo "Get subnet:$2 fail" >>$SAVEFILE
			error_quit
		fi

	    ipset3=`printf %02x $ip3`
		if [ $? != 0 ]; then
			echo_fail "Get subnet:$2 fail"
			echo "Get subnet:$2 fail" >>$SAVEFILE
			error_quit
		fi

	    ipset4=`printf %02x $ip4`
		if [ $? != 0 ]; then
			echo_fail "Get subnet:$2 fail"
			echo "Get subnet:$2 fail" >>$SAVEFILE
			error_quit
		fi

	    loadip="$ipset1 $ipset2 $ipset3 $ipset4"

	    run_cmd "f 0 c 1 $1 d6 db 07 00 $loadip ff ff ff 00" "0f 0d 00 01 00"
	    if [ $? != 0 ] ; then
	    	echo_fail "Set subnet to $2 failed"
	    	echo "Set subnet to $2 failed" >> $SAVEFILE
	    	error_quit
	    else
	    	echo_success "Set subnet to $2 successfully"
	    	echo "Set subnet to $2 successfully" >> $SAVEFILE
	    fi
	fi
}

config_rack_inner_subnet()
{
	getini $CONFIGFILE BMCSet_RackInner_SubNet
	subnet_segment=$parameter
	default_subnet="172.31.0.0"
	if [ "$subnet_segment" != "" ] ; then
		# 17/18分别为机柜内网通信的2个Channel
		set_subnet "11" $subnet_segment
		set_subnet "12" $subnet_segment
	else
		# 17/18分别为机柜内网通信的2个Channel
		set_subnet "11" $default_subnet
		set_subnet "12" $default_subnet
	fi
}

#设置NCSI通道RX使能,以支持交换机侧配置LACP
set_ncsi_rx_channel_configuration()
{
    local NCSI_RX_Channel=""
    local NCSI_CardType=""
    local PortA=""
    local PortB=""
    local PortC=""
    local PortD=""
	
	# 判断是否支持 设置NCSI通道RX使能
	run_cmd "f 0 30 93 db 07 00 06 19 00 04" "0f 31 00 93 d6"
	if [ $? == 0 ] ; then
	    echo "NCSI is not supported on the board."
		return 0
	fi
	  
	getini  $CONFIGFILE   BMCSet_NCSI_RX_Channel
	NCSI_RX_Channel=$parameter
	getini  $CONFIGFILE  BMCSet_NCSI_RX_Channel_CardType
	NCSI_CardType=$parameter
	getini  $CONFIGFILE BMCSet_NCSI_RX_Channel_PortA
	PortA=$parameter
	getini  $CONFIGFILE BMCSet_NCSI_RX_Channel_PortB
	PortB=$parameter
	getini  $CONFIGFILE BMCSet_NCSI_RX_Channel_PortC
	PortC=$parameter
	getini  $CONFIGFILE BMCSet_NCSI_RX_Channel_PortD
	PortD=$parameter
	if [ "$NCSI_RX_Channel" = "on" ] ; then
	    # 对如参进行校验,异常则退出
	    if [ "$NCSI_CardType" = "" ] || [ "$PortA" = "" ] || [ "$PortB" = "" ]; then
		    echo_fail "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!"	
			echo "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!" >> $SAVEFILE
			error_quit
		fi
		
        if [ "$PortC" != "" ] && [ "$PortD" != "" ] ; then
            run_cmd "f 0 30 93 db 07 00 05 19 00 00 06 01 $NCSI_CardType $PortA $PortB $PortC $PortD" "0f 31 00 93 00 db 07 00"
            if [ $? != 0 ] ; then
                echo_fail "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB $PortC $PortD fail!"
                echo "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB $PortC $PortD fail!" >> $SAVEFILE
                error_quit
            else
                echo_success "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB $PortC $PortD success!"
                echo "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB $PortC $PortD success!" >> $SAVEFILE
            fi
        else
            run_cmd "f 0 30 93 db 07 00 05 19 00 00 04 01 $NCSI_CardType $PortA $PortB" "0f 31 00 93 00 db 07 00" 
            if [ $? != 0 ] ; then
                echo_fail "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!"		
                echo "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!" >> $SAVEFILE
                error_quit
            else
                echo_success "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB success!"
                echo "Enable NCSI RX channels:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB success!" >> $SAVEFILE
            fi
        fi
	else
		run_cmd "f 0 30 93 db 07 00 05 19 00 00 04 00 ff ff ff" "0f 31 00 93 00 db 07 00"
		if [ $? != 0 ] ; then
			echo_fail "Disable NCSI RX channels fail!"
			echo "Disable NCSI RX channels fail!" >> $SAVEFILE
			error_quit
		else
			echo_success "Disable NCSI RX channels success!"
			echo "Disable NCSI RX channels success!" >> $SAVEFILE
		fi		
	fi
}

#设置LLDP使能开关：0：关闭; 1:开启; 
set_LLDP_enable()
{
	fun_id=144
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 1)
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	set_data="$key_id $key_type $key_len $key_val $info_id $info_type $info_len $info_val"
	set_prop_cmd $fun_id 1 "$set_data"
	if [ $? != 0 ] ; then
			echo_fail "set LLDP enable $1 fail!"
			echo "set LLDP enable $1 fail!" >>$SAVEFILE
			error_quit
	else
			echo_success "set LLDP enable $1 success!"
			echo "set LLDP enable $1 success!" >>$SAVEFILE
			return 0
	fi
}

#设置专用网口近端运维IPv4地址
set_maint_ipv4_addr()
{
	if [ "$1" == "" ] ; then
		return 0
	else
		maintipv4=$1
		strlen=`echo ${#maintipv4}`
		if [ $strlen -gt 15 ] ; then
			echo_fail "Maintenance dedicated port IPv4 addrss length($strlen) is too big!"
			echo "Maintenance dedicated port IPv4 addrss length($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		maintipv4=$(string2hex "$maintipv4")
	fi
	
	strlen=`printf "%02x" $strlen`
	run_cmd "f 0 30 93 db 07 00 5a 02 00 $strlen 00 $maintipv4" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
		echo_fail "Set maintenance dedicated port IPv4 address $1 fail!"
		echo "Set maintenance dedicated port IPv4 address $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Set maintenance dedicated port IPv4 address $1 success!"
		echo "Set maintenance dedicated port IPv4 address $1 success!" >>$SAVEFILE
		return 0
	fi
}

#设置专用网口近端运维子网掩码
set_maint_ipv4_subnetmask()
{
	if [ "$1" == "" ] ; then
		return 0
	else
		submask=$1
		strlen=`echo ${#submask}`
		if [ $strlen -gt 15 ] ; then
			echo_fail "Maintenance dedicated port IPv4 submask length($strlen) is too big!"
			echo "Maintenance dedicated port IPv4 submask length($strlen) is too big!" >> $SAVEFILE
			error_quit
		fi
		
		submask=$(string2hex "$submask")
	fi
	
	strlen=`printf "%02x" $strlen`
	run_cmd "f 0 30 93 db 07 00 5a 03 00 $strlen 00 $submask" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
		echo_fail "Set maintenance dedicated port IPv4 submask $1 fail!"
		echo "Set maintenance dedicated port IPv4 submask $1 fail!" >>$SAVEFILE
		error_quit
	else
		echo_success "Set maintenance dedicated port IPv4 submask $1 success!"
		echo "Set maintenance dedicated port IPv4 submask $1 success!" >>$SAVEFILE
		return 0
	fi
}

clear_ip_addr()
{
	#清空对外ip、备份ip、近端运维ip、内网ip
	set_ipaddr 01 "0.0.0.0"
	if [ $? != 0 ] ; then
		echo_fail "Clear eth2 ipv4 Fail"
		echo "Clear eth2 ipv4 Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Clear eth2 ipv4 success!"
		echo "Clear eth2 ipv4 success!" >>$SAVEFILE
	fi
	get_irm_heartbeat_enable
	heartbeat_enable_state=$?
	if [ "$heartbeat_enable_state" = 1 ]; then
		set_ipaddr 10 "0.0.0.0"
		if [ $? != 0 ] ; then
			echo_fail "Clear maint ip Fail"
			echo "Clear maint ip Fail" >>$SAVEFILE
			error_quit
		else
			echo_success "Clear maint ip success!"
			echo "Clear maint ip success!" >>$SAVEFILE
		fi
		set_ipaddr 11 "0.0.0.0"
		if [ $? != 0 ] ; then
			echo_fail "Clear inner ip 1 Fail"
			echo "Clear inner ip 1 Fail" >>$SAVEFILE
			error_quit
		else
			echo_success "Clear inner ip 1 success!"
			echo "Clear inner ip 1 success!" >>$SAVEFILE
		fi
		set_ipaddr 12 "0.0.0.0"
		if [ $? != 0 ] ; then
			echo_fail "Clear inner ip 1 Fail"
			echo "Clear inner ip 2 Fail" >>$SAVEFILE
			error_quit
		else
			echo_success "Clear inner ip 1 success!"
			echo "Clear inner ip 2 success!" >>$SAVEFILE
		fi
	fi
	
    #OSCA、TCE产品不支持backup ip，不做清空操作
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
		#清空backup ip
        run_cmd "f 0 30 93 db 07 00 05 1A 01 00 08 00 00 00 00 FF FF FF 00" "0f 31 00 93 00 db 07 00"	
	    if [ $? != 0 ] ; then
		    echo_fail "Clear back up ip fail"
		    echo "Clear back up ip fail" >>$SAVEFILE
		    error_quit
	    else
		    echo_success "Clear back up ip success"
		    echo "Clear back up ip success" >>$SAVEFILE
	    fi
	fi
	
	return 0
}

#设置中国移动Redfish开关开启 
set_event_customized_state()
{
    run_cmd "f 0 30 93 db 07 00 35 63 00 01 00 00 00 ff ff 00 00 03 00 79 01 $1" "0f 31 00 93 00 db 07 00"
    if [ $? -ne 0 ] ; then
        echo_fail "set event customized state fail!"
        echo "set event customized state fail!" >> $SAVEFILE
        error_quit
    else
        echo_success "set event customized state success!"
        echo "set event customized state success!" >> $SAVEFILE
        return 0
    fi
}

main()
{
	chmod +x *

	# net相关定制化开始前，恢复IP Version到默认（IPv4和IPv6都支持）,避免反厂单板因为客户设置仅支持IPv4或IPv6导致定制化失败
	set_default_ip_version
	if [ $? != 0 ]; then
		echo_fail "Set BMC ip version to IPv4 and IPv6 failed!"
		echo "Set BMC ip version to IPv4 and IPv6 failed!" >>$SAVEFILE
		error_quit
	fi

	# 在运维ip和内网ip修改为192.168.2.X的情况下空定制化会失败，需要清空相关ip，net相关定制化开始前，将外网ip、备份ip、内网ip、运维ip都清空
	clear_ip_addr

	#4.1、设置主机名称 以定制项Custom_SerialNUM作为主机名，原则上不应该将主机名定制成一样的，如果客户强烈要求，由客户自己承担结果
	getini $CONFIGFILE BMCSet_Hostname
	HOSTNAME=$parameter
	if [ "$HOSTNAME" = "on" ] ;then
		getini $CONFIGFILE Custom_SerialNUM
		HOSTNAME=$parameter
		if [ "$HOSTNAME" != "" ] ;then
			set_host_name
			sleep 1
		else
			echo_fail "HOSTNAME is NULL set HOSTNAME fail" 
			echo "HOSTNAME is NULL set HOSTNAME fail" >>$SAVEFILE
			error_quit
		fi
	else
		sleep 1
	fi

	#4.2、设置 Eth2 IPV4或者设置DHCP模式
	getini  $CONFIGFILE  BMCSet_ETH0_DHCPMode
	DHCP=$parameter
	if [ "$DHCP" = "on" ] ; then
		set_eth2_dhcp_mode
		sleep 1	
	else
		getini  $CONFIGFILE  BMCSet_Eth0_Ipv4
		IPADDR=$parameter
		if [ "$IPADDR" = "on" ] ; then
			grep "Custom_Eth0_Ip_Addr" $CONFIGFILE | grep "." > ip.txt
			if [ $? != 0 ] ; then
				echo_fail "IPADDR=NULL set ipaddr fail" 
				echo "IPADDR=NULL set ipaddr fail" >>$SAVEFILE
				error_quit
			else
				dos2unix ip.txt > /dev/null 2&>1
				ipaddr=`cat ip.txt | grep "." |  awk -F "/" '{print $1}'`
				netmask=`cat ip.txt | grep "." |   awk -F "/" '{print $2}'`
				IPADDR=$ipaddr
				IPMASK=$netmask
				IPADDR=`echo $IPADDR  | awk -F "=" '{print $2}'`
				Eth2_IPADDR=$IPADDR
				Eth2_IPMASK=$IPMASK
				set_eth2_ipaddr
				sleep 5
				set_eth2_ipmask
				sleep 5
				set_ipv4_gateway
				sleep 5
			fi
		else
			# OSCA的产品不支持默认开启DHCP功能
			if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
				restore_eth2_ipaddr
				sleep 5
				restore_eth2_ipmask
				sleep 5
				restore_gateway
				sleep 5
			else
				set_eth2_dhcp_mode
				sleep 1	
			fi
		fi
	fi
	
	#恢复backup ip到默认
	restore_backup_ip_and_mask

	#内网vlanID与外网vlanID不能设置一样
	getini $CONFIGFILE  BMCSet_RackInner_VLAN_ID
	VLAN_ID=$parameter

	if [ "$VLAN_ID" == "" ] ; then
		VLAN_ID="702"
	fi

	getini $CONFIGFILE  BMCSet_NCSI_VLAN_ID
	NCSI_VLAN_ID=$parameter

	if [ "$NCSI_VLAN_ID" == "" ] ; then
		NCSI_VLAN_ID="0"
	fi

	if [ "$VLAN_ID" == "$NCSI_VLAN_ID" ] ; then
		echo_and_save_fail "VLAN ID conflict"
		error_quit
	fi
	
	#4.3、网口模式（强制管理网口、强制共享网口、依据管理网口link状态自适应）设置、NCSI vlan id配置，端口绑定
	#E6000刀片类不支持NCSI功能
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_ATLAS_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ]; then
		Set_NET_Mode			
		config_NCSI
	fi
	
	if [ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID = $BOARD_G2500_ID ]; then
		Set_NET_Mode			
		config_NCSI
	fi

	#定制IPv6为DHCP模式
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE BMCSet_Ipv6_DHCPMode
	IPV6_DHCP=$parameter
	if [ "$IPV6_DHCP" != "" ] ; then
		if [ "$IPV6_DHCP" == "on" ] ; then
			set_ipv6_dhcp_mode
			sleep 1
		elif [ "$IPV6_DHCP" == "off" ] ; then
			set_ipv6_default_addr
			sleep 1
		else
			echo_fail "Set ipv6 DHCP parameter error!"
			echo "Set ipv6 DHCP parameter error!" >>$SAVEFILE
		fi
	else
		#没有配置dhcpv6，且不支持ipv6，返回成功
		check_support_ipv6_mode
		if [ $? == 1 ] ; then
			sleep 1
		else
			set_ipv6_default_addr
			sleep 1
		fi
	fi
	fi
	####DNS配置########################################################################
	#4.4、DDNS配置
	#配置项名称BMCSet_DDSN系写错，但是由于各平台已经使用，不做修改
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE  BMCSet_DDSN_AutoIpMode
	DDSN_AutoIpMode=$parameter
			
	if [ "$DDSN_AutoIpMode" = "1" ] ; then
		set_ddns_ipmode 1
	else
		set_ddns_ipmode 0
	fi
	
	getini $CONFIGFILE  BMCSet_DDSN
	DDNS=$parameter
	if [ "$DDNS" = "on" ] ; then
		set_ddns_on
	else
		set_ddns_off   
		set_ddns_domain_name
		set_ddns_primary_domain
		set_ddns_backup_domain
		set_ddns_tertiary_domain
	fi
	sleep 1
	fi
	####网络服务端口配置########################################################################
	#4.5、网络服务端口配置
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	set_service_configuration
	
	#设置SNMP服务开启类型 
	#null: 默认值 关闭V1 V2
	#0: 关闭V1 V2  
	#1: 开启V1 关闭V2  
	#2: 关闭V1 开启V2  
	#3: 开启V1 V2
	#其他值: 参数错误
	getini $CONFIGFILE Custom_MIBVersion
	MIBVersion=$parameter
	if [ "$MIBVersion" == "" ] ;then
		# 无配置型默认关闭V1 V2
		MIBVersion="0"
		string="default V1 off v2 off"
		V1State=0
		V2State=0
	elif [ "$MIBVersion" == "0" ] ;then
		string="V1 off v2 off"
		V1State=0
		V2State=0
	elif [ "$MIBVersion" == "1" ] ;then
		string="V1 on v2 off"
		V1State=1
		V2State=0
	elif [ "$MIBVersion" == "2" ] ;then
		string="V1 off v2 on"
		V1State=0
		V2State=1
	elif [ "$MIBVersion" == "3" ] ;then
		string="V1 on v2 on"
		V1State=1
		V2State=1
	else
		echo_fail "MIBVersion parameter fail"
		echo "MIBVersion parameterfail">>$SAVEFILE
		error_quit
	fi

	config_snmp_mib_version 4 $V1State
	config_snmp_mib_version 5 $V2State
	echo_success "Set MIBVersion $string success"
	echo "Set MIBVersion $string success">>$SAVEFILE
	fi
	
	####SSDP服务功能配置########################################################################
	#4.11 SSDP服务功能配置
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE BMCSet_SSDPEnable
	SSDPEnable=$parameter
	if [ "$SSDPEnable" == "" ] ; then
		SSDPEnable="0";
	fi

    set_prop_cmd 57 1 "03 00 00 04 0D 00 00 00 01 00 00 01 $SSDPEnable"
    if [ $? != 0 ] ; then
        echo_fail "Set SSDP State:$SSDPEnable fail!"
        echo "Set SSDP State:$SSDPEnable fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "set SSDP State:$SSDPEnable success!"
        echo "set SSDP State:$SSDPEnable success!" >>$SAVEFILE
    fi
	#SSDP服务NOTIFY时间间隔设置
	getini $CONFIGFILE BMCSet_SSDP_NOTIFY_INTERVAL
	NotifyInterval=$parameter
	if [ "$NotifyInterval" == "0" ] ; then
		echo_fail "Set SSDP Notify Interval:$NotifyInterval fail!"
        echo "Set SSDP Notify Interval:$NotifyInterval fail!" >>$SAVEFILE
        error_quit 
	fi
	if [ "$NotifyInterval" == "" ] ; then
		NotifyInterval=600;
	fi

	NotifyInterval=$(dword2string $NotifyInterval)
    set_prop_cmd 77 1 "ff ff 00 01 00 04 00 00 04 $NotifyInterval"
    if [ $? != 0 ] ; then
        echo_fail "Set SSDP Notify Interval:$NotifyInterval fail!"
        echo "Set SSDP Notify Interval:$NotifyInterval fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "Set SSDP Notify Interval:$NotifyInterval success!"
        echo "Set SSDP Notify Interval:$NotifyInterval success!" >>$SAVEFILE
    fi
	
	#SSDP服务模式设置
	getini $CONFIGFILE BMCSet_SSDP_MODE
	SSDPMode=$parameter
	if [ "$SSDPMode" == "0" ] ; then
		echo_fail "Set SSDP mode:$SSDPMode fail!"
        echo "Set SSDP mode:$SSDPMode fail!" >>$SAVEFILE
        error_quit 
	fi
	if [ "$SSDPMode" == "" ] ; then
		if [ "$SSDPEnable" == "1" ] ; then
			SSDPMode=3;
		else
			SSDPMode=2;
		fi
	fi

    set_prop_cmd 77 1 "ff ff 00 01 00 01 00 00 01 $SSDPMode"
    if [ $? != 0 ] ; then
        echo_fail "Set SSDP mode:$SSDPMode fail!"
        echo "Set SSDP mode:$SSDPMode fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "Set SSDP mode:$SSDPMode success!"
        echo "Set SSDP mode:$SSDPMode success!" >>$SAVEFILE
    fi
	fi
	####SNMP OID 定制#######################################################################
	#null: 默认值：
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE Custom_MIBOID
	MIBOID=$parameter
	if [ "$MIBOID" == "" ] ;then
		config_snmp_mib_oid ""
	else
		config_snmp_mib_oid $MIBOID
	fi
	
	####SNMP 引擎ID更新#######################################################################
	enable_snmp_engineID_update
	fi
	####NTP 定制#######################################################################
	#设置NTP使能状态
	getini $CONFIGFILE BMCSet_NTPEnableStatus
	NTPEnableStatus=$parameter
	if [ "$NTPEnableStatus" == "" ] ;then
		NTPEnableStatus=0;
	fi
	
	#校验NTP使能入参
	checkInt $NTPEnableStatus
	if [ $? != 0 ] ; then
		echo_fail "NTPEnableStatus'value ($NTPEnableStatus) is wrong";
		echo "NTPEnableStatus'value ($NTPEnableStatus) is wrong" >> $SAVEFILE;
		error_quit
	fi
	
	if [ "$NTPEnableStatus" != "0" ] && [ "$NTPEnableStatus" != "1" ]; then
		echo_fail "NTPEnableStatus'value($NTPEnableStatus) is wrong"
		echo "NTPEnableStatus'value($NTPEnableStatus) is wrong" >> $SAVEFILE;
		error_quit
	fi
	
	set_ntp_enable_status $NTPEnableStatus
	if [ $? != 0 ] ; then
		echo_fail "Set NTP enable status : $NTPEnableStatus fail!"
		echo "Set NTP enable status : $NTPEnableStatus fail!" >> $SAVEFILE
		error_quit
	else
		echo_success "Set NTP enable status : $NTPEnableStatus success!"
		echo "Set NTP enable status : $NTPEnableStatus success!" >> $SAVEFILE
	fi
	#设置NTP个数
	getini $CONFIGFILE BMCSet_NTPCount
	NTPCount=$parameter
	if [ "$NTPCount" == "" ] ;then
		NTPCount=3;
	fi
	#校验入参
	checkInt $NTPCount
	if [ $? != 0 ] ; then
		echo_fail "NTPCount'value ($NTPCount) is wrong";
		echo "NTPCount'value ($NTPCount) is wrong" >> $SAVEFILE;
		error_quit
	fi
	
	if [ "$NTPCount" != "6" ] && [ "$NTPCount" != "3" ]; then
		echo_fail "NTPCount'value($NTPCount) is wrong"
		echo "NTPCount'value($NTPCount) is wrong" >> $SAVEFILE;
		error_quit
	fi
	
	set_ntp_count $NTPCount
	if [ $? != 0 ] ; then
		echo_fail "Set NTP count : $NTPCount fail!"
		echo "Set NTP NTP count : $NTPCount fail!" >> $SAVEFILE
		error_quit
	else
		echo_success "Set NTP count : $NTPCount success!"
		echo "Set NTP count : $NTPCount success!" >> $SAVEFILE
	fi
	
	#设置NTP主服务器地址
	getini $CONFIGFILE BMCSet_NTPPreferredServerAddr
	NTPPreferredServerAddr=$parameter
	set_ntp_preferred_server $NTPPreferredServerAddr
	if [ $? != 0 ] ; then
		echo_fail "Set NTP preferred server : $NTPPreferredServerAddr fail!"
		echo "Set NTP preferred server : $NTPPreferredServerAddr fail!" >> $SAVEFILE
		error_quit
	else
		echo_success "Set NTP preferred server : $NTPPreferredServerAddr success!"
		echo "Set NTP preferred server : $NTPPreferredServerAddr success!" >> $SAVEFILE
	fi
	
	#设置NTP备服务器地址
	getini $CONFIGFILE BMCSet_NTPAlternativeServerAddr
	NTPAlternativeServerAddr=$parameter
	set_ntp_alternative_server $NTPAlternativeServerAddr
	if [ $? != 0 ] ; then
		echo_fail "Set NTP alternative server : $NTPAlternativeServerAddr fail!"
		echo "Set NTP alternative server : $NTPAlternativeServerAddr fail!" >> $SAVEFILE
		error_quit
	else
		echo_success "Set NTP alternative server : $NTPAlternativeServerAddr success!"
		echo "Set NTP alternative server : $NTPAlternativeServerAddr success!" >> $SAVEFILE
	fi
	#设置NTP拓展的服务器地址1
	getini $CONFIGFILE BMCSet_NTPExtraServerAddr
	NTPExtraServerAddr=$parameter
	set_ntp_extra_server $NTPExtraServerAddr
	if [ $? != 0 ] ; then
		echo_fail "Set NTP extra server : $NTPExtraServerAddr fail!"
		echo "Set NTP extra server : $NTPExtraServerAddr fail!" >> $SAVEFILE
		error_quit
	else
		echo_success "Set NTP extra server : $NTPExtraServerAddr success!"
		echo "Set NTP extra server : $NTPExtraServerAddr success!" >> $SAVEFILE
	fi
	
	#设置NTP模式
	getini $CONFIGFILE BMCSet_NTPMode
	NTPMode=$parameter
	if [ "$NTPMode" == "" ] ;then
		NTPMode=1;
	fi
	
	#校验NTP模式入参
	checkInt $NTPMode
	if [ $? != 0 ] ; then
		echo_fail "NTPMode'value ($NTPMode) is wrong";
		echo "NTPMode'value ($NTPMode) is wrong" >> $SAVEFILE;
		error_quit
	fi
	
	if [ "$NTPMode" != "1" ] && [ "$NTPMode" != "2" ] && [ "$NTPMode" != "3" ]; then
		echo_fail "NTPMode'value($NTPMode) is wrong"
		echo "NTPMode'value($NTPMode) is wrong" >> $SAVEFILE;
		error_quit
	fi
	
	set_ntp_mode $NTPMode
	if [ $? != 0 ] ; then
		echo_fail "Set NTP mode : $NTPMode fail!"
		echo "Set NTP NTP mode : $NTPMode fail!" >> $SAVEFILE
		error_quit
	else
		echo_success "Set NTP mode : $NTPMode success!"
		echo "Set NTP mode : $NTPMode success!" >> $SAVEFILE
	fi
	
    #设置NTP认证使能状态
	getini $CONFIGFILE BMCSet_AuthNTPServer
	AuthNTPServer=$parameter
	if [ "$AuthNTPServer" == "" ] ; then
		AuthNTPServer=0;
	fi
	
	if [ "$AuthNTPServer" != "0" ] && [ "$AuthNTPServer" != "1" ]; then
		echo_fail "AuthNTPServer'value($AuthNTPServer) is wrong"
		echo "AuthNTPServer'value($AuthNTPServer) is wrong" >> $SAVEFILE;
		error_quit
	fi
	
	set_ntp_auth_status $AuthNTPServer
	if [ $? != 0 ] ; then
		echo_fail "Set NTP auth status : $AuthNTPServer fail!"
		echo "Set NTP NTP auth status : $AuthNTPServer fail!" >> $SAVEFILE
		error_quit
	else
		echo_success "Set NTP auth status : $AuthNTPServer success!"
		echo "Set NTP auth status : $AuthNTPServer success!" >> $SAVEFILE
	fi

	#设置LLDP使能开关
	getini $CONFIGFILE BMCSet_LLDPEnable
	LLDPEnable=$parameter
	if [ "$LLDPEnable" != "" ] ; then
		if [ "$LLDPEnable" = "on" ] ; then
			set_LLDP_enable 1
		else
			set_LLDP_enable 0
		fi 
	else
		set_LLDP_enable 0
	fi
	
	#设置中国移动Redfish开关使能
	getini $CONFIGFILE BMCSet_EventCustomizedState
	EventCustomizedState=$parameter
	if [ "$EventCustomizedState" = "on" ] ; then
		set_event_customized_state 01
	elif [ "$EventCustomizedState" = "off" ] ; then
		set_event_customized_state 00
	fi
	
	#####设置NCSI通道RX使能,以交换机侧配置LACP########################################################################
    if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ];then
	set_ncsi_rx_channel_configuration
    fi
	get_irm_heartbeat_enable
	heartbeat_enable_state=$?
	if [ "$heartbeat_enable_state" = 1 ]; then
		#分e对机柜内网的两个Channel进行设置
		getini $CONFIGFILE  BMCSet_RackInner_VLAN_ID
		VLAN_ID=$parameter
		if [ "$VLAN_ID" != "" ] ; then
			set_vlan_id $VLAN_ID "11"
			set_vlan_id $VLAN_ID "12"
		else
			#内网默认VLAN为702
			set_vlan_id "702" "11"
			set_vlan_id "702" "12"
		fi

		#设置专用网口近端运维IPv4地址
		getini $CONFIGFILE BMCSet_Maint_IPv4
		MaintIPv4Address=$parameter
		if [ "$MaintIPv4Address" != "" ] ; then
			set_maint_ipv4_addr $MaintIPv4Address
		else
			set_maint_ipv4_addr "192.168.240.100"
		fi	

		#设置专用网口近端运维子网掩码
		getini $CONFIGFILE BMCSet_Maint_SubnetMask
		MaintSubmask=$parameter
		if [ "$MaintSubmask" != "" ] ; then
			set_maint_ipv4_subnetmask $MaintSubmask
		else
			set_maint_ipv4_subnetmask "255.255.255.0"
		fi
		
		config_rack_inner_subnet
	fi
	
	
}

main
