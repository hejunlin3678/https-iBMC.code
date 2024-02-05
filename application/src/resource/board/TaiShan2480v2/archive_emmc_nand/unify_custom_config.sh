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
    dir=([0x02070100]="S920X02" [0x02070101]="S920X02" [0x0207010A]="S920X02K")

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

custom_forward_config_function()
{
    #redfish接口查询UID，根据UID转换产品名称
    query_produce_name
    # 设置机器名称
    set_machine_name "${PRODUCT_NAME}"
    # 检测机器名称
    check_machine_name "${PRODUCT_NAME}"
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



