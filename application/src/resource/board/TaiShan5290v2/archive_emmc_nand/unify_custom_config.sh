chmod +x *

CUSTOM_FLAG=0
CUSTOM_LOG_FILE="unify_custom_config_save.txt"

echo > ${CUSTOM_LOG_FILE}

if [ "$1" == "" ]; then
    echo_fail "there is no input para in unify custom config script"
    echo "there is no input para in unify custom config script" >> ${CUSTOM_LOG_FILE}
    error_quit
elif [ "$1" == 0 ]; then
    CUSTOM_FLAG=0
    echo_success "there is foraward custom config"
    echo "there is foraward custom config" >> ${CUSTOM_LOG_FILE}
elif [ "$1" == 1 ]; then
    CUSTOM_FLAG=1
    echo_success "there is backward custom config"
    echo "there is backward custom config" >> ${CUSTOM_LOG_FILE}
else
    echo_fail "the input para in unify custom config script wrong"
    echo "the input para in unify custom config script wrong" >> ${CUSTOM_LOG_FILE}
    error_quit
fi

function query_produce_name()
{
	#判断默认的用户名密码是否被修改，如果被修改就创建临时账号密码
	check_default_name_pswd

	declare -A dir
	#创建一个字典进行uid和产品名称的对照
    dir=([0x02070500]="S920X03" [0x02070400]="S920X03")

	PRODUCT_UID=$(curl -k -s -u ${UserName}:${Password} -g "https://[${IPV6_ADDR}]/redfish/v1/Managers/1" | awk -F "\"ProductUniqueID\":" '{print $2}' | awk -F "\"" '{print $2}')
	echo ${ProductUniqueID} >>$SAVEFILE
	if [ $? -ne 0 -o -z ${PRODUCT_UID} ];
	then	
		echo_fail "fail to get ProductUniqueID"
		error_quit
	else	
		echo "get the ProductUniqueID success" >>$SAVEFILE
	fi

	for key in $(echo ${!dir[*]})
	do
		if [ ${key} = ${PRODUCT_UID} ];
		then
			PRODUCT_NAME=${dir[${key}]}
			echo_success "get the produce_name :${PRODUCT_NAME}"
			echo "the uid ${PRODUCT_UID} match name is ${PRODUCT_NAME}" >>$SAVEFILE
			break
		fi
	done

	if [ "${PRODUCT_NAME}" = "" ];
	then
		echo_fail "cant match produce_name"
		error_quit
	fi
	
	#判断是否创建了user17，如果创建了就删除，防止影响其他功能
	check_userid_null 17
}

# 设置机器别名函数
set_machine_alias_function()
{
    MACHINEALIAS="$1"
    
    strlen=`echo ${#MACHINEALIAS}`
	info_val=$(string2hex "$MACHINEALIAS")
	info_len=`printf "%02x" $strlen`
    
    if [ ${strlen} -gt 16 ]; then
        echo_fail "the machine_alias is too long"
        echo "the machine_alias is too long" >>$SAVEFILE
        error_quit
    fi
    
    run_cmd "f 0 30 93 db 07 00 35 30 00 1 0 00 00 ff ff 00 01 00 08 00 73 ${info_len} ${info_val}" "0f 31 00 93 00 db 07 00"
    if [ $? != 0 ]; then
        echo_fail "set_machine_alias fail"
        echo "set_machine_alias fail" >>$SAVEFILE
        error_quit
    fi
    
    echo_success "set_machine_alias $MACHINEALIAS success"
}

# 设置机器别名
set_machine_alias()
{
    machine_alias=""
	
 	declare -A dir
	
    dir=([0x02070400]="4U")

	for key in $(echo ${!dir[*]})
	do
		if [ ${key} = ${PRODUCT_UID} ];
		then
			machine_alias=${dir[${key}]}
			echo_success "get the machine_alias :${machine_alias}"
			echo "the uid ${PRODUCT_UID} match machine_alias and alias is ${machine_alias}" >>$SAVEFILE
			break
		fi
	done

	if [ "${machine_alias}" = "" ];
	then
		echo "this machine needn't set machine_alias"
        return
	fi
	
    # 设置机器别名函数
    set_machine_alias_function "${machine_alias}"
}

custom_forward_config_function()
{
    #redfish接口查询UID，根据UID转换产品名称
    query_produce_name
    # 设置机器名称
    set_machine_name "${PRODUCT_NAME}"
    # 检测机器名称
    check_machine_name "${PRODUCT_NAME}"
    # 设置机器别名
    set_machine_alias
}

custom_backward_config_function()
{
    # 暂时逆向无特殊配置
    return 0
}

if [ ${CUSTOM_FLAG} == 0 ]; then
    # 如果是正向特殊配置，就运行正向特殊配置函数
    custom_forward_config_function
elif [ ${CUSTOM_FLAG} == 1 ]; then
    # 如果是逆向特殊配置，就运行逆向特殊配置函数
    custom_backward_config_function
else
    echo_fail "the custom flag is wrong"
    echo "the custom flag is wrong" >>$CUSTOM_LOG_FILE
    error_quit
fi



