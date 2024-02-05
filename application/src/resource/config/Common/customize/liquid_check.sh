#!/bin/bash


#******************  default info cann't be changed  ****************
TuName=`echo $0 | awk -F "/" '{print $NF}' | awk -F "." '{print $1}'`			# Get script name
SoftID=`echo $TuName | awk -F "_" '{print $1}'`									# Get 0502XXXX softcode
SaveFile="${SoftID}_save.txt"													# File to save info

#*******************************  end  ******************************

#命令执行函数  参数为：命令  正确返回值,  保存的文件（temp.txt）
REPEAT=3
REPEAT_TIME=1
ret=0

RunCmdCheckResult()
{
    for((n=0;n<=$REPEAT;n++))
    do
        result=`ipmitool raw $1`
		check_ret=$?
		if [ $check_ret -ne 0 ] && [ $n -eq $REPEAT ] ; then
            return 1
		elif [ $check_ret -ne 0 ] && [ $n -ne $REPEAT ]
		then
			continue
        fi
	data1=`echo $result | sed s/[[:space:]]//g`
	data2=`echo $2 | sed s/[[:space:]]//g`
        if [ "$data1" = "$data2" ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    
    return 1
}

RunCmdCheckResult "0x30 0x93 0xdb 0x07 0x00 0x36 0x05a 0x00 0x01 0xFF 0x00 0x00 0xff 0xff 0x00 0x01 0x01 0x04 0x00" "db 07 00 00 04 00 79 01 01" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check liquid cooling mode pass" >> $SaveFile
	echo "Check liquid cooling mode pass"
else
	echo "Check liquid cooling mode failed" >> $SaveFile
	echo "Check liquid cooling mode failed"
	ret=1
fi

echo "done"
exit $ret


