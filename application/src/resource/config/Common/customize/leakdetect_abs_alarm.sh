#!/bin/bash

#*************************  default info cann't be changed **************************
# 设置漏液检测卡不在位告警
project_code=`echo $0 | awk -F "_" '{printf $1}'`                                       # 获取脚本名称
project_code=`echo $project_code | awk -F "/" '{printf $2}'`                            # 获取0502编码

SAVEFILE=$project_code"_save.txt"                                                       # 保存日志
RESULTFILE=$project_code"_result.txt"                                                   # 保存结果
#**************************************  end  ***************************************

err_parse(){
	local code=$1
	if [ $code="c9" ];then
		echo "ERROR_CODE(0xC9): Param out of range."
	elif [ $code="80" ];then
		echo "ERROR_CODE(0x80): Data not available."
	elif [ $code="ce" ];then
		echo "ERROR_CODE(0xCE): Cannot Response."
	else
		echo "ERROR_CODE(0x${code}): Unknown error."
	fi
}

# 定制漏液检测卡不在位告警关闭
set_ret="0f 31 00 93 00 db 07 00 00"
get_ret="0f 31 00 93 00 db 07 00 00"
summary=0
echo "LOG:">$SAVEFILE

for ((i=0;i<3;i++))
do
	ipmicmd -k "0f 00 30 93 db 07 00 5a 26 00 02 00 03 00" smi 0 > $RESULTFILE
	cat $RESULTFILE | grep "${set_ret}" >/dev/null
	if (( $?!=0 )); then
		err_code=`cat $RESULTFILE | awk -F " " '{printf $5}'`
		parse=$(err_parse "${err_code}")
		echo $parse
		echo $parse>>$SAVEFILE
		sleep 1
		continue
	fi
	break
done


ipmicmd -k "0f 00 30 93 db 07 00 5b 26 00 01 00 03" smi 0 > $RESULTFILE
cat $RESULTFILE | grep "${get_ret}" >/dev/null
if (( $?!=0 )); then
	err_code=`cat $RESULTFILE | awk -F " " '{printf $5}'`
	parse=$(err_parse "${err_code}")
	echo $parse
	echo $parse>>$SAVEFILE
	summary=1
fi

if (( summary==0 )); then
	echo "Success - "$0
	echo "Success - "$0>>$SAVEFILE
	exit 0
else
	echo "Failure in "$0
	echo "Failure in "$0>>$SAVEFILE
	exit 1
fi