#!/bin/sh

if [ $1 == "1" ];
then 
# 修改iptable的input表
	
	# 判断参数个数有效性,奇数个数为有效；否则为无效，退出
	arg_cnt_mod=$(( $# % 2 ))
	if [ $arg_cnt_mod != 1 ];
	then
		echo "iptables filter tables arg error"
		exit
	fi
	
	#删除filter中INPUT链配置
	input_cnt=$(iptables-save  | grep  ^-A | grep -i INPUT | wc -l)
	
	let rule_to_del=1
	SSDP_IP="239.255.255.250|FF02::C|FF05::C|FF08::C"
	for i in `seq 1 $input_cnt`;
	do
		filter_cnt=$(iptables -t filter -nL INPUT $rule_to_del -w 2>/dev/null|grep -iE $SSDP_IP|wc -l)
		if [ $filter_cnt == 1 ];
		then
			let rule_to_del+=1
			continue
		fi
		
		iptables -t filter -D INPUT $rule_to_del -w 1>/dev/null 2>&1 ;  
	done                         

	#删除filter中OUTPUT链配置
	output_cnt=$(iptables-save  | grep  ^-A | grep -i OUTPUT | wc -l)
	
	for i in `seq 1 $output_cnt`;
	do
		iptables -t filter -D OUTPUT 1 -w 1>/dev/null 2>&1 ;  
	done                         	
	
	until [ -z "$2" ] # Until all parameters used up
	do
		# 使用-m limit参数时，需要xt_limit.ko驱动
		xt_limit_exist_flag=$(lsmod | grep "xt_limit"|wc -l)
		if [ $xt_limit_exist_flag == 0 ];then
			KERNEL_NAME=`uname -r`
			insmod /lib/modules/${KERNEL_NAME}/kernel/net/netfilter/xt_limit.ko
		fi
		
		# ibmc上看下这个配置； iptables-save   $2:eth1,eth2,eth3
		iptables -t filter -A INPUT  -i $2 -d $3 -p tcp --syn -m limit --limit 100/s --limit-burst 100 -j ACCEPT -w 1>/dev/null 2>&1
		iptables -t filter -A INPUT  -i $2 -d $3 -p tcp --syn -j DROP -w 1>/dev/null 2>&1
		
		# 只接收目的ip是本接口ip的报文,非本接口ip的报文丢弃
		iptables  -t filter -A INPUT -i $2 -d $3 -j  ACCEPT -w 1>/dev/null 2>&1
		if [ -z "$4" -o "$4" != "$2" ]
		then
			iptables -t filter -A INPUT -i $2 -p icmp --icmp-type time-exceeded -j DROP -w 1>/dev/null 2>&1
			iptables -t filter -A INPUT -i $2 -p icmp --icmp-type timestamp-request -j DROP	-w 1>/dev/null 2>&1
			iptables -t filter -A INPUT -i $2 -j  DROP -w 1>/dev/null 2>&1
		fi
		# 把入参向左移动2个
		shift 2
	done

	iptables -t filter -A OUTPUT -p icmp --icmp-type time-exceeded -j DROP -w 1>/dev/null 2>&1
	iptables -t filter -A OUTPUT -p icmp --icmp-type timestamp-reply -j DROP -w 1>/dev/null 2>&1
elif [ $1 == "3" ];
then
# 修改ip6tables的input表
	
	# 判断参数个数有效性,奇数个数为有效；否则为无效，退出
	arg_cnt_mod=$(( $# % 2 ))
	if [ $arg_cnt_mod != 1 ];
	then
		echo "ip6tables filter tables arg error"
		exit
	fi
	
	#删除ip6tables的filter表中的除SSDP功能特殊IP以外的Input链配置
	input_cnt=$(ip6tables-save  | grep  ^-A | grep -i INPUT | wc -l)
	
	let rule_to_del=1
	SSDP_IP="FF02::C|FF05::C|FF08::C"
	for i in `seq 1 $input_cnt`;
	do
		filter_cnt=$(ip6tables -t filter -nL INPUT $rule_to_del -w 2>/dev/null |grep -iE $SSDP_IP|wc -l)
		if [ $filter_cnt == 1 ];
		then
			let rule_to_del+=1
			continue
		fi
		
		ip6tables -t filter -D INPUT $rule_to_del -w 1>/dev/null 2>&1 ;  
	done
	
	#删除ip6tables的filter中的OUTPUT链配置
	output_cnt=$(ip6tables-save  | grep  ^-A | grep -i OUTPUT | wc -l)
	
	for i in `seq 1 $output_cnt`;
	do
		ip6tables -t filter -D OUTPUT 1 -w 1>/dev/null 2>&1 ;  
	done 	
	
	until [ -z "$2" ] # Until all parameters used up
	do
	
		# ibmc上看下这个配置； ip6tables-save   $2:eth1,eth2,eth3
		eth_config=`ip6tables-save |grep -i "limit-burst" |sed -r "s/.*-i (.*) -p.*/\1/" |grep "$2"`
		if [ -z "$eth_config" ];
		then
			ip6tables -t filter -A INPUT -i $2 -p tcp --syn -m limit --limit 100/s --limit-burst 100 -j ACCEPT -w 1>/dev/null 2>&1
			ip6tables -t filter -A INPUT -i $2 -p icmpv6 -m limit --limit 100/s --limit-burst 100 -j ACCEPT -w 1>/dev/null 2>&1
			ip6tables -t filter -A INPUT -i $2 -p tcp --syn -j DROP -w 1>/dev/null 2>&1
		fi
	
		ip6tables -t filter -A INPUT -i $2 -p icmpv6 --icmpv6-type time-exceeded -j DROP -w 1>/dev/null 2>&1
		
		# 只接收目的ip是本接口ip的报文,非本接口ip的报文丢弃
		# ibmc上看下这个配置； ip6tables-save   $3:ipaddr/prefix
		ip6tables -t filter -A INPUT -i $2 -d $3 -j ACCEPT -w 1>/dev/null 2>&1
		if [ -z "$4" -o "$4" != "$2" ]
		then
			ip6tables -t filter -A INPUT -i $2 -j DROP -w 1>/dev/null 2>&1
		fi
		# 把入参向左移动2个
		shift 2
	done
	
	ip6tables -t filter -A OUTPUT -p icmpv6 --icmpv6-type time-exceeded -j DROP	-w 1>/dev/null 2>&1
else
	
	# 判断参数个数有效性,9个参数为有效，否则为无效，退出
	if [ $# != 9 ];
	then
		echo "iptables nat tables arg error"
		exit
	fi	
	
	# 删除nat表
	iptables -F -t nat -w
	iptables -X -t nat -w
	iptables -Z -t nat -w
	
	# 删除filter表Forward链，统计FORWARD表项个数
	forward_cnt=$(iptables-save  | grep  ^-A | grep -i FORWARD | wc -l)
	for i in `seq 1 $forward_cnt`;
	do
		iptables -t filter -D FORWARD 1 -w 1>/dev/null 2>&1
	done

	if [ $2 == "1" ];
	then
		# 使能转发功能
		echo  1 > /proc/sys/net/ipv4/ip_forward
		# eth是以逗号分隔的
		eth_string=`echo $3 | tr , " "`
		for eth in $eth_string
		do
			iptables -t nat -A PREROUTING  -i $eth -p $7 --dst $4 --dport $8 -j DNAT --to-destination "$5:$9" -w
			# 从eth上收到非eth ip的报文，丢弃
			iptables -t nat -A PREROUTING  -i $eth --dst  $4  -j DNAT --to-destination $4 -w
			iptables -t nat -A PREROUTING  -i $eth -j DNAT --to-destination  127.0.0.1 -w
		done
		iptables -t nat -A POSTROUTING -p $7 --dst $5 --dport $9 -j SNAT --to-source $6 -w
		iptables -t filter -A FORWARD -p $7 --dport $9 -j ACCEPT -w 1>/dev/null 2>&1
		iptables -t filter -A FORWARD -p $7 --sport $9 -j ACCEPT -w 1>/dev/null 2>&1
	else
		# 去使能转发功能
		echo  0 > /proc/sys/net/ipv4/ip_forward
	fi
# 修改iptable的forward表
fi

	

