#!/bin/sh

REPEAT=6
REPEAT_TIME=1 
IPMI_CMD_LOG_FILE=custom.log
IPMI_CMD_RETURN_FILE=ipmi_cmd_return_string.txt
SAVEFILE=05024HQH_save.txt

function error_quit()
{
    exit 1
}

function ok_quit()
{
    exit 0
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

chmod +x *
dos2unix *ini

set_ipmi_version_number()
{
	version_number=$1
	sub_cmd=0x3c
	selector=0x10
	
	run_cmd  "f 0 30 93 db 07 00 $sub_cmd $selector 25 00 00 00 00 00 00 01 $version_number"  "0f 31 00 93 00"
	if [ $? != 0 ] ; then
		echo "Set version number failed!"
		echo "Set version number failed!" >>$SAVEFILE
		error_quit
	else
		echo "Set version number success!"
        echo "Set version number success!" >>$SAVEFILE
	fi
}

check_revision_number()
{
	version_number=00
	sub_cmd=0x27
	selector=0x10
	
	run_cmd  "f 0 30 93 db 07 00 $sub_cmd $selector 01 25 00 ff"  "0f 31 00 93 00 db 07 00 00 $version_number"
	if [ $? != 0 ] ; then
        version_number=01
        run_cmd  "f 0 30 93 db 07 00 $sub_cmd $selector 01 25 00 ff"  "0f 31 00 93 00 db 07 00 00 $version_number"
        if [ $? != 0 ] ; then
            echo "Check version number failed!"
            echo "Check version number failed!" >>$SAVEFILE
            error_quit
        fi
	fi

    echo "Check version number success!"
    echo "Check version number success!" >>$SAVEFILE
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

# 设置eec数据0x00
VersionNumber=0x00
set_ipmi_version_number $VersionNumber

#查询eec数据0x00是否设置成功
check_revision_number

ok_quit
