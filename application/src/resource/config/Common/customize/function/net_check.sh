#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#设置主机名称
check_set_host_name()
{	
    echo "$HOSTNAME" > temp.txt 

    #校验主机名称
    #对于HOSTNAME的最大长度为64位做判断
	if [ "${#HOSTNAME}" -gt 64 ] ; then
		echo_fail "Surpasses the greatest length limit 64! restore hostname $HOSTNAME failed!"
		error_quit
	fi
	
	local ipmicode=""
	local req_block=0	
	local begin_hex=0
	local hex_len=0
	while true
	do
		#读取22个字符
		begin_hex=`printf "%x" $[22*req_block]`
		hex_len=`printf "%x" 22`
		((req_block++))
		
		run_cmd "f 0 30 93 db 07 00 06 01 $begin_hex $hex_len" "0f 31 00 93 00 db 07 00"
		grep -wq "0f 31 00 93 00 db 07 00" $IPMI_CMD_RETURN_FILE    	
		if [ $? != 0 ]; then
			echo_fail "verify Hostname fail!"
			echo "verify Hostname fail!" >> $SAVEFILE
			error_quit
		fi	
		
		grep -wq "0f 31 00 93 00 db 07 00 01" $IPMI_CMD_RETURN_FILE 								
		if [ $? -eq 0 ]; then
			ipmicode="$ipmicode`cat $IPMI_CMD_RETURN_FILE | tr -d "\r\n" | awk -F "0f 31 00 93 00 db 07 00 01 " '{printf $2}'`"
			continue
		fi
		
		grep -wq "0f 31 00 93 00 db 07 00 00" $IPMI_CMD_RETURN_FILE	
		if [ $? -eq 0 ]; then
			ipmicode="$ipmicode`cat $IPMI_CMD_RETURN_FILE | tr -d "\r\n" | awk -F "0f 31 00 93 00 db 07 00 00 " '{printf $2}'`"
			break
		fi
	done
	
	echo $ipmicode > hex_tmp.txt
	
	hextochar hex_tmp.txt 1 str_tmp.txt
	READ_HOST_STR=`cat str_tmp.txt`
	if [ "$HOSTNAME" != "$READ_HOST_STR" ] ; then
		echo_fail "verify Hostname:$HOSTNAME fail!"
		echo "verify Hostname:$HOSTNAME fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "verify Hostname:$HOSTNAME success!"
		echo "verify Hostname:$HOSTNAME success!" >>$SAVEFILE
		return 0
	fi
}

#检测主机名称默认值  
check_restore_host_name()
{
	#X/T6000bmc的资产序列号后缀需要BMC复位才生效，所以改为verify.sh中还原hostname
	FRU_ID=0
	AREA_NUMBER=3
	FIELD_NUMBER=4
	READ_ELABEL_STRING=""

	read_elabel
	HOSTNAME=$READ_ELABEL_STRING
	if [ "$HOSTNAME" = "" ] ; then 
	   HOSTNAME="HostName"
	fi
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

	#读取写入的字符串，校验字符串是否写入成功
	local ipmicode=""
	local req_block=0	
	local begin_hex=0
	local hex_len=0
	while true
	do
		#读取22个字符
		begin_hex=`printf "%x" $[22*req_block]`
		hex_len=`printf "%x" 22`
		((req_block++))
		
		run_cmd "f 0 30 93 db 07 00 06 01 $begin_hex $hex_len" "0f 31 00 93 00 db 07 00"
		grep -wq "0f 31 00 93 00 db 07 00" $IPMI_CMD_RETURN_FILE    	
		if [ $? != 0 ]; then
			echo_fail "verify Hostname fail!"
			echo "verify Hostname fail!" >> $SAVEFILE
			error_quit
		fi	
		
		grep -wq "0f 31 00 93 00 db 07 00 01" $IPMI_CMD_RETURN_FILE 								
		if [ $? -eq 0 ]; then
			ipmicode="$ipmicode`cat $IPMI_CMD_RETURN_FILE | tr -d "\r\n" | awk -F "0f 31 00 93 00 db 07 00 01 " '{printf $2}'`"
			continue
		fi
		
		grep -wq "0f 31 00 93 00 db 07 00 00" $IPMI_CMD_RETURN_FILE	
		if [ $? -eq 0 ]; then
			ipmicode="$ipmicode`cat $IPMI_CMD_RETURN_FILE | tr -d "\r\n" | awk -F "0f 31 00 93 00 db 07 00 00 " '{printf $2}'`"
			break
		fi
	done
	
	echo $ipmicode > hex_tmp.txt
	
	hextochar hex_tmp.txt 1 str_tmp.txt
	READ_HOST_STR=`cat str_tmp.txt`
	if [ "$HOSTNAME" != "$READ_HOST_STR" ] ; then
		echo_fail "verify Hostname:$HOSTNAME fail!"
		echo "verify Hostname:$HOSTNAME fail!" >>$SAVEFILE
		error_quit
	else 
		echo_success "verify Hostname:$HOSTNAME success!"
		echo "verify Hostname:$HOSTNAME success!" >>$SAVEFILE
		return 0
	fi
}

#校验Eth2_DHCP模式
check_set_eth2_dhcp_mode()
{
	run_cmd "f 0 0c 02 01 04 00 00" "0f 0d 00 02 00 11 02"
	if [ $? != 0 ] ; then
        echo_fail "Check Eth2 DHCP fail!"
        echo "Check Eth2 DHCP fail!" >>$SAVEFILE
        error_quit
   else
        echo_success "Check Eth2 DHCP success!"
        echo "Check Eth2 DHCP success!" >>$SAVEFILE
        return 0
   fi
}

#校验ip addr
check_set_ipaddr()
{
	Ethport=$1
	IPADDR=$2
    echo "$IPADDR" >temp.txt
    dos2unix temp.txt > /dev/null 2&>1
   
    ipaddr=`cat temp.txt | grep "." |   awk -F "/" '{print $1}' | awk -F "." '{print $1,$2,$3,$4}'`
    if [ "$ipaddr" = "" ] ; then
	    echo_fail "GET IP:$IPADDR fail"
	    echo "GET IP:$IPADDR fail" >>$SAVEFILE
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

	    #校验ip地址
	    run_cmd  "f 0 c 2 $Ethport 3 0 0"  "0f 0d 00 02 00 11 $loadip"
    fi
}

#检测ip mask
check_set_ipmask()
{
	Ethport=$1
	IPMASK=$2
    ipmask=0
    case $IPMASK in
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
	    echo_fail "set netmask $IPMASK fail"
		echo "set netmask $IPMASK fail" >>$SAVEFILE
		error_quit 
	;;
    esac

    run_cmd "f 0 c 2 $Ethport 6 0 0" "0f 0d 00 02 00 11 $ipmask"
} 

#校验gateway
check_gateway()
{
	Ethport=$1
	GATEWAY=$2
	echo "$GATEWAY" >temp.txt
	dos2unix temp.txt > /dev/null 2&>1
	gateway=`cat temp.txt | grep "." |  awk -F "." '{print $1,$2,$3,$4}'`
	if [ "$gateway" = "" ] ; then
		echo_fail "GET IP:$GATEWAY fail"
		echo "GET IP:$GATEWAY fail" >>$SAVEFILE
		error_quit
	else
		#echo gateway: $gateway
		ip1=`echo $gateway | awk -F " " '{print $1}'`
		ip2=`echo $gateway | awk -F " " '{print $2}'`
		ip3=`echo $gateway | awk -F " " '{print $3}'`
		ip4=`echo $gateway | awk -F " " '{print $4}'`
		#echo "$ip1 $ip2 $ip3 $ip4"
		ipset1=`printf %02x $ip1`
		ipset2=`printf %02x $ip2`
		ipset3=`printf %02x $ip3`
		ipset4=`printf %02x $ip4`
		loadip="$ipset1 $ipset2 $ipset3 $ipset4"
		#校验ip地址
		run_cmd  "f 0 c 2 $Ethport c 0 0"  "0f 0d 00 02 00 11 $loadip"
    fi
}

#校验管理网口手动IP地址
check_set_eth2_ipaddr()
{
	check_set_ipaddr  01  $Eth2_IPADDR
	if [ $? != 0 ] ; then
		echo_fail "verify Eth2 Ipaddr:$Eth2_IPADDR Fail"
		echo "verify Eth2 Ipaddr:$Eth2_IPADDR Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "verify Eth2 Ipaddr:$Eth2_IPADDR success"
		echo "verify Eth2 Ipaddr:$Eth2_IPADDR success" >>$SAVEFILE
		return 0	
	fi
}

#校验eth0 ip mask
check_set_eth2_ipmask()
{
	#如果是OSCA产品，ip恢复为空，不需要校验掩码
	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
		return 0
    fi
	
	check_set_ipmask 01 $Eth2_IPMASK
    if [ $? != 0 ] ; then
	    echo_fail "verify Eth2 Netmask: $Eth2_IPMASK fail"
	    echo "verify Eth2 Netmask: $Eth2_IPMASK fail" >>$SAVEFILE
	    error_quit 
    else
	    echo_success "verify Eth2 Netmask: $Eth2_IPMASK success"
	    echo "verify Eth2 Netmask: $Eth2_IPMASK success" >>$SAVEFILE
	    return 0
    fi  
}

# 校验网关
check_ipv4_gateway()
{
	getini $CONFIGFILE  Custom_ipv4_gateway
	ipv4_gateway=$parameter

	if [ "$ipv4_gateway" != "" ] ; then
		check_gateway 01 $ipv4_gateway
		if [ $? != 0 ] ; then
			echo_fail "verify ipv4 gateway: $ipv4_gateway fail"
			echo "verify ipv4 gateway: $ipv4_gateway fail" >>$SAVEFILE
			error_quit
		else
			echo_success "verify ipv4 gateway: $ipv4_gateway success"
			echo "verify ipv4 gateway: $ipv4_gateway success" >>$SAVEFILE
		fi
	else 
		check_gateway 01 $Eth2_IPADDR
		if [ $? != 0 ] ; then
			echo_fail "verify ipv4 gateway: $Eth2_IPADDR fail"
			echo "verify ipv4 gateway: $Eth2_IPADDR fail" >>$SAVEFILE
			error_quit
		else
			echo_success "verify ipv4 gateway: $Eth2_IPADDR success"
			echo "verify ipv4 gateway: $Eth2_IPADDR success" >>$SAVEFILE
		fi
	fi
}

#校验管理网口默认IP地址
check_restore_ipaddr()
{	
	if [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
	    IPADDR=$(get_default_restore_ip_by_product)
	else
	    IPADDR=$(get_default_ip_by_product)
	fi
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
	
		#校验ip地址
		run_cmd  "f 0 c 2 1 3 0 0"  "0f 0d 00 02 00 11 $loadip"
		if [ $? != 0 ] ; then
		    echo_fail "verify BMC IP:$IPADDR Fail"
		    echo "verify BMC IP:$IPADDR Fail" >>$SAVEFILE
		    error_quit
		else
		    echo_success "verify BMC IP:$IPADDR success"
		    echo "verify BMC IP:$IPADDR success" >>$SAVEFILE	
		    return 0
		fi
	fi
}


check_default_gateway()
{
	#如果是OSCA产品，ip恢复为空，不需要校验网关
	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
		return 0
	fi
	
	ipv4_gateway=$(get_default_ip_by_product)
	echo "$ipv4_gateway" >temp.txt
	dos2unix temp.txt
	ipaddr=`cat temp.txt | grep "." |   awk -F "/" '{print $1}' | awk -F "." '{print $1,$2,$3,$4}'`
	if [ "$ipaddr" = "" ] ; then
	    echo_fail "GET gateway:$IPADDR error"
	    echo "GET gateway:$IPADDR error" >>$SAVEFILE
	    error_quit 
	else
		#echo gateway:$ipaddr
		ip1=`echo $ipaddr | awk -F " " '{print $1}'`
		ip2=`echo $ipaddr | awk -F " " '{print $2}'`
		ip3=`echo $ipaddr | awk -F " " '{print $3}'`
		ip4=`echo $ipaddr | awk -F " " '{print $4}'`
		ipset1=`printf %02x $ip1`
		ipset2=`printf %02x $ip2`
		ipset3=`printf %02x $ip3`
		ipset4=`printf %02x $ip4`
		loadip="$ipset1 $ipset2 $ipset3 $ipset4"
		#校验ip地址
		run_cmd  "f 0 c 2 1 c 0 0"  "0f 0d 00 02 00 11 $loadip"
		if [ $? != 0 ] ; then
			echo_fail "verify BMC default gateway:$IPADDR Fail"
			echo "verify BMC default gateway:$IPADDR Fail" >>$SAVEFILE
			error_quit
		else
			echo_success "verify BMC default gateway:$IPADDR ok"
			echo "verify BMC default gateway:$IPADDR ok" >>$SAVEFILE
			return 0
		fi
	fi
}

#校验默认backup ip
check_default_backup_ip()
{
	#OSCA产品不支持backup ip
	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ]; then
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
	
		#获取backup ip并进行校验	
		run_cmd  "f 0 30 93 db 07 00 06 1A 0 0a"  "$loadip $ipmask"
			
		if [ $? != 0 ] ; then
			echo_fail "Check default back up ip $IPADDR fail"
			echo "Check default back up ip $IPADDR fail" >>$SAVEFILE
			error_quit
		else
			echo_success "Check default back up ip $IPADDR success"
			echo "Check default back up ip $IPADDR success" >>$SAVEFILE
		fi
	fi
}

#检查网口模式设置的准确性 
check_NET_Mode()
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
		value=03
		NET_Mode="auto"
	fi
	
	value=`printf %02x $value`
	run_cmd "f 0 0c 02 01 d7 0 0"  "0f 0d 00 02 00 11 $(get_custom_manu_id) $value"
	if [ $? != 0 ] ; then
		echo_and_save_fail "check Set NET Mode: $NET_Mode fail"
		error_quit
	else
		echo_and_save_success "check Set NET Mode: $NET_Mode success"
		sleep 1
	fi	
	
	check_Set_NCSI_port $value
}

#校验默认的vlanid是否为0
check_restore_NCSI_vlan_id()
{
	run_cmd "f 0 0c 02 01 14 00 00"  "0f 0d 00 02 00 11 00 00"
	if [ $? != 0 ] ; then
        echo_fail "check restore NCSI vlan id $VLAN_ID fail!"
        echo "check restore NCSI vlan id $VLAN_ID fail!" >>$SAVEFILE
        error_quit
    else
        echo_success "check restore NCSI vlan id $VLAN_ID success!"
        echo "check restore NCSI vlan id $VLAN_ID success!" >>$SAVEFILE
        return 0
    fi
}

#校验NCSI Port Num
check_Set_NCSI_port()
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
		echo "NCSIPortNum=$NCSIPortNum">>$SAVEFILE
		run_cmd "f 0 30 93 db 07 00 06 0D 00 02"  "0f 31 00 93 00 db 07 00 00 $NCSIPortNum $net_mode"
		if [ $? = 0 ] ; then
			echo_success "Check NCSI Port Num:$NCSIPortNum success"
			echo "Check NCSI Port Num:$NCSIPortNum success">>$SAVEFILE
			sleep 1
		else
			echo_fail "Check NCSI Port Num:$NCSIPortNum fail"
			echo "Check NCSI Port Num:$NCSIPortNum fail">>$SAVEFILE
			error_quit
		fi
	fi
}

check_vlan_id()
{
	vlanid=`printf "%04x" ${1}`
	vlanidlow=${vlanid:2}
	vlanidhigh=${vlanid:0:2}
	vlanidhigh=`printf "%d" 0x$vlanidhigh`
	if [ "$vlanid" != "0000" ]; then
		vlanidhigh=`expr $vlanidhigh + 128`
	fi
	
	vlanidhigh=`printf "%02x" $vlanidhigh`
	
	run_cmd "f 0 0c 02 ${2} 14 00 00"  "0f 0d 00 02 00 11 $vlanidlow $vlanidhigh"
	if [ $? != 0 ] ; then
		echo_fail "check vlan id ${1} fail!"
		echo "check vlan id ${1} fail!" >>$SAVEFILE
		error_quit
    else
		echo_success "check vlan id ${1} success!"
		echo "check vlan id ${1} success!" >>$SAVEFILE
		return 0
    fi
}

#校验NCSI VLAN ID
check_Set_NCSI_vlan_id()
{
	getini $CONFIGFILE  BMCSet_NCSI_VLAN_ID
	VLAN_ID=$parameter
	if [ "$VLAN_ID" != "" ] ; then
		check_vlan_id "$VLAN_ID" "1"
	else
		check_vlan_id "0" "1"
	fi
}

#校验机柜内网通信VLAN Id
check_set_rack_inner_vlan_id()
{
	getini $CONFIGFILE  BMCSet_RackInner_VLAN_ID
	VLAN_ID=$parameter
	if [ "$VLAN_ID" != "" ] ; then
		check_vlan_id "$VLAN_ID" "11"
		check_vlan_id "$VLAN_ID" "12"
	else
		check_vlan_id "702" "11"
		check_vlan_id "702" "12"
	fi
}

#开启DDNS功能
check_set_ddns_on()
{
    #校验ddns是否开启	
	get_prop_cmd 3 "09 00 75 04 05 00 00 00" 6 "06 00 79 01 01"
	if [ $? != 0 ] ; then
	   echo_fail "verify ddns on fail!"
	   echo "verify ddns on fail!" >>$SAVEFILE
	   error_quit
	else
	   echo_success "verify ddns on success!"
	   echo "verify ddns on success!" >>$SAVEFILE
	   return 0
	fi
}

#关闭DDNS功能
check_set_ddns_off()
{
    #校验ddns是否开启
	get_prop_cmd 3 "09 00 75 04 05 00 00 00" 6 "06 00 79 01 00"
	if [ $? != 0 ] ; then
	   echo_fail "verify ddns off fail!"
	   echo "verify ddns off fail!" >>$SAVEFILE
	   error_quit
	else
	   echo_success "verify ddns off success!"
	   echo "verify ddns off success!" >>$SAVEFILE
	   return 0
	fi
}

#检查DDNS ip模式：0：IPV4；1：IPV6
check_ddns_ipmode()
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
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 5 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check DDSN_AutoIpMode is $info_val fail"
		echo "Check DDSN_AutoIpMode is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check DDSN_AutoIpMode is $info_val success"
		echo "Check DDSN_AutoIpMode is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#设置DDNS域名校验
check_set_ddns_domain_name()
{
	getini $CONFIGFILE BMCSet_DDSN_DomainName
	strval=$parameter
	strlen=0

	if [ "$strval" != "" ] ; then
		strlen=`echo ${#strval}`
		strval=$(string2hex "$strval")
	else
		strval=""
		strlen=0
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
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 1 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check DDSN_DomainName address fail"
		echo "Check DDSN_DomainName address fail" >> $SAVEFILE
		error_quit		
	else
		echo_success "Check DDSN_DomainName address success"
		echo "Check DDSN_DomainName address success" >>$SAVEFILE
		return 0
	fi
}

#设置DDNS首选服务器校验
check_set_ddns_primary_domain()
{
	getini $CONFIGFILE BMCSet_DDSN_PrimaryDomain
	strval=$parameter
	strlen=0

	if [ "$strval" != "" ] ; then
		strlen=`echo ${#strval}`
		strval=$(string2hex "$strval")
	else
		strval=""
		strlen=0
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
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 2 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check DDSN_PrimaryDomain address fail"
		echo "Check DDSN_PrimaryDomain address fail" >> $SAVEFILE
		error_quit		
	else
		echo_success "Check DDSN_PrimaryDomain address success"
		echo "Check DDSN_PrimaryDomain address success" >>$SAVEFILE
		return 0
	fi
}

#设置DDNS备用服务器校验
check_set_ddns_backup_domain()
{
	getini $CONFIGFILE BMCSet_DDSN_BackupDomain
	strval=$parameter
	strlen=0

	if [ "$strval" != "" ] ; then
		strlen=`echo ${#strval}`
		strval=$(string2hex "$strval")
	else
		strval=""
		strlen=0
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
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 3 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check DDSN_BackupDomain address fail"
		echo "Check DDSN_BackupDomain address fail" >> $SAVEFILE
		error_quit		
	else
		echo_success "Check DDSN_BackupDomain address success"
		echo "Check DDSN_BackupDomain address success" >>$SAVEFILE
		return 0
	fi
}

#设置DDNS第二备用服务器校验
check_set_ddns_tertiary_domain()
{
	getini $CONFIGFILE BMCSet_DDSN_TertiaryDomain
	strval=$parameter
	strlen=0

	if [ "$strval" != "" ] ; then
		strlen=`echo ${#strval}`
		strval=$(string2hex "$strval")
	else
		strval=""
		strlen=0
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
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 10 "$expect_data"
	if [ $? != 0 ] ; then
		echo_fail "Check DDSN_TertiaryDomain address fail"
		echo "Check DDSN_TertiaryDomain address fail" >> $SAVEFILE
		error_quit		
	else
		echo_success "Check DDSN_TertiaryDomain address success"
		echo "Check DDSN_TertiaryDomain address success" >>$SAVEFILE
		return 0
	fi
}


#ssh/http/https定制化端口配置是否正确
common_check_service_port_config()
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
	if [ "$ServicePortId" = "on" ] ; then			
		#获取配置服务端口的网络端口号
		getini $CONFIGFILE Custom_${SERVERICE_TYPE}_ServicePort${INDEX_ID}
		ServicePort=$parameter
		is_digit $ServicePort
		if [ $? != 0 ]; then
			echo_fail "Check $service_name ServicePort:$ServicePort failed! port value(1~65535)"
			error_quit
		fi
		#对于port的有效判断
		if [ "$ServicePort" -gt "65535" ] || [ "$ServicePort" -lt "1" ] ; then
			echo_fail "Check $service_name ServicePort: $ServicePort failed! portvalue(1~65535)"
			error_quit
		fi			
	else
		getini defaultserverport.ini ${SERVERICE_TYPE}_${INDEX_ID}_PORT
		ServicePort=$parameter
	fi
	
	key_id=$(word2string $KEY_ID) 
	key_type=75   #'u'
	key_len="04"
	key_val=$(dword2string $KEY_VALUE)
	info_id=$(word2string $INFO_ID) 
	info_type=71   #'q'
	info_len="02"
	info_val=$(word2string $ServicePort)
	
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
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $FUNCTION_ID "$key_id $key_type $key_len $key_val" $INFO_ID "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check ${service_name}_ServicePort is $ServicePort fail"
		echo "Check ${service_name}_ServicePort is $ServicePort fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check ${service_name}_ServicePort is $ServicePort success"
		echo "Check ${service_name}_ServicePort is $ServicePort success" >>$SAVEFILE
   	fi	
}

#ssh/http/https等支持定制化使能
common_check_service_enable_config()
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
	
	service_name=$(get_net_service_name $SERVERICE_TYPE $INDEX_ID)
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

	if [ "$SERVERICE_TYPE" == "RemoteControl" ] && [ "$INDEX_ID" == "2" ]  && [ "$enable_state" == "1" ] ; 
	then
		get_remote_ctrl_vmm_state
		enable_state=$?
	fi	
		
	if [ "$enable_state" = "0" ] ; then
		ServiceControl=off
	else
		ServiceControl=on
	fi
	
	key_id=$(word2string $KEY_ID) 
	key_type=75   #'u'
	key_len="04"
	key_val=$(dword2string $KEY_VALUE)
	info_id=$(word2string $INFO_ID) 
	info_type=79   #'y'
	info_len="01"
	info_val=`printf "%02x" $enable_state`

	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $FUNCTION_ID "$key_id $key_type $key_len $key_val" $INFO_ID "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check ${service_name}_ServiceControl:$ServiceControl fail"
		echo "Check ${service_name}_ServiceControl:$ServiceControl fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check ${service_name}_ServiceControl:$ServiceControl success"
		echo "Check ${service_name}_ServiceControl:$ServiceControl success" >>$SAVEFILE
   	fi	

}

#服务配置
check_set_service_configuration()
{
	SSH_CONFIG="SSH,1,6,3,2"
	HTTP_CONFIG="WEBServer,1,6,3,9"
	HTTPS_CONFIG="WEBServer,2,6,3,11"
	SNMP_CONFIG="SNMPAgent,1,1,10,33"
	RMCP_CONFIG="IPMILAN,1,5,6,32"
	RMCP_PLUS_CONFIG="IPMILAN,2,5,6,32"
	KVM_CONFIG="RemoteControl,1,10,65535,0"  #kvm
	VMM_CONFIG="RemoteControl,2,11,65535,0"  #vmm
	VIDEO_CONFIG="Video,1,12,65535,0"  		 #video

	for CONFIG_INFO in $SSH_CONFIG $HTTP_CONFIG $HTTPS_CONFIG $RMCP_CONFIG $SNMP_CONFIG $RMCP_PLUS_CONFIG $KVM_CONFIG $VMM_CONFIG $VIDEO_CONFIG;
	do		
		#服务启用控制
		common_check_service_enable_config $CONFIG_INFO
		#服务启用控制
		common_check_service_port_config $CONFIG_INFO	
	done

}

#检查MIB版本设置(使用check)
check_snmp_mib_version()
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
	
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" $1 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "check_snmp_mib_version Fail"
		echo "check_snmp_mib_version Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_snmp_mib_version $VersionType $VersionState success!"
		echo "check_snmp_mib_version $VersionType $VersionState success!" >>$SAVEFILE
	fi
}


#检查MIB OID 设置(使用check)
check_snmp_mib_oid()
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
	info_type=73
	info_len=`printf "%02x" $strlen`
	info_val=$mib_oid_info

	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 3 "$exp_data"
    if [ $? != 0 ] ; then
		echo_fail "check_snmp_mib_oid Fail"
		echo "check_snmp_mib_oid Fail" >>$SAVEFILE
		error_quit
	else
		echo_success "check_snmp_mib_oid $mib_oid_str success!"
		echo "check_snmp_mib_oid $mib_oid_str success!" >>$SAVEFILE
	fi
}

#检查开启snmpv3引擎ID更新功能是否开启
check_enable_snmp_engineID_update()
{
    command="f 0 30 93 db 07 00 36 01 00 01 ff 00 00 ff ff 79 01 00 0f 00"
    result="0f 31 00 93 00 db 07 00 00 0f 00 79 01 01"
    run_cmd "$command" "$result"
    if [ $? != 0 ] ; then
        echo_fail "Check enable snmp engine id update failed!"
        echo "Check enable snmp engine id update failed!" >>$SAVEFILE
        error_quit
    else
        echo_success "Check enable snmp engine id update success!"
        echo "Check enable snmp engine id update success!" >>$SAVEFILE
        return 0
    fi
}

#检查NTP使能状态
check_ntp_enable_status()
{
	fun_id=81
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
       	echo_fail "Check NTP enable status is $info_val fail"
		echo "Check NTP enable status is $info_val fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check NTP enable status is $info_val success"
		echo "Check NTP enable is $info_val success" >>$SAVEFILE
		return 0
   	fi
}

#检查NTP主服务器地址
check_ntp_preferred_server()
{
	if [ "$1" != "" ] ; then
		preferredserver=$1
		strlen=`echo ${#preferredserver}`
		preferredserver=$(string2hex "$preferredserver")
	else
		#删除服务器地址
		preferredserver=""
		strlen=0
	fi
	
	fun_id=81
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 2) 
	info_type=73 #s
	info_len=`printf "%02x" $strlen` 
	info_val=$preferredserver
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 2 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check NTP preferred server address fail"
		echo "Check NTP preferred server address fail" >> $SAVEFILE
		error_quit		
   	else
		echo_success "Check NTP preferred server address success"
		echo "Check NTP preferred server address success" >>$SAVEFILE
		return 0
   	fi
}

#检查NTP备服务器地址
check_ntp_alternative_server()
{
	if [ "$1" != "" ] ; then
		alternativeserver=$1
		strlen=`echo ${#alternativeserver}`
		alternativeserver=$(string2hex "$alternativeserver")
	else
		#删除服务器地址
		alternativeserver=""
		strlen=0
	fi
	
	fun_id=81
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 3) 
	info_type=73 #s
	info_len=`printf "%02x" $strlen` 
	info_val=$alternativeserver
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 3 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check NTP alternative server address fail"
		echo "Check NTP alternative server address fail" >> $SAVEFILE
		error_quit		
   	else
		echo_success "Check NTP alternative server address success"
		echo "Check NTP alternative server address success" >>$SAVEFILE
		return 0
   	fi
}
#检查NTP拓展的服务器地址
check_ntp_extra_server()
{
	extraserver_arr=(`echo $1 | tr ',' ' '`)
	echo "Check NTP extra server address $1"
	
	#获取拓展NTP服务器可设置个数
	run_cmd "f 0 30 93 DB 07 00 5b 05 00 00 00" "0f 31 00 93 00 db 07 00 06"
	if [ $? != 0 ] ; then
		extra_ntp_cnt=1
	else 
		extra_ntp_cnt=4
	fi	
	echo "extra ntp count is $extra_ntp_cnt"
	
	for ((i=0;i<extra_ntp_cnt;i++))
	do
		ipmitool raw 0x30 0x93 0xdb 0x07 0x00 0x5b 0x06 0x00 0x00 0x00 0x0`expr $i + 2`>temp_ntpserver.txt 
		if [ $? != 0 ] ; then
			echo_fail "Check NTP extra server `expr $i + 1` address fail"
			echo "Check NTP extra server `expr $i + 1` address fail" >> $SAVEFILE
			error_quit
		fi
				
		echo `cat temp_ntpserver.txt`>temp_ntpserver.txt  #去除换行符
		if [ $i -lt ${#extraserver_arr[@]} ] ; then
			extraserver_arr[$i]=$(string2hex "${extraserver_arr[$i]}")
			if [ $? != 0 ] ; then
				echo_fail "Check NTP extra server `expr $i + 1` address fail"
				echo "Check NTP extra server `expr $i + 1` address fail" >> $SAVEFILE
				error_quit
			fi
			init=`grep -wq "db 07 00 ${extraserver_arr[$i]} 00" temp_ntpserver.txt`
		else
			init=`grep -wq "db 07 00 00" temp_ntpserver.txt`  #未配置NTP server时的检查
		fi
		if [ $? != 0 ] ; then
			echo_fail "Check NTP extra server `expr $i + 1` fail!expect data is $1"
			echo "Check NTP extra server `expr $i + 1` fail!expect data is $1" >>$SAVEFILE
			error_quit
		fi
		if [ x"$init" != x ] ; then
			echo_fail "Check NTP extra server `expr $i + 1` fail:$init"
			echo "Check NTP extra server `expr $i + 1` fail:$init" >>$SAVEFILE
			error_quit
		fi
	done

	echo_success "Check NTP extra server $1 success:$init"
	return 0
}

#检查NTP server的个数
check_ntp_count()
{
    run_cmd "f 0 30 93 DB 07 00 5b 05 00 00 00" "0f 31 00 93 00 db 07 00 0$1"
    if [ $? != 0 ] ; then
       	echo_fail "Check NTP count is $1 fail"
		echo "Check NTP count is $1 fail" >>$SAVEFILE
		error_quit		
    else
		echo_success "Check NTP count is $1 success"
		echo "Check NTP count is $1 success" >>$SAVEFILE
		return 0
    fi
}
check_ntp_mode()
{
	fun_id=81
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 4) 
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 4 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check NTP mode is $info_val fail"
		echo "Check NTP mode is $info_val fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check NTP mode is $info_val success"
		echo "Check NTP mode is $info_val success" >>$SAVEFILE
		return 0
   	fi
}

check_ntp_auth_enable()
{
	fun_id=81
	key_id=$(word2string 0xffff)
	key_type=00
	key_len=01
	key_val=00
	info_id=$(word2string 5) 
	info_type=79
	info_len=01
	info_val=`printf "%02x" $1`
	
	expect_data="$info_id $info_type $info_len $info_val"
	
	get_prop_cmd $fun_id "$key_id $key_type $key_len $key_val" 5 "$expect_data"
    if [ $? != 0 ] ; then
       	echo_fail "Check NTP auth enable is $info_val fail"
		echo "Check NTP auth enable is $info_val fail" >>$SAVEFILE
		error_quit		
   	else
		echo_success "Check NTP auth enable is $info_val success"
		echo "Check NTP auth enable is $info_val success" >>$SAVEFILE
		return 0
   	fi
}

#校验IPv6为DHCP模式
check_ipv6_dhcp_mode()
{
	run_cmd "f 0 0c 02 1 D0 00 00" "0f 0d 00 02 00 11 $(get_custom_manu_id) 02"
	if [ $? != 0 ] ; then
        echo_and_save_fail "Check ipv6 addr DHCP mode fail!"
        error_quit
   else
        echo_and_save_success "Check ipv6 addr DHCP mode success!"
        return 0
   fi
}

check_ipv6_static_mode()
{
	run_cmd "f 0 0c 02 1 D0 00 00" "0f 0d 00 02 00 11 $(get_custom_manu_id) 01"
	if [ $? != 0 ] ; then 
		echo_and_save_fail "Check ipv6 static mode fail!"
		error_quit
	else
		echo_and_save_success "Check ipv6 static mode success!"
		return 0
	fi
}

check_ipv6_default_addr()
{
	run_cmd "f 0 0c 02 1 CC 00 00" "0f 0d 00 02 00 11 $(get_custom_manu_id) 00 00 00 00 00 00 00 00 00 00 00 
             00 00 00 00 00 "
	if [ $? != 0 ] ; then 
		echo_and_save_fail "Check ipv6 default address fail!"
		error_quit
	else
		echo_and_save_success "Check ipv6 default address success!"
	fi

    # 校验ipv6默认网关是否被清空
	run_cmd "f 0 0c 02 1 CE 00 00" "0f 0d 00 02 00 11 db 07 00 00 00 00 00 00 00 00 00 00 00 00 
             00 00 00 00 00 "
	if [ $? != 0 ] ; then 
		echo_and_save_fail "Check ipv6 default gateway fail!"
		error_quit
	else
		echo_and_save_success "Check ipv6 default gateway success!"
		return 0
	fi
}

check_ipv6_default_prefix()
{
	run_cmd "f 0 0c 02 1 CD 00 00" "0f 0d 00 02 00 11 $(get_custom_manu_id) 00"
	if [ $? != 0 ] ; then
		echo_and_save_fail "Check ipv6 address prefix fail!"
		error_quit
	else
		echo_and_save_success "Check ipv6 address prefix success!"
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

#校验NCSI通道RX使能
check_ncsi_rx_channel_configuration()
{
    local NCSI_RX_Channel=""
    local NCSI_CardType=""
    local PortA=""
    local PortB=""
    local PortC=""
    local PortD=""
	
	# 判断是否支持 校验NCSI通道RX使能
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
			echo_fail "Check NCSI RX channel:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!"	
			echo "Check NCSI RX channel:$NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!" >> $SAVEFILE
			error_quit
		fi
		
        if [ "$PortC" != "" ] && [ "$PortD" != "" ] ; then
            run_cmd "f 0 30 93 db 07 00 06 19 00 06" "0f 31 00 93 00 db 07 00 00 01 `printf %02x $NCSI_CardType` `printf %02x $PortA` `printf %02x $PortB` `printf %02x $PortC` `printf %02x $PortD`"	
            if [ $? != 0 ] ; then
                echo_fail "Check NCSI RX channel: $NCSI_RX_Channel $NCSI_CardType $PortA $PortB $PortC $PortD fail!"
                echo "Check NCSI RX channel: $NCSI_RX_Channel $NCSI_CardType $PortA $PortB $PortC $PortD fail!" >> $SAVEFILE
                error_quit
            else
                echo_success "Check NCSI RX channel: $NCSI_RX_Channel  $NCSI_CardType $PortA $PortB $PortC $PortD success!"
                echo "Check NCSI RX channel: $NCSI_RX_Channel  $NCSI_CardType $PortA $PortB $PortC $PortD success!" >> $SAVEFILE
            fi
        else
            run_cmd "f 0 30 93 db 07 00 06 19 00 04" "0f 31 00 93 00 db 07 00 00 01 `printf %02x $NCSI_CardType` `printf %02x $PortA` `printf %02x $PortB`"	
            if [ $? != 0 ] ; then
                echo_fail "Check NCSI RX channel: $NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!"
                echo "Check NCSI RX channel: $NCSI_RX_Channel $NCSI_CardType $PortA $PortB fail!" >> $SAVEFILE
                error_quit
            else
                echo_success "Check NCSI RX channel: $NCSI_RX_Channel  $NCSI_CardType $PortA $PortB success!"
                echo "Check NCSI RX channel: $NCSI_RX_Channel  $NCSI_CardType $PortA $PortB success!" >> $SAVEFILE
            fi
        fi
	else
		run_cmd "f 0 30 93 db 07 00 06 19 00 04" "0f 31 00 93 00 db 07 00 00 00 ff ff ff"
		if [ $? != 0 ] ; then
			#没有网口的时候，命令返回cc； 要确认是不是没有网口的场景
			run_cmd "f 0 30 93 db 07 00 06 19 00 04" "0f 31 00 93 cc"
			if [ $? != 0 ] ; then
				echo_fail "Check NCSI RX channel fail!"		
				echo "Check NCSI RX channel fail!" >> $SAVEFILE
				error_quit				
			else
				echo_success "Check NCSI RX channel success!"
				echo "Check NCSI RX channel success!" >> $SAVEFILE
			fi
		else
			echo_success "Check NCSI RX channel success!"
			echo "Check NCSI RX channel success!" >> $SAVEFILE
		fi		
	fi
}


check_subnet()
{
	echo "$2" > temp.txt
   	dos2unix temp.txt > /dev/null 2&>1
   	ipaddr=`cat temp.txt | grep "." |   awk -F "/" '{print $1}' | awk -F "." '{print $1,$2,$3,$4}'`
   	ip_segment_count=`echo $ipaddr | awk '{print split($0, a)}'`
	if [ "$ipaddr" = "" ] || [ "$ip_segment_count" != "4" ]; then
	    echo_fail "Get subnet:$2 fail"
	    echo "Get subnet:$2 fail" >>$SAVEFILE
	    error_quit
   	else
	    ip1=`echo $ipaddr | awk -F " " '{print $1}'`
	    ip2=`echo $ipaddr | awk -F " " '{print $2}'`
	    ip3=`echo $ipaddr | awk -F " " '{print $3}'`

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

		#不检查末字节
	    loadip="$ipset1 $ipset2 $ipset3 00"

	    run_cmd "f 0 c 2 $1 d6 00 00" "0f 0d 00 02 00 11 $(get_custom_manu_id) $loadip"
	    if [ $? != 0 ] ; then
		echo_and_save_fail "Check subnet with $2 failed"
		error_quit
	    else
		echo_and_save_success "Check subnet with $2 successfully"
	    fi
   	fi
}

check_rack_inner_subnet()
{
	getini $CONFIGFILE BMCSet_RackInner_SubNet
	subnet_segment=$parameter
	default_subnet="172.31.0.0"
	if [ "$subnet_segment" != "" ] ; then
		# 17/18分别为机柜内网通信的2个Channel
		check_subnet "11" $subnet_segment
		check_subnet "12" $subnet_segment
	else
		# 17/18分别为机柜内网通信的2个Channel
		check_subnet "11" $default_subnet
		check_subnet "12" $default_subnet
	fi
}

#检查LLDP使能开关：0：关闭; 1:开启
check_LLDP_enable()
{
	fun_id=144
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
		echo_fail "Check LLDP enable is $info_val fail"
		echo "Check LLDP enable is $info_val fail" >>$SAVEFILE
		error_quit
	else
		echo_success "Check LLDP enable is $info_val success"
		echo "Check LLDP enable is $info_val success" >>$SAVEFILE
		return 0
	fi
}

#检查专用网口近端运维IPv4地址
check_maint_ipv4_address()
{
	if [ "$1" != "" ] ; then
		maintipv4=$1
		strlen=`echo ${#maintipv4}`
		maintipv4=$(string2hex "$maintipv4")
	else
		return 0
	fi
	
	run_cmd "f 0 30 93 db 07 00 5b 02 00 01 00 00" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
       	echo_fail "Check maintenance dedicated port IPv4 address fail"
		echo "Check maintenance dedicated port IPv4 address fail" >> $SAVEFILE
		error_quit		
   	else
		cmd_result=""
		for line in `cat $IPMI_CMD_RETURN_FILE`
		do
			line=$(remove_space "$line")
			cmd_result=$cmd_result" $line"
		done
		cmd_result=$(remove_space "$cmd_result")
		echo $cmd_result
		expect_string="0f 31 00 93 00 db 07 00 $maintipv4"
		expect_string=$(remove_space "$expect_string")
		echo $expect_string
        if [ "$expect_string" = "$cmd_result" ] ; then
		    echo_success "Check maintenance dedicated port IPv4 address success"
			echo "Check maintenance dedicated port IPv4 address success" >>$SAVEFILE
			return 0
		else
			echo_fail "Check maintenance dedicated port IPv4 address fail"
			echo "Check maintenance dedicated port IPv4 address fail" >> $SAVEFILE
			error_quit
        fi
   	fi
}

#检查专用网口近端运维子网掩码
check_maint_ipv4_subnetmask()
{
	if [ "$1" != "" ] ; then
		maintsubmask=$1
		strlen=`echo ${#maintsubmask}`
		maintsubmask=$(string2hex "$maintsubmask")
	else
		return 0
	fi
	
	run_cmd "f 0 30 93 db 07 00 5b 03 00 01 00 00" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ] ; then
       	echo_fail "Check maintenance dedicated port IPv4 submask fail"
		echo "Check maintenance dedicated port IPv4 submask fail" >> $SAVEFILE
		error_quit		
   	else
		cmd_result=""
		for line in `cat $IPMI_CMD_RETURN_FILE`
		do
			line=$(remove_space "$line")
			cmd_result=$cmd_result" $line"
		done
		cmd_result=$(remove_space "$cmd_result")
		echo $cmd_result
		expect_string="0f 31 00 93 00 db 07 00 $maintsubmask"
		expect_string=$(remove_space "$expect_string")
		echo $expect_string
        if [ "$expect_string" = "$cmd_result" ] ; then
			echo_success "Check maintenance dedicated port IPv4 submask success"
			echo "Check maintenance dedicated port IPv4 submask success" >>$SAVEFILE
			return 0
		else
			echo_fail "Check maintenance dedicated port IPv4 submask fail"
			echo "Check maintenance dedicated port IPv4 submask fail" >> $SAVEFILE
			error_quit
        fi
   	fi
}

main()
{
	chmod +x *
	#4.1、校验主机名称
	getini $CONFIGFILE BMCSet_Hostname
	HOSTNAME=$parameter
	if [ "$HOSTNAME" = "on" ] ;then
		getini $CONFIGFILE Custom_SerialNUM
		#用Custom_SerialNUM作为主机名定制参数，会导致高密度服务器主机名缺少槽位号后缀
		HOSTNAME=$parameter
		if [ "$HOSTNAME" != "" ] ;then
			check_set_host_name
			sleep 1
		else
			echo_fail "HOSTNAME=NULL verify HOSTNAME fail" 
			echo "HOSTNAME=NULL verify HOSTNAME fail" >>$SAVEFILE
			error_quit
		fi
		sleep 1
	else
		check_restore_host_name
		sleep 1
	fi

	####网络配置####################################################################
	#4.2、校验Eth0 IP 或 IP模式
	getini  $CONFIGFILE  BMCSet_ETH0_DHCPMode
	DHCP=$parameter
	if [ "$DHCP" = "on" ] ; then
		check_set_eth2_dhcp_mode
		sleep 1	
	else
		getini  $CONFIGFILE  BMCSet_Eth0_Ipv4
		IPADDR=$parameter
		if [ "$IPADDR" = "on" ] ; then
			grep "Custom_Eth0_Ip_Addr" $CONFIGFILE | grep "." >ip.txt
			if [ $? != 0 ] ; then
				echo_fail "IPADDR=NULL verify ipaddr fail" 
				echo "IPADDR=NULL verify ipaddr fail" >>$SAVEFILE
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
				check_set_eth2_ipaddr
				sleep 1				
				check_set_eth2_ipmask
				sleep 1
				check_ipv4_gateway
				sleep 1
			fi
		else
			getini  $CONFIGFILE  BMCSet_NCSI_Enable
			if [ "$parameter" = "on" ] ; then
				getini  $CONFIGFILE  BMCSet_NCSI_DHCPMode
				DHCP=$parameter
				#是否开启DHCP
				if [ "$DHCP" = "on" ] ; then
					check_set_eth2_dhcp_mode
					sleep 1
				fi
			else
				if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
					check_restore_ipaddr
					sleep 1
					IPMASK=24
					Eth2_IPMASK=$IPMASK
					check_set_eth2_ipmask
					sleep 1
					check_default_gateway
					sleep 1
				else
					check_set_eth2_dhcp_mode
					sleep 1				
				fi	
			fi		
		fi	
	fi
	
	#校验默认backup ip
	check_default_backup_ip

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
	
	#4.3校验NCSI使能配置
	if [ $PRODUCT_ID != $PRODUCT_OSCA_TYPE ] && [ $PRODUCT_ID != $PRODUCT_ATLAS_TYPE ] && [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ]; then
		check_NET_Mode	
		check_Set_NCSI_vlan_id		
	fi
	
	# G2500是Atlas的ProductId，但是其支持NCSI
	if [ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID = $BOARD_G2500_ID ]; then
		check_NET_Mode	
		check_Set_NCSI_vlan_id		
	fi
		
	####DNS配置#####################################################################
	#4.4、DDNS配置.校验DDNS
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then
	getini $CONFIGFILE  BMCSet_DDSN_AutoIpMode
	DDSN_AutoIpMode=$parameter
	
	if [ "$DDSN_AutoIpMode" = "1" ] ; then
		check_ddns_ipmode 1
	else
		check_ddns_ipmode 0
	fi
		
	getini $CONFIGFILE  BMCSet_DDSN
	DDNS=$parameter
	if [ "$DDNS" = "on" ] ; then
		check_set_ddns_on
	else
		check_set_ddns_off   
		check_set_ddns_domain_name
		check_set_ddns_primary_domain
		check_set_ddns_backup_domain
		check_set_ddns_tertiary_domain
	fi
	sleep 1
	fi
	####服务启停配置################################################################
	#8、校验服务启停配置  
	if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ] ; then	
	check_set_service_configuration
	
	#校验 SNMP服务启用状态
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

	check_snmp_mib_version 4 $V1State
	check_snmp_mib_version 5 $V2State
	echo_success "verify MIBVersion $string success"
	echo "verify MIBVersion $string success">>$SAVEFILE
	
	
	####SSDP服务功能配置校验########################################################################
	#4.11 SSDP服务功能配置校验
	getini $CONFIGFILE BMCSet_SSDPEnable
	SSDPEnable=$parameter
	if [ "$SSDPEnable" == "" ] ; then
		SSDPEnable="0";
	fi

    get_prop_cmd 57 "03 00 00 04 0D 00 00 00" 01 "01 00 79 01 0$SSDPEnable"
    if [ $? != 0 ] ; then
        echo_fail "verify SSDP State:$SSDPEnable fail!"
        echo "verify SSDP State:$SSDPEnable fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "verify SSDP State:$SSDPEnable success!"
        echo "verify SSDP State:$SSDPEnable success!" >>$SAVEFILE
    fi
	#SSDP服务NOTIFY时间间隔校验
	getini $CONFIGFILE BMCSet_SSDP_NOTIFY_INTERVAL
	NotifyInterval=$parameter
	if [ "$NotifyInterval" == "0" ] ; then
		echo_fail "verify SSDP Notify Interval:$NotifyInterval fail!"
        echo "verify SSDP Notify Interval:$NotifyInterval fail!" >>$SAVEFILE
        error_quit
	fi
	if [ "$NotifyInterval" == "" ] ; then
		NotifyInterval=600;
	fi

	NotifyInterval=$(dword2string $NotifyInterval)
    get_prop_cmd 77 "ff ff 00 01 00" 04 "04 00 75 04 $NotifyInterval"
    if [ $? != 0 ] ; then
        echo_fail "verify SSDP Notify Interval:$NotifyInterval fail!"
        echo "verify SSDP Notify Interval:$NotifyInterval fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "verify SSDP Notify Interval:$NotifyInterval success!"
        echo "verify SSDP Notify Interval:$NotifyInterval success!" >>$SAVEFILE
    fi
	
	#SSDP服务模式校验
	getini $CONFIGFILE BMCSet_SSDP_MODE
	SSDPMode=$parameter
	if [ "$SSDPMode" == "0" ] ; then
		echo_fail "verify SSDP mode:$SSDPMode fail!"
        echo "verify SSDP mode:$SSDPMode fail!" >>$SAVEFILE
        error_quit
	fi
	if [ "$SSDPMode" == "" ] ; then
		if [ "$SSDPEnable" == "1" ] ; then
			SSDPMode=3;
		else
			SSDPMode=2;
		fi
	fi

    get_prop_cmd 77 "ff ff 00 01 00" 01 "01 00 79 01 0$SSDPMode"
    if [ $? != 0 ] ; then
        echo_fail "verify SSDP mode:$SSDPMode fail!"
        echo "verify SSDP mode:$SSDPMode fail!" >>$SAVEFILE
        error_quit   
    else 
        echo_success "verify SSDP mode:$SSDPMode success!"
        echo "verify SSDP mode:$SSDPMode success!" >>$SAVEFILE
    fi

	####SNMP OID 检验#######################################################################
	#null: 默认值：
	getini $CONFIGFILE Custom_MIBOID
	MIBOID=$parameter
	if [ "$MIBVersion" == "" ] ;then
		check_snmp_mib_oid ""
	else
		check_snmp_mib_oid $MIBOID
	fi
	
	####SNMP 引擎ID更新功能检查#######################################################################
	check_enable_snmp_engineID_update
	fi
	####NTP 检验#######################################################################
	#检验NTP使能
	getini $CONFIGFILE BMCSet_NTPEnableStatus
	NTPEnableStatus=$parameter
	check_ntp_enable_status $NTPEnableStatus
	
	#检验NTP主服务器地址
	getini $CONFIGFILE BMCSet_NTPPreferredServerAddr
	NTPPreferredServerAddr=$parameter
	check_ntp_preferred_server $NTPPreferredServerAddr
	
	#检验NTP备用的服务器地址 
	getini $CONFIGFILE BMCSet_NTPAlternativeServerAddr
	NTPAlternativeServerAddr=$parameter
	check_ntp_alternative_server $NTPAlternativeServerAddr
	
	#检验NTP拓展的服务器地址
	getini $CONFIGFILE BMCSet_NTPExtraServerAddr
	NTPExtraServerAddr=$parameter
	check_ntp_extra_server $NTPExtraServerAddr
	
	#检验NTP个数
	getini $CONFIGFILE BMCSet_NTPCount
	NTPCount=$parameter
	if [ "$NTPCount" == "" ] ; then
		NTPCount=3;
	fi
	check_ntp_count $NTPCount
	
	#检验NTP模式
	getini $CONFIGFILE BMCSet_NTPMode
	NTPMode=$parameter
	if [ "$NTPMode" == "" ] ; then
		NTPMode=1;
	fi
	check_ntp_mode $NTPMode
	
    #检验NTP认证使能
	getini $CONFIGFILE BMCSet_AuthNTPServer
	AuthNTPServer=$parameter
	check_ntp_auth_enable $AuthNTPServer
    
	#校验Ipv6模式
    if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ];then
	getini $CONFIGFILE BMCSet_Ipv6_DHCPMode
	IPV6_DHCP=$parameter	
	if [ "$IPV6_DHCP" != "" ] ; then
		if [ "$IPV6_DHCP" == "on" ] ; then
			check_ipv6_dhcp_mode
			sleep 1
		elif [ "$IPV6_DHCP" == "off" ] ; then
			check_support_ipv6_mode
			if [ $? == 1 ] ; then
				echo_fail "Check ipv6 addr default value fail!"
				echo "Check ipv6 addr default value fail!" >>$SAVEFILE
				sleep 1
			else
				check_ipv6_static_mode
				sleep 1
				check_ipv6_default_addr
				sleep 1
				check_ipv6_default_prefix
				sleep 1
			fi
		else
			echo_fail "Check ipv6 DHCP parameter error!"
			echo "Check ipv6 DHCP parameter error!" >>$SAVEFILE
		fi
	else
		#没有配置dhcpv6，且不支持ipv6，返回成功
		check_support_ipv6_mode
		if [ $? == 1 ] ; then
			sleep 1
		else
			check_ipv6_static_mode
			sleep 1
			check_ipv6_default_addr
			sleep 1
			check_ipv6_default_prefix
			sleep 1
		fi	
	fi
	
	#检查LLDP使能开关
	getini $CONFIGFILE BMCSet_LLDPEnable
	LLDPEnable=$parameter
	if [ "$LLDPEnable" != "" ] ; then
		if [ "$LLDPEnable" = "on" ] ; then
			check_LLDP_enable 1
		else
			check_LLDP_enable 0
		fi 
	else
		check_LLDP_enable 0
	fi
	fi
	####校验NCSI通道RX使能########################################################################
    if [ $PRODUCT_ID != $PRODUCT_TCE_TYPE ];then
	check_ncsi_rx_channel_configuration
    fi
	get_irm_heartbeat_enable
	heartbeat_enable_state=$?
	if [ "$heartbeat_enable_state" = 1 ]; then
		#校验专用网口近端运维IPv4地址
		getini $CONFIGFILE BMCSet_Maint_IPv4
		MaintIPv4Address=$parameter
		if [ "$MaintIPv4Address" != "" ] ; then
			check_maint_ipv4_address $MaintIPv4Address
		else
			check_maint_ipv4_address "192.168.240.100"
		fi
		
		#校验专用网口近端运维子网掩码
		getini $CONFIGFILE BMCSet_Maint_SubnetMask
		MaintSubmask=$parameter
		if [ "$MaintSubmask" != "" ] ; then
			check_maint_ipv4_subnetmask $MaintSubmask
		else
			check_maint_ipv4_subnetmask "255.255.255.0"
		fi

		check_rack_inner_subnet
		check_set_rack_inner_vlan_id
	fi
}

main
