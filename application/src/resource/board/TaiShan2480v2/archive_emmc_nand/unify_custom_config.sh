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
	
	#�ж��Ƿ񴴽���user17����������˾�ɾ������ֹӰ����������
	check_userid_null 17
}

custom_forward_config_function()
{
    #redfish�ӿڲ�ѯUID������UIDת����Ʒ����
    query_produce_name
    # ���û�������
    set_machine_name "${PRODUCT_NAME}"
    # ����������
    check_machine_name "${PRODUCT_NAME}"
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



