#避免重复加载common脚本
if [ "$MODULE_COMMON_LOADED" != "" ]; then
	return
fi
AdminUserName="Administrator"
AdminPassword="Admin@9000"
MODULE_COMMON_LOADED=1
#命令失败时的重试次数 和间隔 
REPEAT=6
REPEAT_TIME=1 
UPGRADE_REPEAT=3
#日志文件
IPMI_CMD_LOG_FILE=custom.log
IPMI_CMD_RETURN_FILE=ipmi_cmd_return_string.txt
BP_PUID_FILE=bp_product_unique_id.txt
BP_FLAG_FILE=bp_flag.txt
SAVEFILE=$project_code"_save.txt"
#产品版本，默认是V3
PRODUCT_VERSION_NUM=03
#产品类型
PRODUCT_ID=0
BOARD_ID=0
PRODUCT_UNIQUE_ID=0
GPU_BOARD_ID=0
BLADNUM=0
PRODUCT_OSCA_TYPE=4
PRODUCT_RACK_TYPE=1
PRODUCT_RACK_TYPE_AJ_CUSTOMIZE=40
PRODUCT_8100_TYPE=8
PRODUCT_8100_TYPE_AJ_CUSTOMIZE=41
PRODUCT_XSERIALS_TYPE=3
PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE=43
PRODUCT_ATLAS_TYPE=2
PRODUCT_TCE_TYPE=6
PRODUCT_KUNPENG_TYPE=10
PRODUCT_TAISHAN_1711_TYPE=255
#2280v2是TaiShan产品，但不是7是1
PRODUCT_TAISHAN_TYPE=7
PRODUCT_TIANCHI_TYPE=0
BOARD_6800_ID=0d
BOARD_XH628_XH622_ID=0d
BOARD_G2500_ID=91
#天池产品
BOARD_TIANCHI_ID=255
#XH321有两个BoardID（高、低规格）
BOARD_XH321_ID_HIGH=a0
BOARD_XH321_ID_STANDARD=a3
BOARD_XH321V5_ID=0c
BOARD_XH321V5L_ID=0b
BOARD_XH628V5_ID=09
BOARD_XH620_ID=0e
BOARD_G560_ID=50
BOARD_G530V5_ID=3c
BOARD_G560V5_ID=51
BOARD_ATLAS880_ID=92
BOARD_ATLAS800_9010_ID=52
BOARD_ATLAS500_ID=94
PRODUCT_XH628_XH622_ID=30d
PRODUCT_XH620_ID=30e
PRODUCT_XH321_HIGH_ID=3a0
PRODUCT_XH321_STANDARD=3a3
PRODUCT_XH321V5_ID=30c
PRODUCT_XH628V5_ID=309
PRODUCT_XH321V5L_ID=30b
PRODUCT_G560_ID=250
PRODUCT_G530V5=23c
PRODUCT_G560V5_ID=251
#xa320有6款a6\b0\b1\b2\b3\b4\b5
PRODUCT_XA320_A6_ID=3a6
PRODUCT_XA320_B0_ID=3b0
PRODUCT_XA320_B1_ID=3b1
PRODUCT_XA320_B2_ID=3b2
PRODUCT_XA320_B3_ID=3b3
PRODUCT_XA320_B4_ID=3b4
PRODUCT_XA320_B5_ID=3b5
PRODUCT_TAISHAN2280V2_ID=1b9
PRODUCT_TAISHAN2280V2_1711_ID=7b9
PRODUCT_TIANCHI_ID=0ff
PRODUCT_XH321V6_ID=3b5
PRODUCT_XH321CV6_ID=3b6
PRODUCT_D320V2_ID=43b5
PRODUCT_D320CV2_ID=43b6
GPU_BOARD_GP316_ID=a1
GPU_BOARD_GP308_ID=b4
GPU_BOARD_GP608_ID=b0
GPU_BOARD_GS608_ID=b3

#ProductUniqueID
PRODUCT_DA122C_UNIQUEID=0x02070d01

#配置文件存放地方
CONFIGFILE=config.ini
#是否支持的功能
LCD_SUPPORT_STATE=0
ACTIVE_STANDBY_SUPPORT_STATE=0
#最大字符串长度
MAX_STRING_NUM=24
MAX_FUN_STRING_LEN=20
#获取到的属性值
GET_PROP_INF_STRING=""
G_WORK_DIR=`pwd`
#添加临时用户会用
PASSWORD_WEAK_STATE=0
PASSWORD_WEAK_SUPPORT=0
PASSWORD_COMPLEXITY_STATE=0
PRODUCT_VERSION=03
#读取的最大FRU ID个数
MAX_FRUID_COUNT=128
#FRU ID集合
FRUID_SET=

SCP_MAX_RETRY_TIME=10
SCP_RETRY_INTERVAL=10

#是否支持装备分离：0表示不支持装备隔离，1表示支持装备隔离
EQUIPMENT_SEPARATION_SUPPORT_STATE=0

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

echo_and_save_fail()
{
    echo -e "\033[31;1m$1\033[0m" | tee -a $SAVEFILE
}

echo_success()
{
	echo -e "\033[32;1m$1\033[0m"
}

echo_and_save_success()
{
    echo -e "\033[32;1m$1\033[0m" | tee -a $SAVEFILE
}

#参数为 输入文件  开始字节  输出文件
hextochar()
{
    temp=`cat  $1`
    temp=`echo $temp | cut -b $2- `

    converhex="$temp"

    #清空文件
    >$3

    for k in $converhex
    do
       TEMP="\\x$k"
       echo -n -e $TEMP >>$3      
    done
}

check_cmd_byte_valid()
{
	arr=${1}
	for i in $arr; do
		dec_data=`printf %d 0x$i`
		if [ $dec_data -gt 255 ]; then
			echo "check run cmd failed, invalid byte $i"
			echo "check run cmd failed, invalid byte $i" >>$SAVEFILE
			return 1
		fi
	done

	return 0
}

#使用ipmitool命令执行函数  参数为：命令  正确返回值  保存的文件（$IPMI_CMD_RETURN_FILE）
run_ipmitool()
{
    local i
    echo "ipmitool $1 : $2" >>$IPMI_CMD_LOG_FILE
    for((i=0;i<$REPEAT;i++))
    do
        ipmitool $1 > $IPMI_CMD_RETURN_FILE
        #ipmitool 返回值超过16字节时换行，合并为一行
        echo `cat $IPMI_CMD_RETURN_FILE | sed 's/^//g'` > $IPMI_CMD_RETURN_FILE
        grep -wq "$2" $IPMI_CMD_RETURN_FILE
        if [ $? = 0 ] ; then
            cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
            return 0
        fi
        sleep $REPEAT_TIME
    done

    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
    return 1
}

#使用ipmitool命令执行函数，针对执行ipmitool回显为空场景，参数为：命令 保存的文件（$IPMI_CMD_LOG_FILE)
run_ipmitool_without_echo()
{
    local i
    echo "ipmitool $1" >>$IPMI_CMD_LOG_FILE
    for((i=0;i<$REPEAT;i++))
    do
        ipmitool $1
        if [ $? = 0 ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    return 1
}

#命令执行函数  参数为：命令  正确返回值  保存的文件（$IPMI_CMD_RETURN_FILE）
run_cmd()
{
    local i

	check_cmd_byte_valid "${1}"
	if [ $? != 0 ]; then
		return 1
	fi

    echo "ipmicmd -k \"$1\" smi 0 : $2" >>$IPMI_CMD_LOG_FILE
    for((i=0;i<$REPEAT;i++))
    do
        ipmicmd -k "$1" smi 0 > $IPMI_CMD_RETURN_FILE
        grep -wq "$2" $IPMI_CMD_RETURN_FILE
        if [ $? = 0 ] ; then
		    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
            return 0
        fi
        sleep $REPEAT_TIME
    done
	
    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
    return 1
}


#获取get属性万能命令返回值函数  参数为：fun_id,key_id_string,info_id,获取到的返回值存放在g_get_prop_cmd_rsp全局变量中 
g_get_prop_cmd_rsp=0
get_prop_cmd_rsp()
{	
	g_get_prop_cmd_rsp=0
	local RETRY_COUNT=10
	local cmd_str=0	
	local i=0
	local fun_id=$(word2string $1)
	local key_id_string=$2
	local info_id=$(word2string $3)
	local max_len=`printf "%02x" $MAX_FUN_STRING_LEN`
	local end_flag=0
	local offset=0
	local msg_flag=0
	local msg_tol_len=0
	local GET_PROP_INF_STRING=""
	
	for((i=0;i<$RETRY_COUNT;i++))
	do
		#每次重试刷新变量		
		end_flag=0
		offset=0
		msg_flag=0
		msg_tol_len=0
		GET_PROP_INF_STRING=""
		
		while true
		do
			offset_s=$(word2string $offset)
			cmd_str="f 0 30 93 db 07 00 36 $fun_id 1 $max_len $offset_s $key_id_string $info_id"
			ipmicmd -k "$cmd_str" smi 0 > $IPMI_CMD_RETURN_FILE
			cat $IPMI_CMD_RETURN_FILE
			tmp_arr=(`cat $IPMI_CMD_RETURN_FILE`)
			tmp_array=`cat $IPMI_CMD_RETURN_FILE`
			#是否为最后一帧的结束标志在byte4
			cmd_result_tmp=${tmp_arr[4]}
			if [ "$cmd_result_tmp" != "00" ] ; then
				#返回失败延时1s重试
				sleep 1
				break
			fi
			
			offset=`expr $MAX_FUN_STRING_LEN + $offset`
			#是否为最后一帧的结束标志在byte8
			end_flag=${tmp_arr[8]}
			
			#此次返回有效属性数据长度保存在byte12，先要把长度值转化为十进制，再累加
			tmp_len=${tmp_arr[12]}
			((tmp_len=0x$tmp_len))
			((msg_tol_len=0x$msg_tol_len))	
			msg_tol_len=`expr $tmp_len + $msg_tol_len`
			msg_tol_len=`printf %02x $msg_tol_len`
			
			#对于一个属性需要多帧获取，每一帧都会带有prop_info数据，只需要第一帧保存起来
			if [ $msg_flag -eq 0 ] ; then
				#属性id，type在返回命令的byte9~byte11
				prop_info=$(get_sub_array 9 11 "$tmp_array") 
				msg_flag=1
			fi
			#每一帧返回的属性数据保存在byte13开始的位置，截取属性值并累加至上次的后面
			GET_PROP_INF_STRING=$GET_PROP_INF_STRING" ${tmp_arr[@]:13}"
			echo "GET_PROP_INF_STRING is $GET_PROP_INF_STRING" #todo
			if [ "$end_flag" = "01" ] ; then
				#sleep 1
				continue
			else					
				break
			fi
		done
		#获取所有的数据帧之后，去掉空格后，与prop_info， length等信息拼接后再把多余的前后空格去掉
		GET_PROP_INF_STRING=$(remove_space "$GET_PROP_INF_STRING")
		GET_PROP_INF_STRING="$prop_info $msg_tol_len $GET_PROP_INF_STRING"
		GET_PROP_INF_STRING=$(remove_space "$GET_PROP_INF_STRING")
		
		if [ "$GET_PROP_INF_STRING" != "" ] ; then
			break
		fi
	done
	
	if [ $i -eq $RETRY_COUNT ]; then
	    echo "retry $RETRY_COUNT to get_prop_cmd_rsp."
		return 1
	fi
	g_get_prop_cmd_rsp=$GET_PROP_INF_STRING
	return 0
}



get_product_version_num()
{
	ipmicmd -k "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 07 00" smi 0 > product_version_num.txt
	if [ $? == 0 ] ; then
		PRODUCT_VERSION_NUM=`cat product_version_num.txt | awk '{print $NF}'`
	fi
	
	echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM"
	echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM" >>$SAVEFILE
	rm -f product_version_num.txt
}

#获取product_id(需要安装信息命令封装)
# 刀片类:PRODUCT_ID=4
# 机架类:PRODUCT_ID=1
# 8100  :PRODUCT_ID=8
# X系列 :PRODUCT_ID=3
# Atlas :PRODUCT_ID=2
# TCE类 :PRODUCT_ID=6
# 天池  :PRODUCT_ID=0
get_product_id()
{
	PRODUCT_ID=0
    run_cmd  "f 0 30 90 29 00 01"  "0f 31 00 90 00"
    grep -wq "0f 31 00 90 00 04" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
	    PRODUCT_ID=$PRODUCT_OSCA_TYPE
	    echo_success "PRODUCT_TYPE: CH"
	    echo "PRODUCT_TYPE: CH get_product_id:$PRODUCT_ID" >>$SAVEFILE
    fi
    
    grep -wq "0f 31 00 90 00 01" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
    	#机架类服务器（2285）
	    PRODUCT_ID=$PRODUCT_RACK_TYPE
	    echo_success "PRODUCT_TYPE: RH"
	    echo "PRODUCT_TYPE: RH get_product_id:$PRODUCT_ID" >>$SAVEFILE
    fi

	grep -wq "0f 31 00 90 00 40" $IPMI_CMD_RETURN_FILE
	if [ $? = 0 ] ; then
		#机架类服务器（2285） (AJ custom brand)
		PRODUCT_ID=$PRODUCT_RACK_TYPE_AJ_CUSTOMIZE
		echo_success "PRODUCT_TYPE: RH"
		echo "PRODUCT_TYPE: RH get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi

    grep -wq "0f 31 00 90 00 08" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
    	#高性能类服务器（8100）
	    PRODUCT_ID=$PRODUCT_8100_TYPE
		echo_success "PRODUCT_TYPE: XH/DH"
		echo "PRODUCT_TYPE: XH/DH get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi

	grep -wq "0f 31 00 90 00 41" $IPMI_CMD_RETURN_FILE
	if [ $? = 0 ] ; then
		#高性能类服务器（8100） (AJ custom brand)
		PRODUCT_ID=$PRODUCT_8100_TYPE_AJ_CUSTOMIZE
		echo_success "PRODUCT_TYPE: XH/DH"
		echo "PRODUCT_TYPE: XH/DH get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi

	grep -wq "0f 31 00 90 00 03" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
	    PRODUCT_ID=$PRODUCT_XSERIALS_TYPE
		echo_success "PRODUCT_TYPE: X-Serials"
		echo "PRODUCT_TYPE:X-Serials get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi

    grep -wq "0f 31 00 90 00 43" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
        #高密类服务器（X-Serials） (AJ custom brand)
        PRODUCT_ID=$PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE
        echo_success "PRODUCT_TYPE: DH"
        echo "PRODUCT_TYPE: DH get_product_id:$PRODUCT_ID" >>$SAVEFILE
    fi

	grep -wq "0f 31 00 90 00 02" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
	    PRODUCT_ID=$PRODUCT_ATLAS_TYPE
		echo_success "PRODUCT_TYPE: Atlas-Serials"
		echo "PRODUCT_TYPE:Atlas-Serials get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi	
	

	grep -wq "0f 31 00 90 00 0a" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
    	#鲲鹏单板
	    PRODUCT_ID=$PRODUCT_KUNPENG_TYPE
	    echo_success "PRODUCT_TYPE: Kunpeng_Board-Serials"
	    echo "PRODUCT_TYPE: Kunpeng_Board-Serials get_product_id:$PRODUCT_ID" >>$SAVEFILE
    fi
	grep -wq "0f 31 00 90 00 ff" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
    	#TaiShan2280v2_1711
	    PRODUCT_ID=$PRODUCT_TAISHAN_1711_TYPE
	    echo_success "PRODUCT_TYPE: TaiShan2280v2_1711_Board-Serials"
	    echo "PRODUCT_TYPE: TaiShan2280v2_1711_Board-Serials get_product_id:$PRODUCT_ID" >>$SAVEFILE
    fi
	grep -wq "0f 31 00 90 00 06" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
	    PRODUCT_ID=$PRODUCT_TCE_TYPE
		echo_success "PRODUCT_TYPE: TCE"
		echo "PRODUCT_TYPE:TCE get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi	
	grep -wq "0f 31 00 90 00 07" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
		#TaiShan类服务器（2480v2）
	    PRODUCT_ID=$PRODUCT_TAISHAN_TYPE
		echo_success "PRODUCT_TYPE: TaiShan-Serials"
		echo "PRODUCT_TYPE:TaiShan-Serials get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi	

	grep -wq "0f 31 00 90 00 00" $IPMI_CMD_RETURN_FILE
    if [ $? = 0 ] ; then
		#TianChi类服务器
	    PRODUCT_ID=$PRODUCT_TIANCHI_TYPE
		echo_success "PRODUCT_TYPE: TianChi-Serials"
		echo "PRODUCT_TYPE:TianChi-Serials get_product_id:$PRODUCT_ID" >>$SAVEFILE
	fi

#if [ $PRODUCT_ID = 0 ] ; then
#		echo_fail "get_product_id fail"
#		echo "get_product_id fail" >>$SAVEFILE
#		error_quit
#	fi
}

#合并配置文件
merge_config_file()
{
	dos2unix *.ini
	cat $1 >$CONFIGFILE
	echo "" >>$CONFIGFILE
	cat $2 >>$CONFIGFILE

	sort $CONFIGFILE | uniq >bak
	sort bak | uniq >$CONFIGFILE
}

#打开DFT接口
DFT_enable()
{
    run_cmd  "f 0 30 90 20 db 07 00 01"  "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo_fail "enable DFT fail!"
        echo "enable DFT fail!" >>$SAVEFILE
        error_quit
    else 
        echo_success "enable DFT success!"
        return 0
    fi
}

#设置定制化模式
customize_enable()
{
    run_cmd  "f 0 30 90 20 db 07 00 01"  "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo_fail "enable customize fail!"
        echo "enable customize fail!" >>$SAVEFILE
        error_quit
    else 
        echo_success "enable customize success!"
        return 0
    fi
}

#获取boardid
get_board_id()
{	
	run_cmd  "f 00 6 1" "0f 07 00 01 00"
	if [ $? != 0 ] ; then
		echo_fail "get_board_id fail!"
        echo "get_board_id fail!" >> $SAVEFILE
        return 1
	else
		tmp_ret=`cat $IPMI_CMD_RETURN_FILE`
		BOARD_ID=${tmp_ret: -12 : 2}
        echo_success "get_board_id:$BOARD_ID"
        echo "get_board_id:$BOARD_ID" >> $SAVEFILE
    fi
}

#获取GPU载板boardid DTS2018060707573 【G530v5】【GP308】G530V5配GP308载板，生产空定制化脚本失败
get_gpu_board_id()
{	
	i2c_bus_no=$1
	run_cmd  "f 0 30 92 db 07 00 22 03 $i2c_bus_no 84 00 44 00 01" "0f 31 00 92 00"
	if [ $? != 0 ] ; then
		echo_fail "get_gpu_board_id fail!"
		echo "get_gpu_board_id fail!" >> $SAVEFILE
		return 1
	else
		tmp_ret=`cat $IPMI_CMD_RETURN_FILE`
		GPU_BOARD_ID=${tmp_ret: -3 : 2}
		echo_success "get_gpu_board_id:$GPU_BOARD_ID"
		echo "get_gpu_board_id:$GPU_BOARD_ID" >> $SAVEFILE
	fi
}

#获取CoreRemObjTem默认值
get_CpuCoreRemObjTem()
{
	run_cmd  "f 00 30 93 db 07 00 36 4f 00 01 ff 00 00 ff ff 00 01 00 05 00" "db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "get_CpuTjmax fail!"
		echo "get_CpuTjmax fail!" >> $SAVEFILE
		error_quit
		
	else
		tmp_ret=`cat $IPMI_CMD_RETURN_FILE`
                CpuTjmax=${tmp_ret:39:2}
                let CpuTjmax=0x$CpuTjmax
		echo_success "get_CpuTjmax:$CpuTjmax"
		echo "get_CpuTjmax:$CpuTjmax" >> $SAVEFILE
		return $(($CpuTjmax-15))
	fi		
}

#获取iRM心跳是否支持
get_irm_heartbeat_enable()
{
    #返回结果为01表示支持产品iRM心跳，02表示产品支持iRM心跳但是装备使能时临时关闭了iRM心跳
    run_cmd  "f 0 30 93 db 07 00 5b 0b 00 00 00"  "0f 31 00 93 00 db 07 00 02"
    if [ $? != 0 ] ; then
        run_cmd  "f 0 30 93 db 07 00 5b 0b 00 00 00"  "0f 31 00 93 00 db 07 00 01"
        if [ $? != 0 ] ; then
            result=0
        else
            result=1
        fi
    else
        result=1
    fi
	
    if [ $result = 1 ] ; then
        echo "irm heart beat enable!"
        echo "irm heart beat enable!" >>$SAVEFILE
    fi

    return $result
}

#获取SmartCooling是否支持
get_SmartCooling_Enable_state()
{
	fun_id=90					 #90的16进制，表示Cooling类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 			     #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=$(word2string 7)
	info_type=79				#y的ascii码,16进制
	info_len=01
	info_val=`printf "%02x" 1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 7 "$exp_data"	
	if [ $? != 0 ] ; then
		echo "get SmartCooling Enable state 0 success!"
       	echo "get SmartCooling Enable state 0 success!" >>$SAVEFILE
       	return 0
    else 
       	echo_success "get SmartCooling state 1 success!"
       	return 1
    fi
}


#获取是否处于液冷模式
get_Cooling_Medium_Liquid()
{
	fun_id=90					 #90的16进制，表示Cooling类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 			     #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=$(word2string 4)
	info_type=79				#y的ascii码,16进制
	info_len=01
	info_val=`printf "%02x" 1`
	key_string="$key_id $key_type $key_len $key_val"
	exp_data="$info_id $info_type $info_len $info_val"
	get_prop_cmd $fun_id "$key_string" 4 "$exp_data"	
	if [ $? != 0 ] ; then
		echo "get Medium Liquid state 0 success!"
       	echo "get Medium Liquid state 0 success!" >>$SAVEFILE
       	return 0
    else 
       	echo_success "get Medium Liquid 1 success!"
       	return 1
    fi
}

#获取SmartCooling是否支持的CustomSettingMask
get_SmartCooling_Custom_Setting_Mask()
{
	fun_id=90					 #90的16进制，表示Cooling类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 			     #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=11
	key_string="$key_id $key_type $key_len $key_val"
	get_prop_cmd $fun_id "$key_string" "$info_id"	

	if [ $? != 0 ] ; then
		echo "get SmartCooling Custom Setting Mask error!"
		echo "get SmartCooling Custom Setting Mask error!" >>$SAVEFILE
		return 0
	else 
		echo_success "get SmartCooling Custom Setting Mask success!"
		CustomSettingMask=$((16#`echo $GET_PROP_INF_STRING|awk -F' ' '{print $NF$(NF-1)$(NF-2)$(NF-3)}'`))   #取最后四个字节
		return 0
	fi
}



#获取单板槽位号
get_blade_num()
{
    BLADNUM=0
    
    run_cmd  "f 0 2c 1 0 0"  "0f 2d 00 01 00 00"
    if [ $? != 0 ] ; then
		echo_fail "get_blade_num fail!"
        echo "get_blade_num fail!" >> $SAVEFILE
        error_quit
	fi

	BLADNUM=`cat $IPMI_CMD_RETURN_FILE |  awk -F " " '{print $11}'`
	
	#BEGIN: 问题单 DTS2015111209712, 2015-11-12 by l00205093
	#十六进制转为十进制
	BLADNUM=`echo $((16#${BLADNUM}+0))`
	#END: 问题单 DTS2015111209712, 2015-11-12 by l00205093

	if [ $BLADNUM = 0 ] ; then
		echo_fail "get_blade_num:get blade slot id fail$BLADNUM!"
		echo "get_blade_num:get blade slot id fail$BLADNUM!" >>$SAVEFILE
		error_quit
	fi

	echo "get_blade_num:get blade slot id success $BLADNUM" >>$SAVEFILE
	return 
}

#解析配置文件（关键字提取函数  参数为：配置文件  关键词  返回值：赋值）
parameter=0
getini()    
{
    FILENAME=$1
    KEYWORD=$2
    #/* BEGIN: Modified by w00208253, 2016/1/5, PN: DTS2015102100420 */
    #parameter=`cat $FILENAME | grep "^${KEYWORD}\>"   | awk -F "=" '{print $2}'`
	
	tmp_param=`cat $FILENAME | grep "^${KEYWORD}\>"`
	parameter=`echo ${tmp_param#*=}`
    #/* END: Modified by w00208253, 2016/1/5, PN: DTS2015102100420 */

}

#判断并分割字符串
string2substring()
{
	STRING=$1
	local max_string_len=$2
	if [ "$max_string_len" = "" ] ; then
		max_string_len=48
	fi
	echo $STRING > temp.txt
	#对于STRING的最大长度为48位做判断
	if [ "${#STRING}" -gt $max_string_len ] ; then
		echo_fail "Surpasses the greatest length limit $max_string_len! write e_label $STRING failed!"
		error_quit
	fi
	#转换字符串为16进制字符串
	./conver temp.txt 0 1 > temp_tmp.txt
	if [ $? != 0 ] ; then
		echo_fail  "conver $STRING Fail"
		echo  "conver $STRING Fail"  >> $SAVEFILE
		error_quit
	fi
	
	TMP_STRING=`cat temp_tmp.txt`
	#整个字符串长度
	strlen=`echo ${#TMP_STRING}`
	#计算子字符长度
	len=`expr $MAX_STRING_NUM \* 3`
	#下个字符串开始位置
	offset=`expr $len + 1 `
	
	if [ "${#STRING}" -gt "$MAX_STRING_NUM" ] ; then
		TMP_STRING1=`echo $TMP_STRING | cut -b -$len`
		TMP_STRING2=`echo $TMP_STRING | cut -b $offset-$strlen`
	fi
}

#设置备份点
set_bak_point()
{
    local REPEAT_TIME_tmp=$REPEAT_TIME
    local REPEAT_tmp=$REPEAT
    REPEAT_TIME=30
    REPEAT=30
    sleep 30
    echo "Before set_bak_point : REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
    #此命令最后使用  把配置信息备份起来
    run_cmd "f 0 30 93 db 07 00 07 00 aa" "0f 31 00 93 00 db 07 00 01"
    if [ $? != 0 ] ; then
        echo_fail "Set bakpoint fail!"
        echo "Set bakpoint fail!" >>$SAVEFILE
        REPEAT_TIME=$REPEAT_TIME_tmp
        REPEAT=$REPEAT_tmp
        echo "After set_bak_point : REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
        error_quit
    else
        echo_success "Set bakpoint success!"
        echo "Set bakpoint success!" >>$SAVEFILE
        REPEAT_TIME=$REPEAT_TIME_tmp
        REPEAT=$REPEAT_tmp
        echo "After set_bak_point : REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
    fi
	

    SET_BAK_POINT_STATE=0
    SET_BAK_POINT_STATE_TRY_COUNT=0

    while [[ "$SET_BAK_POINT_STATE_TRY_COUNT" -lt 15 ]]
    do
    SET_BAK_POINT_STATE_TRY_COUNT=$(($SET_BAK_POINT_STATE_TRY_COUNT+1))

    run_cmd "f 0 30 93 db 07 00 07 00 55" ""
    if [ $? != 0 ] ; then
        echo "Get set_bak_point error: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
        error_quit
    else
        tmp_ret=`cat $IPMI_CMD_RETURN_FILE`
        echo $tmp_ret
        SET_BAK_POINT_STATE=${tmp_ret:24:2}
        echo $SET_BAK_POINT_STATE
        if [ "$SET_BAK_POINT_STATE" -eq 1 ];then
             echo "Get set_bak_point success: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
             return 0
        elif [ "$SET_BAK_POINT_STATE" -eq 2 ];then
             echo "Get set_bak_point failure 2: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
             error_quit	 
		fi
    fi
    sleep 2
    done

    echo "Get set_bak_point failure: REPEAT=$REPEAT REPEAT_TIME=$REPEAT_TIME"
    error_quit
}

converstring2hex()
{
	STRING=$1
	echo $STRING >temp.txt

	#转换字符串为16进制字符串
	./conver temp.txt 0 1 >temp_tmp.txt
	if [ $? != 0 ] ; then
		echo_fail  "conver $STRING Fail"
		echo  "conver $STRING Fail"  >>$SAVEFILE
		error_quit
	fi
	
	TMP_STRING=`cat temp_tmp.txt`
}

#写电子标签
write_elabel()
{
	string2substring "$1"
	strlen=`echo ${#1}`
	#判断是否使用子字符串
	if [ "$strlen" -gt "$MAX_STRING_NUM" ] ; then
		first_len=`printf %2x $MAX_STRING_NUM`
		last_len=`expr $strlen - $MAX_STRING_NUM `
		last_len=`printf %2x $last_len`
		#写入第一个字符串
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 80 $first_len $TMP_STRING1" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Set e_label:$1 fail!"
			echo "Set e_label:$1 fail!" >>$SAVEFILE
			error_quit
		fi
		
		#写入剩余字符串
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER $first_len $last_len $TMP_STRING2" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Set e_label:$1 fail!"
			echo "Set e_label:$1 fail!" >>$SAVEFILE
			error_quit
		fi

		echo_success "Set e_label:$1 success!"
		echo "Set e_label:$1 success!" >>$SAVEFILE
	
	elif [ "$strlen" -gt "0" ] ; then
		#直接写入整串字符
		strlen=`printf %2x $strlen`
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 00 $strlen $TMP_STRING" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Set e_label:$1 fail!"
			echo "Set e_label:$1 fail!" >>$SAVEFILE
			error_quit
		else
			echo_success "Set e_label:$1 success!"
			echo "Set e_label:$1 success!" >>$SAVEFILE
		fi
	else
		#写电子标签时如果长度为0则数据data域不用填写
		run_cmd "f 0 30 90 04 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 00 00" "0f 31 00 90 00"
		if [ $? != 0 ] ; then
			echo_fail "Clear e_label fail!"
			echo "Clear e_label fail!" >>$SAVEFILE
			error_quit
		else
			echo_success "Clear e_label success!"
			echo "Clear e_label success!" >>$SAVEFILE
		fi
	fi
}

#读电子标签
read_elabel()
{
	max_str_len=`printf %2x $MAX_STRING_NUM`
	#读取22个字符
	run_cmd "f 0 30 90 05 $FRU_ID $AREA_NUMBER $FIELD_NUMBER 00 $max_str_len" "0f 31 00 90 00"
	if [ $? != 0 ]; then
		echo_fail "read e_label fail!"
		echo "read e_label fail!" >> $SAVEFILE
		error_quit
	fi
	
	hextochar $IPMI_CMD_RETURN_FILE 19 temp_tmp.txt
	READ_ELABEL_STRING=`cat temp_tmp.txt`

	#根据返回值判断是否还有字符未读取完
	grep -wq "0f 31 00 90 00 80" $IPMI_CMD_RETURN_FILE
	if [ $? != 0 ]; then
		run_cmd "f 0 30 90 05 $FRU_ID $AREA_NUMBER $FIELD_NUMBER $max_str_len $max_str_len" "0f 31 00 90 00"
		if [ $? != 0 ]; then
			echo_success "read e_label:$READ_ELABEL_STRING"
			echo "read e_label:$READ_ELABEL_STRING" >> $SAVEFILE
		else
			hextochar $IPMI_CMD_RETURN_FILE 19 temp_tmp.txt
			READ_ELABEL_STRING=$READ_ELABEL_STRING`cat temp_tmp.txt`
			echo_success "read e_label:$READ_ELABEL_STRING"
			echo "read e_label:$READ_ELABEL_STRING" >> $SAVEFILE
		fi
	else
		echo_success "read e_label:$READ_ELABEL_STRING"
		echo "read e_label:$READ_ELABEL_STRING" >> $SAVEFILE
	fi
}

#读FRU ID集合信息
read_fruid_set()
{
	local offset=0
	local max_str_len=`printf %2x $MAX_STRING_NUM`
	
	while true
	do		
		offset=`printf %2x $offset`
		#读取24个FRU ID
		run_cmd "f 0 30 93 db 07 00 3b 00 ff 00 00 00 $offset $max_str_len" "0f 31 00 93 00"
		if [ $? != 0 ]; then
			echo_fail "read fru id info fail!"
			echo "read fru id info fail!" >> $SAVEFILE
			error_quit
		fi
		
		tmp_fruid_set=`cat $IPMI_CMD_RETURN_FILE`	
		tmp_fruid_set=`echo $tmp_fruid_set | cut -b 28-`	
		FRUID_SET="$FRUID_SET $tmp_fruid_set"
		
		#根据返回值判断是否还有字符未读取完
		grep -wq "0f 31 00 93 00 db 07 00 80" $IPMI_CMD_RETURN_FILE
		if [ $? == 0 ]; then
			offset=`expr $MAX_STRING_NUM + $offset`
		else
			FRUID_SET=$(remove_space "$FRUID_SET")
			echo_success "read fru id info:$FRUID_SET"
			echo "read fru id info:$FRUID_SET" >> $SAVEFILE			
			return 0
		fi		
	done
}

#校验FRU是否支持装备读写电子标签功能
check_fru_support_elabel()
{
	#通过读取FRU的电子标签信息来判断:d3表示没有eeprom，c9表示不支持装备读写电子标签
	ipmicmd -k "f 0 30 90 05 $1 01 00 00 ff" smi 0 > $IPMI_CMD_RETURN_FILE
	grep -wq "0f 31 00 90 d3" $IPMI_CMD_RETURN_FILE		
	if [ $? == 0 ]; then
		echo_success "FRU ID $1 not support eeprom function"
		echo "FRU ID $1 not support eeprom function" >> $SAVEFILE
		return 1		
	else
		grep -wq "0f 31 00 90 c9" $IPMI_CMD_RETURN_FILE
		if [ $? == 0 ]; then
			echo_success "FRU ID $1 not support elabel operation"
			echo "FRU ID $1 not support elabel operation" >> $SAVEFILE
			return 1
		fi
	fi
	return 0
}

#将id转换成string
word2string()
{
	key_val_2=`printf "%04x" $1 | cut -b 1-2`
	key_val_1=`printf "%04x" $1 | cut -b 3-4`
	local id_string="$key_val_1 $key_val_2"
	
	echo "$id_string"
}

#去掉前后空格
remove_space()
{
	local covert_string="$1"
	covert_string=`echo "$covert_string" | sed 's/^[ ]*//g' | sed 's/[ ]*$//g'`
	echo "$covert_string"
}

#将id转换成string
dword2string()
{
	key_val_4=`printf "%08x" $1 | cut -b 1-2`
	key_val_3=`printf "%08x" $1 | cut -b 3-4`
	key_val_2=`printf "%08x" $1 | cut -b 5-6`
	key_val_1=`printf "%08x" $1 | cut -b 7-8`
	local id_string="$key_val_1 $key_val_2 $key_val_3 $key_val_4"
	
	echo "$id_string"
}

#将字符串转换成十六进制
string2hex()
{
	STRING=$1
	echo $STRING > temp.txt
	#对于STRING的最大长度为255字节做判断
	if [ "${#STRING}" -gt 255 ] ; then
		echo_fail "Surpasses the greatest length limit 255!"
		error_quit
	fi
	#转换字符串为16进制字符串
	./conver temp.txt 0 1 > temp_tmp.txt
	if [ $? != 0 ] ; then
		echo_fail  "conver $STRING Fail"
		echo  "conver $STRING Fail"  >> $SAVEFILE
		error_quit
	fi
	
	TMP_STRING=`cat temp_tmp.txt`
	TMP_STRING=$(remove_space "$TMP_STRING")
	
	echo "$TMP_STRING"
}

#将字符串转换成十六进制且每个字节带上0x前缀
string2hex_with_prefix()
{
	string2hex "$1" | sed 's/^/0x/g' | sed 's/ / 0x/g'
}

get_BP_flag()
{
    local i
    for((i=0;i<$REPEAT;i++))
    do
        ipmicmd -k "f 0 30 90 59 10 00 00" smi 0 > $IPMI_CMD_RETURN_FILE
        flag=`cat $IPMI_CMD_RETURN_FILE | cut -d ' ' -f5`
        if [ $flag = 00 ] ; then
            bp_flag=`cat $IPMI_CMD_RETURN_FILE | cut -d ' ' -f6`
            return 0
        fi
        sleep $REPEAT_TIME
    done

    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
    return 1
}

#如果是白牌机
need_add_customization()
{
    get_BP_flag
    if [ "$bp_flag" = "01" ] || [ "$bp_flag" == "11" ] ; then
        return 0
    else
        return 1
    fi
}

#获取白牌机ProductUniqueID
get_BP_ProductUniqueID()
{
    local i
    local j
    for((i=0;i<$REPEAT;i++))
    do
        ipmicmd -k "f 0 30 90 59 12 00 00" smi 0 > $IPMI_CMD_RETURN_FILE
        flag=`cat $IPMI_CMD_RETURN_FILE | cut -d ' ' -f5`
        if [ $flag = 00 ] ; then
            for ((j=1;j<5;j++))
            {
                buf[$j]=`cat $IPMI_CMD_RETURN_FILE | cut -d ' ' -f $[$j+5]`
            }
            BP_ProductUniqueID="0x${buf[1]}${buf[2]}${buf[3]}${buf[4]}"
            return 0
        fi
        sleep $REPEAT_TIME
    done
    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
    return 1
}

#获取产品ProductUniqueID
get_ProductUniqueID()
{
    local i
    local j
    for((i=0;i<$REPEAT;i++))
    do
        ipmicmd -k "f 0 30 90 59 12 00 00" smi 0 > $IPMI_CMD_RETURN_FILE
        flag=`cat $IPMI_CMD_RETURN_FILE | cut -d ' ' -f5`
        if [ $flag = 00 ] ; then
            for ((j=1;j<5;j++))
            {
                buf[$j]=`cat $IPMI_CMD_RETURN_FILE | cut -d ' ' -f $[$j+5]`
            }
            PRODUCT_UNIQUE_ID="0x${buf[1]}${buf[2]}${buf[3]}${buf[4]}"
            echo_success "get_ProductUniqueID:$PRODUCT_UNIQUE_ID"
            echo "get_ProductUniqueID:$PRODUCT_UNIQUE_ID" >> $SAVEFILE
            return 0
        fi
        sleep $REPEAT_TIME
    done
    cat $IPMI_CMD_RETURN_FILE >> $IPMI_CMD_LOG_FILE
    echo_fail "get_ProductUniqueID fail!"
    echo "get_ProductUniqueID fail!" >> $SAVEFILE
    return 1
}

#打开黑匣子
open_black_box()
{
    #打开黑匣子
	run_cmd "f 0 30 93 db 07 00 0f 0c 00 00 00 00 01" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
	   echo "open balck box fail"
	   echo "open balck box fail" >>$SAVEFILE 
	   error_quit
	else
	   echo "open black box restart OS now!"
	   echo "open black box restart OS now!" >>$SAVEFILE
	   reboot
	fi
}

#加载驱动
load_driver()
{
	# suse 15 版本信息配置到了 /etc/os-release
	driver_path=${G_WORK_DIR}/driver.tar.gz
	if [ $is_x86 = 1 ] ; then
		if [ ! -f "/etc/SuSE-release" ] ; then
			driver_version=`cat /etc/os-release |grep VERSION_ID|awk -F "=" '{print $2}' | sed -e 's/"//g' | sed -e 's/\./sp/'`
			echo "SUSE Linux Version : ${driver_version}"
			edma_path=${G_WORK_DIR}/driver/suse${driver_version}/host_edma_drv.ko 
			veth_path=${G_WORK_DIR}/driver/suse${driver_version}/host_veth_drv.ko 
		else
			suse_version=`cat /etc/SuSE-release |grep VERSION|awk -F " " '{print $3}' `
			suse_patch=`cat /etc/SuSE-release |grep PATCHLEVEL|awk -F " " '{print $3}'`
			driver_version=suse${suse_version}sp${suse_patch}
			echo "SUSE Linux Version : ${driver_version}"
			edma_path=${G_WORK_DIR}/driver/suse${suse_version}sp${suse_patch}/host_edma_drv.ko 
			veth_path=${G_WORK_DIR}/driver/suse${suse_version}sp${suse_patch}/host_veth_drv.ko 
		fi
	else
		cat /etc/*-release > verlog.txt		
		grep SP5 verlog.txt > sp5.txt
		if test -s sp5.txt; then
			cat /etc/euleros-release
			edma_path=${G_WORK_DIR}/driver/euler2sp5/host_edma_drv.ko 
			veth_path=${G_WORK_DIR}/driver/euler2sp5/host_veth_drv.ko 
			cdev_path=${G_WORK_DIR}/driver/euler2sp5/host_cdev_drv.ko	
		fi	
	
		grep SP8 verlog.txt > sp8.txt
		if test -s sp8.txt; then
			cat /etc/euleros-release
			edma_path=${G_WORK_DIR}/driver/euler2sp8/host_edma_drv.ko 
			veth_path=${G_WORK_DIR}/driver/euler2sp8/host_veth_drv.ko 
			cdev_path=${G_WORK_DIR}/driver/euler2sp8/host_cdev_drv.ko	
		fi

		cat /etc/centos-release > verlog.txt
		grep "8.2" verlog.txt > cent82.txt
		if test -s cent82.txt; then
			cat /etc/centos-release
			edma_path=${G_WORK_DIR}/driver/centos82/host_edma_drv.ko
			veth_path=${G_WORK_DIR}/driver/centos82/host_veth_drv.ko
			cdev_path=${G_WORK_DIR}/driver/centos82/host_cdev_drv.ko
		fi
		
		#匹配Kylin V10
		grep V10 verlog.txt > V10.txt
		if test -s V10.txt; then
		cat /etc/kylin-release
		edma_path=${G_WORK_DIR}/driver/kylinv10/host_edma_drv.ko 
		veth_path=${G_WORK_DIR}/driver/kylinv10/host_veth_drv.ko 
		cdev_path=${G_WORK_DIR}/driver/kylinv10/host_cdev_drv.ko
		fi
		
		#删除判断文件避免残留影响二次判断
		rm -rf verlog.txt
		rm -rf sp5.txt
		rm -rf sp8.txt
		rm -rf cent82.txt
		rm -rf V10.txt
	fi
	
	#查看BMC的black_box_state，看黑匣子是否打开
	run_cmd "f 0 30 93 db 07 00 10 0c" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
	   echo "get black box state fail"
	   echo "get black box state fail" >>$SAVEFILE
	   error_quit
	else
		status=`cat $IPMI_CMD_RETURN_FILE |awk -F " " '{print $10}' `
		echo "status=$status"
		if [ $((16#$status)) -eq 1 ] ; then
		   echo "The black box state is open"
		   echo "The black box state is open" >>$SAVEFILE
		fi
	fi
	
	#查看OS测黑匣子是否打开
	str=`lspci | grep 1710`
	if [ -z "$str" ] ; then
		echo "pcie device is not present!"
		echo "pcie device is not present!" >>$SAVEFILE
		open_black_box
	else
		echo "pcie device is present!"
	fi
	
	#查看driver.tar.gz文件是否存在
	if [ ! -f "$driver_path" ] ; then
		echo "driver.tar.gz file is not present!"
		echo "driver.tar.gz file is not present!" >>$SAVEFILE
		error_quit
	else
		rm -rf driver
		tar zxf driver.tar.gz 
	fi  
	
	# 判断是否安装了edma驱动
	str=`lsmod |grep host_edma_drv`
	if [ -z "$str" ] ; then
		# 判断host_edma_drv文件是否存在
		if [ ! -f "$edma_path" ] ; then
		   echo "host_edma_drv file is not present!"
		   echo "host_edma_drv file is not present!" >>$SAVEFILE
		   error_quit
		fi  
		
		cd `dirname ${edma_path}`
		insmod host_edma_drv.ko
		if [ $? != 0 ] ; then
			echo "insmod edma driver failed!"
			echo "insmod edma driver failed!" >>$SAVEFILE
			error_quit
		fi
	fi
	
	# 判断是否安装了veth驱动
	str=`lsmod |grep host_veth_drv`
	if [ -z "$str" ] ; then	
		# 判断host_veth_drv文件是否存在
		if [ ! -f "$veth_path" ] ; then
		   echo "host_veth_drv file is not present!"
		   echo "host_veth_drv file is not present!" >>$SAVEFILE
		   error_quit
		fi 
		
		cd `dirname ${veth_path}`
		insmod host_veth_drv.ko
		if [ $? != 0 ] ; then
			echo "insmod veth driver failed!"
			echo "insmod veth driver failed!" >>$SAVEFILE
			error_quit
		fi
	fi
	
	cd ${G_WORK_DIR}
	ifconfig veth up

	echo "load driver ok!"
	ssh-keygen -R fe80::9e7d:a3ff:fe28:6ffa%veth>/dev/null 2>&1
	return 0
}

#传输文件
send_file()
{
	ssh_ip=$1
	src_file=$2
	des_path=$3
	user="root"
	pwd="Huawei12#$"
	
	if [ "$PRODUCT_VERSION_NUM" != "03" ];then
	   user=$AdminUserName
	   pwd=$AdminPassword
	fi
	
	if [ $# -lt 3 ] ; then
	   echo "Parameter error!"
	   echo "Parameter error!" >>$SAVEFILE
	   return 1
	fi
	
	# 增加scp重试解决veth协商概率失败问题
	for((i=0;i<$SCP_MAX_RETRY_TIME;i++)) {
		expect << EOF
		set timeout 300
		spawn scp -6r -l 4096 ${src_file} ${user}@${ssh_ip}:${des_path}
		expect {
		"*yes/no" { send "yes\r"; exp_continue }
		"*password:" { send "$pwd\r" }
		"*Password:" { send "$pwd\r" }
		}
	
		expect {
		"*password:" {exit 2}
		"*Password:" { exit 2 }
		}
EOF
		if [ $? -ne 0 ] ; then
			let scp_retry_times=$i+1
			echo "scp connection was not ready after trying ${scp_retry_times} times " | tee -a >>$SAVEFILE
			sleep $SCP_RETRY_INTERVAL
			continue
		else 
			break
		fi
	}
	if [ "$i" == "$SCP_MAX_RETRY_TIME" ] ;then
		echo "scp failed" | tee -a >>$SAVEFILE
		return 1
	else
		echo "scp successfully"
		return 0
	fi
}

#启动升级 如果发升级命令失败，先发送finsh命令，延时，然后再重发升级命令，重发3次
initiate_bmc_upgrade()
{
	for((i=0;i<3;i++))
	{
		run_cmd "f 0 30 91 db 07 00 06 AA 00 01 00 0E 2F 74 6D 70 2F 69 6D 61 67 65 2E 68 70 6D" "0f 31 00 91 00 db 07 00"
		if [ $? != 0 ] ; then
		   echo "The $i time initiate BMC upgrade fail"
		   echo "The $i time initiate BMC upgrade fail" >>$SAVEFILE
		   
		   #finish upgrade
		   run_cmd "f 0 30 91 db 07 00 06 55" "0f 31 00 91 00 db 07 00"
		   if [ $? != 0 ] ; then
			   echo "The $i time finish BMC upgrade fail"
			   echo "The $i time finish BMC upgrade fail" >>$SAVEFILE
			   return 1
			else
			   echo "The $i time finish BMC upgrade ok"
			   echo "The $i time finish BMC upgrade ok" >>$SAVEFILE
			   sleep 1
		   fi
		else
		   echo "initiate BMC upgrade $getversion ok"
		   echo "initiate BMC upgrade $getversion ok" >>$SAVEFILE
		   return 0
		fi
	}
	
	return 1
}
#升级进度查询 连续6次查询不到进度，返回错误
get_bmc_upgrade_status()
{
	error_counter=0
	
    for((i=0;i<600;i++))
	{
		run_cmd "f 0 30 91 db 07 00 06 00" "0f 31 00 91 00 db 07 00"
		if [ $? != 0 ] ; then
			((error_counter++))
			if [ $error_counter -eq 6 ] ; then
				echo "The $i time get BMC upgrade status fail error_counter: $error_counter"
				echo "The $i time get BMC upgrade status fail error_counter: $error_counter" >>$SAVEFILE
				return 1
			fi
		else
		    error_counter=0
		fi
		
		status=`cat $IPMI_CMD_RETURN_FILE |awk -F " " '{print $9}' `

        status=$(($((16#$status))&127))
		
		if [ $status -eq 100 ] ; then
		   echo "The $i time get BMC upgrade status is 100%"
		   echo "The $i time get BMC upgrade status is 100%" >>$SAVEFILE
		   return 0
		else
		   echo "The $i time get BMC upgrade status is $status%"
		   echo "The $i time get BMC upgrade status is $status%" >>$SAVEFILE
		fi
		
		sleep 1
	}
	
	echo "get BMC upgrade status overtime" 
	echo "get BMC upgrade status overtime" >>$SAVEFILE
	return 1
}

#传输文件，升级，查询进度
start_upgrade()
{

	file_name=$1
    #查询升级文件是否存在
	#update_hpm_file_path=/tmp/dftimage.hpm
	#hpm_file_path=${G_WORK_DIR}/dftimage.hpm
	hpm_file_path=${G_WORK_DIR}/$file_name
	if [ ! -f "$hpm_file_path" ] ; then
	   echo "hpm file is not present!"
	   echo "hpm file is not present!" >>$SAVEFILE
	   error_quit
	fi
	
    #传输文件
	send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" $file_name /tmp/image.hpm
	if [ $? != 0 ] ; then	 
	   echo "send hpm file fail!"
	   echo "send hpm file fail!" >>$SAVEFILE
	   return 1	
	fi
	
	#启动升级
	initiate_bmc_upgrade  
	if [ $? != 0 ] ; then	   
	   return 1	
	fi
	
	#查询进度
	get_bmc_upgrade_status
	if [ $? != 0 ] ; then	   
	   return 1	
	fi
	
	return 0
}

hpm_upgrade()
{
	file_name=$1
	
	for((k=0;k<$UPGRADE_REPEAT;k++))
	{
		start_upgrade $file_name
		if [ $? == 0 ] ; then
			echo "hpm load $file_name ok"
			return 0
		fi
	}

	echo "Try to upgrade $file_name with hpmfwupg"
	#通过veth升级失败则通过bt升级
	for((k=0;k<$UPGRADE_REPEAT;k++))
	{	
		inputfile=in 
		rm -fr $inputfile
		mknod $inputfile p 

		exec 8<>$inputfile 

		echo "0" >$inputfile 
		echo "y" >$inputfile 
		./hpmfwupg upgrade $file_name activate <&8
		if [ $? == 0 ] ; then
			echo "hpm load $file_name ok"
			return 0
		fi
	}
	echo_and_save_fail "hpm load $file_name fail!"
	error_quit	
}

#获取数组中的子数组
get_sub_array()
{
	local start=$1
	local end=$2
	local array=($3)
	local i=0
	local sub_string=""
	
	for one_data in ${array[@]}
	do
		if [ $i -gt $end ] ; then
			break
		fi
		
		if [ $i -ge $start ] ; then
			if [ "$sub_string" == "" ] ; then
				sub_string="$one_data"
			else
				sub_string="${sub_string} $one_data"
			fi			
		fi
		i=`expr $i + 1`
	done
	
	echo ${sub_string}
}

#通过统一的ipmi命令进行设置内部对象的属性
set_prop_cmd()
{
	local i
 	local fun_id=$(word2string $1)
	local set_cnt=$2
	local set_prop_data=($3)
	local set_prop_data_len=${#set_prop_data[@]}
	local tmp_set_prop_data_len=${#set_prop_data[@]}
	local end_flag=0
	local cur_len=0
        # START AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127
	
	if [ "$tmp_set_prop_data_len" -gt "$MAX_FUN_STRING_LEN" ] ; then
		end_flag=1
		key_value_len=${set_prop_data[3]}
		#header包括"$key_id $key_type $key_len $key_val $info_id $info_type"
		header_len=`expr $key_value_len + 4 + 2`
		#获取header的数据
		header_val=$(get_sub_array 0 $header_len "$3")
		info_len_index=`expr $header_len + 1`
		info_len=${set_prop_data[$info_len_index]}
		info_val_index=`expr $info_len_index + 1`
		info_val=${set_prop_data[@]:$info_val_index}
		for((i=0;i<$REPEAT;i++))
		do
			set_prop_multi_cmd "$fun_id" $set_cnt "$header_val" $info_len "$info_val"
			if [ $? = 0 ] ; then
				return 0
			fi
			sleep $REPEAT_TIME
		done
	else
		end_flag=0		
		offset=$cur_len			
		offset_s=$(word2string $offset)			
		cur_len=$set_prop_data_len
		tem_string=$(get_sub_array $offset $cur_len "$3")
		run_cmd "f 0 30 93 db 07 00 35 $fun_id $set_cnt $end_flag $offset_s $tem_string" "0f 31 00 93 00 db 07 00"
		if [ $? = 0 ] ; then
			return 0
		fi
	fi
	# END AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127
	return 1
}
# START AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127 
#通过ipmi命令设置内部属性，因有些类型为string，其长度可能很长，需要连续发送多帧命令，调用可以设置多帧的命令set_prop_multi_cmd
set_prop_multi_cmd()
{
 	#local fun_id=$(word2string $1)
	#local fun_id=($1)
	local set_cnt=$2
	local header_prop_value="$3"
	local header_prop_t=($3)
	local set_prop_data_len=`((i=0x$4)); echo $i`
	#local set_prop_data=($5)
	local header_value_len=${#header_prop_t[@]}	
	local tmp_set_prop_data_len=$set_prop_data_len
	#每一帧能发送的最大的有效属性数据长度（除去key info，property_info等信息）
	local max_send_string_len=`expr "$MAX_FUN_STRING_LEN" - $header_value_len - 1` 
	local end_flag=0
	local cur_len=0
	local prop_data_cur_len=0
	while true
	do
	    echo $tmp_set_prop_data_len
		if [ "$tmp_set_prop_data_len" -gt "$max_send_string_len" ] ; then
			end_flag=1
			tmp_set_prop_data_len=`expr $tmp_set_prop_data_len - $max_send_string_len`
			offset=$cur_len
			offset_s=$(word2string $offset)	
			cur_len=`expr $MAX_FUN_STRING_LEN + $offset`
			#此次发送属性数据的起始位置
			prop_start=$prop_data_cur_len
			prop_data_cur_len=`expr $max_send_string_len + $prop_start`
			#此次发送属性数据的结束为止
			prop_end=`expr $prop_data_cur_len - 1`
			#截取此次发送的属性数据
			tem_string=$(get_sub_array $prop_start $prop_end "$5")
			#计算此次发送的数据长度，并转化为16进制
			cur_data_len=`expr $prop_end - $prop_start + 1`
			send_prop_data_len=`printf "%02x" $cur_data_len`
			#echo ---------- $prop_data_cur_len -----------------
			run_cmd "f 0 30 93 db 07 00 35 $1 $set_cnt $end_flag $offset_s $header_prop_value $send_prop_data_len $tem_string" "0f 31 00 93 00 db 07 00"
			if [ $? != 0 ] ; then
				return 1
			fi
		else
			end_flag=0		
			offset=$cur_len			
			offset_s=$(word2string $offset)	
                        prop_start=$prop_data_cur_len			
			prop_end=$set_prop_data_len
			tem_string=$(get_sub_array $prop_start $prop_end "$5")
			cur_data_len=`expr $prop_end - $prop_start`
			send_prop_data_len=`printf "%02x" $cur_data_len`
			#echo .........$prop_data_cur_len...............
			run_cmd "f 0 30 93 db 07 00 35 $1 $set_cnt $end_flag $offset_s $header_prop_value $send_prop_data_len $tem_string" "0f 31 00 93 00 db 07 00"
			if [ $? != 0 ] ; then
				return 1
			fi
			break
		fi
	done
	
	return 0
}

 # END AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127
#通过统一的ipmi命令进行查询内部对象的属性,一次只查一个属性.
get_prop_cmd()
{
	local fun_id=$(word2string $1)
	local key_id_string=$2
	local info_id=$(word2string $3)
	local end_flag=0
	local offset=0
	local max_len=`printf "%02x" $MAX_FUN_STRING_LEN`
	local msg_flag=0
	local msg_tol_len=0
	GET_PROP_INF_STRING=""
	
	if [ $# -gt 3 ] ; then
		expect_string=$4
	fi
		
	local RETRY_COUNT=10		
	local i	
	for((i=0;i<$RETRY_COUNT;i++))
	do
	    fun_id=$(word2string $1)
	    key_id_string=$2
	    info_id=$(word2string $3)
	    end_flag=0
	    offset=0
	    max_len=`printf "%02x" $MAX_FUN_STRING_LEN`
	    msg_flag=0
	    msg_tol_len=0
		GET_PROP_INF_STRING=""
		while true
		do
			offset_s=$(word2string $offset)
			run_cmd "f 0 30 93 db 07 00 36 $fun_id 1 $max_len $offset_s $key_id_string $info_id" "0f 31 00 93 00 db 07 00"
			if [ $? != 0 ] ; then
				echo_fail "Get property value fail!($1 $2)"
				#echo "Get property value fail!($1 $2)" >>$SAVEFILE
				#error_quit
				break
			fi
			
			offset=`expr $MAX_FUN_STRING_LEN + $offset`
			# START AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127
			cat $IPMI_CMD_RETURN_FILE
			tmp_arr=(`cat $IPMI_CMD_RETURN_FILE`)
			tmp_array=`cat $IPMI_CMD_RETURN_FILE`
			#是否为最后一帧的结束标志在byte8
			end_flag=${tmp_arr[8]}
			#此次返回有效属性数据长度保存在byte12，先要把长度值转化为十进制，再累加
			tmp_len=${tmp_arr[12]}
			((tmp_len=0x$tmp_len))
			((msg_tol_len=0x$msg_tol_len))	
			msg_tol_len=`expr $tmp_len + $msg_tol_len`
			msg_tol_len=`printf %02x $msg_tol_len`
			#对于一个属性需要多帧获取，每一帧都会带有prop_info数据，只需要第一帧保存起来
			if [ $msg_flag -eq 0 ] ; then
				echo $tmp_arr
				#属性id，type在返回命令的byte9~byte11
				prop_info=$(get_sub_array 9 11 "$tmp_array")
				msg_flag=1
			fi
			#echo " >>>>>>>>>>> $prop_info <<<<<<<<<<<<<<<<"
			#每一帧返回的属性数据保存在byte13开始的位置，截取属性值并累加至上次的后面
			GET_PROP_INF_STRING=$GET_PROP_INF_STRING" ${tmp_arr[@]:13}"
			if [ "$end_flag" = "01" ] ; then
				#sleep 1
				continue
			else					
				break
			fi
		done
		#获取所有的数据帧之后，去掉空格后，与prop_info， length等信息拼接后再把多余的前后空格去掉
		GET_PROP_INF_STRING=$(remove_space "$GET_PROP_INF_STRING")
		GET_PROP_INF_STRING="$prop_info $msg_tol_len $GET_PROP_INF_STRING"
		GET_PROP_INF_STRING=$(remove_space "$GET_PROP_INF_STRING")
		# END AR81AEE165-CBEF-44fd-A604-426C638DC283 h00272616 20151127
		if [ $# -gt 3 ] ; then
			expect_string=$(remove_space "$expect_string")
			if [ "$expect_string" != "$GET_PROP_INF_STRING" ] ; then
				#return 1
				continue
			fi
			return 0
		else
			return 0		
		fi
	done

	echo "$GET_PROP_INF_STRING"
	echo "Get property value fail!($1 $2)" >>$SAVEFILE
	return 1
}

#web http进行特殊处理
get_webserver_http_state()
{
	getini $CONFIGFILE BMCSet_CustomWebserverHttpState
	STATE=$parameter
	if [ "$STATE" = "on" ] ;then
		getini $CONFIGFILE Custom_WebserverHttpState
		HTTP_STATE=$parameter
		if [ "$HTTP_STATE" == "disable" ] ;then
			return 0
		elif [ "$HTTP_STATE" == "enable" ] ;then
			return 1
		else
			echo_fail "Custom_WebserverHttpState parameter fail"
			echo "Custom_WebserverHttpState parameterfail">>$SAVEFILE
			error_quit
		fi
	else
		return 1
	fi
}

#rmcp不安全协议进行特殊处理
get_ipmilan_rmcp_state()
{
	getini $CONFIGFILE BMCSet_CustomIPMILANRmcpState
	STATE=$parameter
	if [ "$STATE" = "on" ] ;then
		getini $CONFIGFILE Custom_IPMILANRmcpState
		RMCP_STATE=$parameter
		if [ "$RMCP_STATE" == "disable" ] ;then
			return 0
		elif [ "$RMCP_STATE" == "enable" ] ;then
			return 1
		else
			echo_fail "Custom_IPMILANRmcpState parameter fail"
			echo "Custom_IPMILANRmcpState parameterfail">>$SAVEFILE
			error_quit
		fi
	else
		return 0
	fi
}

get_remote_ctrl_vmm_state()
{
	getini $CONFIGFILE BMCSet_CustomRemoteControlVmmState
	STATE=$parameter
	if [ "$STATE" = "on" ] ;then
		getini $CONFIGFILE Custom_RemoteControlVmmState
		RMCP_STATE=$parameter
		if [ "$RMCP_STATE" == "disable" ] ;then
			return 0
		elif [ "$RMCP_STATE" == "enable" ] ;then
			return 1
		else
			echo_fail "Custom_RemoteControlVmmState parameter fail"
			echo "Custom_RemoteControlVmmState parameterfail">>$SAVEFILE
			error_quit
		fi
	else
		return 1
	fi
}

#针对不同的服务获取不同的enable_info_id
get_enable_info_id()
{
	if [ "$1" == "SNMPAgent" ] ; then
		INFO_ID=3
	fi
	
	if [ "$1" == "RemoteControl" ] || [ "$1" == "Video" ]; then
		INFO_ID=2
	fi
	
	if [ "$1" == "IPMILAN" ] && [ "$2" == "1" ] ; then
		INFO_ID=4
	fi
	
	if [ "$1" == "IPMILAN" ] && [ "$2" == "2" ] ; then
		INFO_ID=5
	fi
}

#针对不同的服务获取不同的port_info_id
get_port_info_id()
{
	if [ "$1" == "SNMPAgent" ] ; then
		INFO_ID=7
	fi
	
	if [ "$1" == "RemoteControl" ] || [ "$1" == "Video" ]; then
		INFO_ID=1
	fi
	
	if [ "$1" == "IPMILAN" ] && [ "$2" == "1" ] ; then
		INFO_ID=1
	fi
	
	if [ "$1" == "IPMILAN" ] && [ "$2" == "2" ] ; then
		INFO_ID=2
	fi
}

get_default_ip_by_product()
{
 	local Eth2_IPADDR

	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
		Eth2_IPADDR=0.0.0.0
	elif [ $PRODUCT_ID = $PRODUCT_KUNPENG_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TAISHAN_1711_TYPE ] || [ $PRODUCT_ID = $PRODUCT_RACK_TYPE ] || [ $PRODUCT_ID = $PRODUCT_RACK_TYPE_AJ_CUSTOMIZE ] || [ $PRODUCT_ID = $PRODUCT_TAISHAN_TYPE ] || [ $PRODUCT_ID = $PRODUCT_TIANCHI_TYPE ] || ([ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && ([ $BOARD_ID = $BOARD_G2500_ID ] || [ $BOARD_ID = $BOARD_ATLAS880_ID ] || [ $BOARD_ID = $BOARD_ATLAS800_9010_ID ] || [ $BOARD_ID = $BOARD_ATLAS500_ID ])) ; then
		Eth2_IPADDR=192.168.2.100
	elif [ $PRODUCT_ID = $PRODUCT_8100_TYPE ] || [ $PRODUCT_ID = $PRODUCT_8100_TYPE_AJ_CUSTOMIZE ] ; then
		Eth2_IPADDR=192.168.2.100
	elif [ $PRODUCT_ID = $PRODUCT_XSERIALS_TYPE ] || [ $PRODUCT_ID = $PRODUCT_XSERIALS_TYPE_AJ_CUSTOMIZE ] || ([ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID != $BOARD_G2500_ID ]); then
		get_blade_num 1>&2
		ip4=`expr 100 + $BLADNUM `
		Eth2_IPADDR="10.10.1.$ip4"	
	fi

	echo "$Eth2_IPADDR"
}

get_default_restore_ip_by_product()
{
 	local Eth2_DEFAULT_IPADDR

	if [ $PRODUCT_ID = $PRODUCT_OSCA_TYPE ] ; then
		Eth2_DEFAULT_IPADDR=0.0.0.0
	elif [ $PRODUCT_ID = $PRODUCT_TCE_TYPE ] ; then
		get_blade_num 1>&2
		ip4=`expr 110 + $BLADNUM `
		Eth2_DEFAULT_IPADDR="192.168.1.$ip4"
	fi
	
	echo "$Eth2_DEFAULT_IPADDR"
}

get_net_service_name()
{
	local SERVICE_TYPE=$1
	local item=$2

	if [ "$SERVICE_TYPE" = "WEBServer" ] && [ "$item" == "1"  ]; then
		echo "HTTP"
	elif [ "$SERVICE_TYPE" = "WEBServer" ] && [ "$item" == "2"  ] ; then
		echo "HTTPS"
	elif [ "$SERVICE_TYPE" = "IPMILAN" ] && [ "$item" == "1"  ] ; then
		echo "RMCP"
	elif [ "$SERVICE_TYPE" = "IPMILAN" ] && [ "$item" == "2"  ] ; then
		echo "RMCP_PLUS"
	elif [ "$SERVICE_TYPE" = "RemoteControl" ] && [ "$item" == "1"  ] ;  then
		echo "KVM"
	elif [ "$SERVICE_TYPE" = "RemoteControl" ] && [ "$item" == "2"  ] ;  then
		echo "VMM"
	else
		echo "$SERVICE_TYPE"
	fi     
}

#检测是X86还是ARM
check_is_x86()
{
	local temp=""
	uname -a > arch_temp
	grep -wq "aarch64" arch_temp
    if [ $? = 0 ] ; then
		echo "The machine is ARM."
		return 0
	fi	
	echo "The machine is X86."
	return 1
}
#检测是否采用新名字,1新名字，0老名字
check_is_new_name()
{
	PRODUCT_NAME_ID=$PRODUCT_ID$BOARD_ID
	check_is_x86
	#TaiShan空定制化，需要判断新老名称
	if [ $? = 0 ];then
		if [ $PRODUCT_NAME_ID = $PRODUCT_XA320_A6_ID ] || [ $PRODUCT_NAME_ID = $PRODUCT_XA320_B0_ID ] || [ $PRODUCT_NAME_ID = $PRODUCT_TAISHAN2280V2_ID ] ; then
			FRU_ID=0
			AREA_NUMBER=3
			FIELD_NUMBER=1
			read_elabel
			TAISHAN_NAME="TaiShan 200"
			echo "$READ_ELABEL_STRING" > temp.txt
			grep -wq "$TAISHAN_NAME" temp.txt
			if [ $? = 0 ] ; then
				echo "The machine is new name:$READ_ELABEL_STRING."
				return 1
			else
				echo "The machine is old name:$READ_ELABEL_STRING."
				return 0
			fi
		fi
	fi
	return 0
}

#升级进入装备生产状态，以便能进行一些专门的操作，该状态复位丢失
enable_dft_mode()
{
    if [ "$PRODUCT_VERSION_NUM" != "03" ];then
        hpm_upgrade dftimage.hpm
    elif [ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID = $BOARD_G2500_ID ]; then
        hpm_upgrade dftimage.hpm
    else
        inputfile=in 
		rm -fr $inputfile
		mknod $inputfile p 

		exec 8<>$inputfile 

		echo "0" >$inputfile 
		echo "y" >$inputfile 
		./hpmfwupg upgrade dftimage.hpm activate <&8
    fi   
	
}

clear_white_branding_config()
{
    if [ "$PRODUCT_VERSION_NUM" != "03" ];then
        hpm_upgrade clear_wbd_config.hpm
    elif [ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID = $BOARD_G2500_ID ]; then
        hpm_upgrade clear_wbd_config.hpm
    else
        inputfile=in 
		rm -fr $inputfile
		mknod $inputfile p 

		exec 8<>$inputfile 

		echo "0" >$inputfile 
		echo "y" >$inputfile 
		./hpmfwupg upgrade clear_wbd_config.hpm activate <&8
    fi   
	
}

#复位BMC
reset_bmc()
{
	#BEGIN: 问题单 DTS2017082312464, 2017-9-21 by z00306487
    local reset_time=180
	#END: 问题单 DTS2017082312464, 2017-9-21 by z00306487

    echo "please wait for 6 second and reset bmc!"
    for((i=0;i<6;i++))
    do
	    echo -n -e "\b\b$i"
	    sleep 1
    done

    for((i=0;i<3;i++))
    do
        run_cmd "f 0 06 02" "0f 07 00 02 00"
        if [ $? -ne 0 ] ; then
            sleep 1
            continue
        fi
        break
    done

    if [ $i -eq 3 ] ; then
        echo_fail "BMC reset test Fail"
        echo "BMC reset test Fail" >>$SAVEFILE
        error_quit
    fi

    echo "In reset_bmc, Project is : $project_code"
    if [ EMP$project_code == "EMP05021MCQ" ] ; then
        reset_time=480
    fi
    if [ EMP$project_code == "EMP05022EUH" ] ; then
        reset_time=360
    fi
    if [ EMP$project_code == "EMP05022UFD" ] ; then
        reset_time=240
    fi
    
    echo "please wait for $reset_time second and reset bmc!"
    for((i=0;i<$reset_time;i++))
    do
	    echo -n -e "\r$i"
	    sleep 1
    done
    
    echo ""

    for((i=0;i<12;i++))
    {
        run_cmd "f 0 06 01" "0f 07 00 01 00"
        if [ $? -ne 0 ] ; then
            echo "please wait for 10 second!"
            sleep 10
        else 
            break
        fi
    }

    if [ $i -eq 12 ] ; then
        echo_fail "BMC reset test Fail"
        echo "BMC reset test Fail" >>$SAVEFILE
        error_quit
    fi

    echo_success "BMC reset ok"
    echo "BMC reset ok" >>$SAVEFILE

    return 0
}

reinit_admin_passswd()
{
	local verify_username=""
	local verify_passwd=""	
	
	if [ "$1" == "config" ];then
	
		echo "restore admin user name and passwd before config"
		source ./function/restore_admin_user.sh
		
		restore_admin_user
		return
	fi
	
	getini  $CONFIGFILE  BMCSet_CustomAdminInfo
	ADMIN_INFO=$parameter
	if [ "$ADMIN_INFO" = "on" ] ; then

		getini  $CONFIGFILE  Custom_AdminName
		verify_username=$parameter
		if [ "$verify_username" != "" ] ; then
			AdminUserName=$verify_username
		fi
	
		getini  $CONFIGFILE  Custom_AdminPassword
		verify_passwd=$parameter
		if [ "$verify_passwd" != "" ] ; then
			AdminPassword=$verify_passwd
		fi
	fi
}

init_common()
{
	local is_x86=0
	merge_config_file $1 $2 > /dev/null 2&>1

	#加载ipmi驱动
	service ipmi start > /dev/null 2&>1

	#获取产品版本号，V3，V5等
	get_product_version_num

	#EQUIPMENT_SEPARATION_SUPPORT_STATE为0表示不支持装备分离，保持原有流程，开启dft验证
	if [ "$EQUIPMENT_SEPARATION_SUPPORT_STATE" == 0 ];then
		DFT_enable
	else
		customize_enable
	fi
	sleep 1

	reinit_admin_passswd $3
	
	#清空日志文件
	date > $SAVEFILE

	check_is_x86
	is_x86=$?
	#V5需要安装驱动
	if [ "$PRODUCT_VERSION_NUM" != "03" ] && [ $is_x86 = 1 ];then
		load_driver
	fi

	# arm也需要安装驱动
	if [ "$is_x86" -eq 0 ];then
		load_driver
	fi

	enable_dft_mode
	sleep 3
	need_add_customization
	if [ $? -eq 0 ] ; then
		#获取合作伙伴的配置项
		source ./function/cooperation_customset.sh
		#将合作伙伴的配置项追加到第二个配置文件中
		cat $SAVE_CUSTOMIZE_FILE >> $CONFIGFILE
	fi

	#获取product id
	get_product_id

	#获取boardid
	get_board_id

    #获取ProductUniqueID
    get_ProductUniqueID
    
	#获取GPU载板boardid DTS2018060707573 【G530v5】【GP308】G530V5配GP308载板，生产空定制化脚本失败
	if [ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID != $BOARD_ATLAS880_ID ] && [ $BOARD_ID != $BOARD_ATLAS800_9010_ID ] && [ $BOARD_ID != $BOARD_ATLAS500_ID ]; then
	    if [ $BOARD_ID = $BOARD_G560V5_ID ]; then
		    get_gpu_board_id "0a"
	    else
		    get_gpu_board_id "05"
	    fi
	fi

	# G2500是V3产品，但是使用了数字签名，需要加载driver
	if [ $PRODUCT_ID = $PRODUCT_ATLAS_TYPE ] && [ $BOARD_ID = $BOARD_G2500_ID ]; then
	    load_driver
	fi
}
#判断参数是否为数字
checkInt(){ 
    tmp=`echo $1 |sed 's/[0-9]//g'` 
    [ -n "${tmp}" ]&& { echo "Args must be integer!";return 1; } 
	return 0;
} 

#AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑:获取默认拓扑模式
get_default_pcie_topo_mode()
{
	fun_id=106					 #十进制的106表示编排类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 			     #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=5                    #属性Id为5表示默认拓扑
	key_string="$key_id $key_type $key_len $key_val"
	get_prop_cmd "$fun_id" "$key_string" $info_id
	if [ $? -eq 0 ] ; then
		echo "get default pcie topo mode success!"
       	echo "get default pcie topo mode success!" >>$SAVEFILE
        
       	return $((16#`echo $GET_PROP_INF_STRING|awk -F' ' '{print $NF}'`))   #取最后一个字符
    else 
       	echo_fail "get default pcie topo mode fail!"
        echo "get default pcie topo mode!" >>$SAVEFILE
       	error_quit
    fi
}

#获取该产品是否支持WOL网络唤醒功能的信息 DTS2018032601574 
get_wol_config_enable_state()
{
	fun_id=62					 #十进制的62表示PMEServiceConfig类
	key_id=$(word2string 0xffff) #对象没有索引属性时填0xffff，则查询类的第一个对象
	key_type=00 			     #key_id=0xffff时值无效
	key_len=01
	key_val=00
	info_id=5	 				 #属性Id为5表示产品是否支持WOL功能
	key_string="$key_id $key_type $key_len $key_val"
	get_prop_cmd "$fun_id" "$key_string" $info_id
	if [ $? -eq 0 ] ; then
		echo "get wol config enable state success!"
       	echo "get wol config enable state success!" >>$SAVEFILE
        
       	return $((16#`echo $GET_PROP_INF_STRING|awk -F' ' '{print $NF}'`))   #取最后一个字符
    else 
       	echo_fail "get wol config enable state fail!"
        echo "get wol config enable state fail!" >>$SAVEFILE
       	error_quit
    fi
}

#添加临时用户
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
			error_quit
		fi
    else
        echo "del user$1 name success!" >>$SAVEFILE
        return 0
    fi
}

#校验user 17是否为空
check_user_name_null()
{
	userid=`printf %2x $1`
	run_cmd "f 00 06 46 $userid" "0f 07 00 46 00 00 00 00 00 00 00 00 00 00"
	if [ $? != 0 ] ; then
		 echo_fail "user$1 is already exit"
		 echo "user$1 is already exit" >>$SAVEFILE
		 error_quit
	else
		 return 0
	fi
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
set_user_name_tmp()
{
	#校验user 17是否已经设置了
	check_user_name_null $1
	if [ $? != 0 ] ; then
		echo_fail "user $1 is not null"
	    error_quit 
    fi	

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
set_user_passwd_tmp()
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
set_user_access_tmp()
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

#使能用户的ssh接口
set_user_ssh_interface()
{
	key_value=`printf %2x $1`
	run_cmd "0f 00 30 93 db 07 00 35 34 00 01 00 00 00 01 00 79 01 $key_value 03 00 75 04 08 00 00 00" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user ssh interface fail"
		echo "set user ssh interface fail"  >>$SAVEFILE
		error_quit
	fi
}

#配置默认管理员帐户信息
add_admin_user()
{
	USER_ACCESS="admin"
	echo "add_admin_user"
	set_user_name_tmp 17
	sleep 1
	set_user_passwd_tmp 17
	sleep 2
	set_user_access_tmp 17
	sleep 1
	
	set_user_ssh_interface 17
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
		error_quit
	fi
}

#设置弱口令开关为enable
set_password_weak_enable()
{
	run_cmd "0f 00 30 93 db 07 00 35 02 00 01 00 00 00 ff ff 00 01 00 0f 00 00 01 01" "0f 31 00 93 00 db 07 00"
	if [ $? != 0 ] ; then
		echo_fail "set user passwd weak enable fail"
		echo "set user passwd weak enable fail" >>$SAVEFILE
		error_quit
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

#检查是否是泰山产品，且是否是鲲鹏定制化
need_special_dispose()
{
	check_is_x86
	if [ $? -eq 0 ]; then
		run_cmd "f 0 30 90 59 04 00 00" "0f 31 00 90 00 00 03"
		if [ $? -eq 0 ]; then
			#需要特殊处理
			return 0
		else
			return 1
		fi
	fi

	return 1
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

#判断是否是数字
is_digit()
{
	echo "$1" | grep -q '^[[:digit:]]\+$'
}

# 判断字符串中是否含有空格
is_have_space()
{
	# 可以同时检测空格和TAB
	echo "$1" | grep -q "[[:space:]]"
}

#获取厂商id
get_custom_manu_id()
{
    custom_manu_id="db 07 00"
    cmd_return=$(ipmicmd -k "f 0 30 90 22 08" smi 0)
    custom_manu_id=${cmd_return:18:8}
    if [ ${#custom_manu_id} -ne 8 ] ; then
        custom_manu_id="db 07 00"
    fi
    echo $custom_manu_id
}