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
        if [ $data1 = $data2 ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    
    return 1
}

#clear one extend elable func
function ClearOneExtendElable()
{
	i=0
	j=0
	unset req
	expend_resp=`ipmitool raw 0x30 0x90 0x05 0x00 0x05 0x01 0x00 0xFF`
	OLD_IFS="$IFS"
	IFS=" "
	expend_resp=($expend_resp)
	IFS="$OLD_IFS"
	if [ ${#expend_resp[@]} -eq 1 ]
	then
		return 1
	fi
	for mem in ${expend_resp[@]}
	do
		if [ $i -le 0 ]
		then
				i=`expr $i + 1`
				continue
		fi
		req[$j]=0x$mem
		if [ "$mem" = "3d" ] || [ "$mem" = "3a" ] ; then
				break
		fi
		j=`expr $j + 1`
	done
	if [ $j -ne 0 ]
	then
		cmd="0x30 0x90 0x04 0x00 0x05 0x00 0x00 "0x`printf %x ${#req[@]}`" "${req[@]}
		RunCmdCheckResult "$cmd" "" >> $SaveFile
		if [ $? -ne 0 ]
		then
			return 2
		fi
	fi
	return 0
}

#clear product one extend elable func
function ClearProductOneExtendElable()
{
	i=0
	j=0
	unset req
	expend_resp=`ipmitool raw 0x30 0x90 0x05 0x00 0x09 0x01 0x00 0xFF`
	OLD_IFS="$IFS"
	IFS=" "
	expend_resp=($expend_resp)
	IFS="$OLD_IFS"
	if [ ${#expend_resp[@]} -eq 1 ]
	then
		return 1
	fi
	for mem in ${expend_resp[@]}
	do
		if [ $i -le 0 ]
		then
				i=`expr $i + 1`
				continue
		fi
		req[$j]=0x$mem
		if [ "$mem" = "3d" ] || [ "$mem" = "3a" ] ; then
				break
		fi
		j=`expr $j + 1`
	done
	if [ $j -ne 0 ]
	then
		cmd="0x30 0x90 0x04 0x00 0x03 0x07 0x00 "0x`printf %x ${#req[@]}`" "${req[@]}
		RunCmdCheckResult "$cmd" "" >> $SaveFile
		if [ $? -ne 0 ]
		then
			return 2
		fi
	fi
	return 0
}

#clear Chassis One extend elable func
function ClearChassisOneExtendElable()
{
	i=0
	j=0
	unset req
	expend_resp=`ipmitool raw 0x30 0x90 0x05 0x00 0x08 0x01 0x00 0xFF`
	OLD_IFS="$IFS"
	IFS=" "
	expend_resp=($expend_resp)
	IFS="$OLD_IFS"
	if [ ${#expend_resp[@]} -eq 1 ]
	then
		return 1
	fi
	for mem in ${expend_resp[@]}
	do
		if [ $i -le 0 ]
		then
				i=`expr $i + 1`
				continue
		fi
		req[$j]=0x$mem
		if [ "$mem" = "3d" ] || [ "$mem" = "3a" ] ; then
				break
		fi
		j=`expr $j + 1`
	done
	if [ $j -ne 0 ]
	then
		cmd="0x30 0x90 0x04 0x00 0x01 0x03 0x00 "0x`printf %x ${#req[@]}`" "${req[@]}
		RunCmdCheckResult "$cmd" "" >> $SaveFile
		if [ $? -ne 0 ]
		then
			return 2
		fi
	fi
	return 0
}

#clear Internal One extend elable func
function ClearInternalOneExtendElable()
{
	i=0
	j=0
	unset req
	expend_resp=`ipmitool raw 0x30 0x90 0x05 0x00 0x07 0x01 0x00 0xFF`
	OLD_IFS="$IFS"
	IFS=" "
	expend_resp=($expend_resp)
	IFS="$OLD_IFS"
	if [ ${#expend_resp[@]} -eq 1 ]
	then
		return 1
	fi
	for mem in ${expend_resp[@]}
	do
		if [ $i -le 0 ]
		then
				i=`expr $i + 1`
				continue
		fi
		req[$j]=0x$mem
		if [ "$mem" = "3d" ] || [ "$mem" = "3a" ] ; then
				break
		fi
		j=`expr $j + 1`
	done
	if [ $j -ne 0 ]
	then
		cmd="0x30 0x90 0x04 0x00 0x00 0x01 0x00 "0x`printf %x ${#req[@]}`" "${req[@]}
		RunCmdCheckResult "$cmd" "" >> $SaveFile
		if [ $? -ne 0 ]
		then
			return 2
		fi
	fi
	return 0
}

#clear all extend elable
function ClearExtendElable()
{
	for m in `seq 1 10`
	do
		ClearOneExtendElable
		ret_temp=$?
		if [ $ret_temp -eq 1 ]
		then
			return 0
		elif [ $ret_temp -eq 2 ]
		then
			return 1
		fi
	done
}

#clear all product extend elable
function ClearProductExtendElable()
{
	for m in `seq 1 10`
	do
		ClearProductOneExtendElable
		ret_temp=$?
		if [ $ret_temp -eq 1 ]
		then
			return 0
		elif [ $ret_temp -eq 2 ]
		then
			return 1
		fi
	done
}

#clear all Chassis extend elable
function ClearChassisExtendElable()
{
	for m in `seq 1 10`
	do
		ClearChassisOneExtendElable
		ret_temp=$?
		if [ $ret_temp -eq 1 ]
		then
			return 0
		elif [ $ret_temp -eq 2 ]
		then
			return 1
		fi
	done
}

#clear all Internal extend elable
function ClearInternalExtendElable()
{
	for m in `seq 1 10`
	do
		ClearInternalOneExtendElable
		ret_temp=$?
		if [ $ret_temp -eq 1 ]
		then
			return 0
		elif [ $ret_temp -eq 2 ]
		then
			return 1
		fi
	done
}

#clear result file
echo "" > $SaveFile
#clear internal use area
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x00 0x00 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear internal use area pass" >> $SaveFile
	echo "Clear internal use area pass"
else
	echo "Clear internal use area failed" >> $SaveFile
	echo "Clear internal use area failed"
	ret=1
fi

#clear chassis info area
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x01 0x00 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear chassis info area Chassis Type pass" >> $SaveFile
	echo "Clear chassis info area Chassis Type pass"
else
	echo "Clear chassis info area Chassis Type failed" >> $SaveFile
	echo "Clear chassis info area Chassis Type failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x01 0x01 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear chassis info area Chassis Part Number pass" >> $SaveFile
	echo "Clear chassis info area Chassis Part Number pass"
else
	echo "Clear chassis info area Chassis Part Number failed" >> $SaveFile
	echo "Clear chassis info area Chassis Part Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x01 0x02 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear chassis info area Chassis Serial Number pass" >> $SaveFile
	echo "Clear chassis info area Chassis Serial Number pass"
else
	echo "Clear chassis info area Chassis Serial Number failed" >> $SaveFile
	echo "Clear chassis info area Chassis Serial Number failed"
	ret=1
fi

#clear specified board info area
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x02 0x01 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear board info area Board Manufacturer pass" >> $SaveFile
	echo "Clear board info area Board Manufacturer pass"
else
	echo "Clear board info area Board Manufacturer failed" >> $SaveFile
	echo "Clear board info area Board Manufacturer failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x02 0x04 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear board info area Board Part Number pass" >> $SaveFile
	echo "Clear board info area Board Part Number pass"
else
	echo "Clear board info area Board Part Number failed" >> $SaveFile
	echo "Clear board info area Board Part Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x02 0x05 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear board info area FRU File ID pass" >> $SaveFile
	echo "Clear board info area FRU File ID pass"
else
	echo "Clear board info area FRU File ID failed" >> $SaveFile
	echo "Clear board info area FRU File ID failed"
	ret=1
fi

#clear product info area
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x03 0x00 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear product info area Product Manufacturer Name pass" >> $SaveFile
	echo "Clear product info area Product Manufacturer Name pass"
else
	echo "Clear product info area Product Manufacturer Name failed" >> $SaveFile
	echo "Clear product info area Product Manufacturer Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x03 0x01 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear product info area Product Name pass" >> $SaveFile
	echo "Clear product info area Product Name pass"
else
	echo "Clear product info area Product Name failed" >> $SaveFile
	echo "Clear product info area Product Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x03 0x02 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear product info area Product Part/Model Number pass" >> $SaveFile
	echo "Clear product info area Product Part/Model Number pass"
else
	echo "Clear product info area Product Part/Model Number failed" >> $SaveFile
	echo "Clear product info area Product Part/Model Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x03 0x03 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear product info area Product Version pass" >> $SaveFile
	echo "Clear product info area Product Version pass"
else
	echo "Clear product info area Product Version failed" >> $SaveFile
	echo "Clear product info area Product Version failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x03 0x04 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear product info area Product Serial Number pass" >> $SaveFile
	echo "Clear product info area Product Serial Number pass"
else
	echo "Clear product info area Product Serial Number failed" >> $SaveFile
	echo "Clear product info area Product Serial Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x03 0x05 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear product info area Asset Tag pass" >> $SaveFile
	echo "Clear product info area Asset Tag pass"
else
	echo "Clear product info area Asset Tag failed" >> $SaveFile
	echo "Clear product info area Asset Tag failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x03 0x06 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear product info area FRU File ID pass" >> $SaveFile
	echo "Clear product info area FRU File ID pass"
else
	echo "Clear product info area FRU File ID failed" >> $SaveFile
	echo "Clear product info area FRU File ID failed"
	ret=1
fi

#clear extend elable
ClearExtendElable
if [ $? -eq 0 ]
then
	echo "Clear extend elable pass" >> $SaveFile
	echo "Clear extend elable pass"
else
	echo "Clear extend elable failed" >> $SaveFile
	echo "Clear extend elable failed"
	ret=1
fi

#clear prouduct extend elable
ClearProductExtendElable
if [ $? -eq 0 ]
then
	echo "Clear Product extend elable pass" >> $SaveFile
	echo "Clear Product extend elable pass"
else
	echo "Clear Product extend elable failed" >> $SaveFile
	echo "Clear Product extend elable failed"
	ret=1
fi

#clear Chassis extend elable
ClearChassisExtendElable
if [ $? -eq 0 ]
then
	echo "Clear Chassis extend elable pass" >> $SaveFile
	echo "Clear Chassis extend elable pass"
else
	echo "Clear Chassis extend elable failed" >> $SaveFile
	echo "Clear Chassis extend elable failed"
	ret=1
fi

#clear Internal extend elable
ClearInternalExtendElable
if [ $? -eq 0 ]
then
	echo "Clear Internal extend elable pass" >> $SaveFile
	echo "Clear Internal extend elable pass"
else
	echo "Clear Internal extend elable failed" >> $SaveFile
	echo "Clear Internal extend elable failed"
	ret=1
fi

#clear system info area
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x06 0x00 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear system info area System Manufacturer Name pass" >> $SaveFile
	echo "Clear system info area System Manufacturer Name pass"
else
	echo "Clear system info area System Manufacturer Name failed" >> $SaveFile
	echo "Clear system info area System Manufacturer Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x06 0x01 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear system info area System Product Name pass" >> $SaveFile
	echo "Clear system info area System Product Name pass"
else
	echo "Clear system info area System Product Name failed" >> $SaveFile
	echo "Clear system info area System Product Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x06 0x02 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear system info area System Version pass" >> $SaveFile
	echo "Clear system info area System Version pass"
else
	echo "Clear system info area System Version failed" >> $SaveFile
	echo "Clear system info area System Version failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x04 0x00 0x06 0x03 0x00 0x00" "" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Clear system info area System Serial Number pass" >> $SaveFile
	echo "Clear system info area System Serial Number pass"
else
	echo "Clear system info area System Serial Number failed" >> $SaveFile
	echo "Clear system info area System Serial Number failed"
	ret=1
fi

#update elabel data
RunCmdCheckResult "0x30 0x90 0x06 0x00 0xAA" "00" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "update elabel data pass" >> $SaveFile
	echo "update elabel data pass"
else
	echo "update elabel data failed" >> $SaveFile
	echo "update elabel data failed"
	ret=1
fi

if [ $ret -ne 0 ]
then
	echo "clear elabel data failed" >> $SaveFile
	echo "clear elabel data failed"
	exit $ret
fi
echo "wait to be effective"
sleep 40

echo "done"

chmod +x $SoftID"_checkfru.sh"
./$SoftID"_checkfru.sh"
exit $?
