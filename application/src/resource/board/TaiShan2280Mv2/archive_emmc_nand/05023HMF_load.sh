#!/bin/sh

REPEAT=6
REPEAT_TIME=1 
CONFIGFILE=trustzone_revision.ini
parameter=0
IPMI_CMD_LOG_FILE=custom.log
IPMI_CMD_RETURN_FILE=ipmi_cmd_return_string.txt
SAVEFILE=05023HMF_save.txt

function error_quit()
{
    exit 1
}

function ok_quit()
{
    exit 0
}	

echo_fail()
{
	echo -e "\033[31;1m$1\033[0m"
}

echo_success()
{
	echo -e "\033[32;1m$1\033[0m"
}

#命令执行函数  参数为：命令  正确返回值  保存的文件（$IPMI_CMD_RETURN_FILE）
run_cmd()
{
    local i
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

#解析配置文件（关键字提取函数  参数为：配置文件  关键词  返回值：赋值）
parameter=0
getini()    
{
    FILENAME=$1
    KEYWORD=$2
	
	tmp_param=`cat $FILENAME | grep "^${KEYWORD}\>"`
	parameter=`echo ${tmp_param#*=}`
}

chmod +x *
dos2unix *ini

set_ipmi_revision_number()
{
	firmware_type=$1
	revision_number=`printf "%02x" $2`
	sub_cmd=0x5a
	selector=0x2d
	
	run_cmd  "f 0 30 93 db 07 00 $sub_cmd $selector 00 02 00 $firmware_type $revision_number"  "0f 31 00 93 00"
	if [ $? != 0 ] ; then
		echo "Set revision number failed!"
		echo "Set revision number failed!" >>$SAVEFILE
		error_quit
	else
		echo "Set revision number success!" >>$SAVEFILE
	fi
}

check_revision_number()
{
	firmware_type=$1
	revision_number=`printf "%02x" $2`
	sub_cmd=0x5b
	selector=0x2d
	
	run_cmd  "f 0 30 93 db 07 00 $sub_cmd $selector 00 01 00 $firmware_type"  "0f 31 00 93 00 db 07 00 $revision_number"
	if [ $? != 0 ] ; then
		echo "check revision number failed!"
		echo "check revision number failed!" >>$SAVEFILE
		error_quit
	else
		echo "check revision number success!" >>$SAVEFILE
	fi
}
enable_dft_mode()
{
	./upgrade_hpm.sh "dftimage.hpm" 0
	if [ $? != 0 ]; then
		echo "upgrade dftimage.hpm fail"
		error_quit
	fi
}

enable_dft_mode
sleep 1

#设置BMC版本防回退RevisionNumber
getini $CONFIGFILE BMCRevisionNumber
RevisionNumber=$parameter
FirmwareType=0xfe
if [ "$RevisionNumber" != "" ] ; then
	set_ipmi_revision_number $FirmwareType $RevisionNumber
	echo "Set BMC revision number to $RevisionNumber success!"
	echo "Set BMC revision number to $RevisionNumber success!" >>$SAVEFILE
else
	set_ipmi_revision_number $FirmwareType 0
fi

#查询BMC版本防回退RevisionNumber是否设置成功
getini $CONFIGFILE BMCRevisionNumber
RevisionNumber=$parameter
FirmwareType=0xfe
if [ "$RevisionNumber" != "" ] ; then
	check_revision_number $FirmwareType $RevisionNumber
	echo "Check BMC revision number success!"
	echo "Check BMC revision number to $RevisionNumber success!" >>$SAVEFILE
else
	check_revision_number $FirmwareType 0
fi

#设置BIOS版本防回退RevisionNumber
getini $CONFIGFILE BIOSRevisionNumber
RevisionNumber=$parameter
FirmwareType=0xfd
if [ "$RevisionNumber" != "" ] ; then
	set_ipmi_revision_number $FirmwareType $RevisionNumber
	echo "Set BIOS revision number to $RevisionNumber success!"
	echo "Set BIOS revision number to $RevisionNumber success!" >>$SAVEFILE
else
	set_ipmi_revision_number $FirmwareType 0
fi

#查询BIOS版本防回退RevisionNumber是否设置成功
getini $CONFIGFILE BIOSRevisionNumber
RevisionNumber=$parameter
FirmwareType=0xfd
if [ "$RevisionNumber" != "" ] ; then
	check_revision_number $FirmwareType $RevisionNumber
	echo "Check BIOS revision number success!"
	echo "Check BIOS revision number to $RevisionNumber success!" >>$SAVEFILE
else
	check_revision_number $FirmwareType 0
fi

ok_quit
