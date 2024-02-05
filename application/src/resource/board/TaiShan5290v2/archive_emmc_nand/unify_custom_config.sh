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
	#�ж�Ĭ�ϵ��û��������Ƿ��޸ģ�������޸ľʹ�����ʱ�˺�����
	check_default_name_pswd

	declare -A dir
	#����һ���ֵ����uid�Ͳ�Ʒ���ƵĶ���
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
	
	#�ж��Ƿ񴴽���user17����������˾�ɾ������ֹӰ����������
	check_userid_null 17
}

# ���û�����������
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

# ���û�������
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
	
    # ���û�����������
    set_machine_alias_function "${machine_alias}"
}

custom_forward_config_function()
{
    #redfish�ӿڲ�ѯUID������UIDת����Ʒ����
    query_produce_name
    # ���û�������
    set_machine_name "${PRODUCT_NAME}"
    # ����������
    check_machine_name "${PRODUCT_NAME}"
    # ���û�������
    set_machine_alias
}

custom_backward_config_function()
{
    # ��ʱ��������������
    return 0
}

if [ ${CUSTOM_FLAG} == 0 ]; then
    # ����������������ã������������������ú���
    custom_forward_config_function
elif [ ${CUSTOM_FLAG} == 1 ]; then
    # ����������������ã������������������ú���
    custom_backward_config_function
else
    echo_fail "the custom flag is wrong"
    echo "the custom flag is wrong" >>$CUSTOM_LOG_FILE
    error_quit
fi



