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
    for((n=0;n<$REPEAT;n++))
    do
        result=`ipmitool raw $1`
		data1=`echo $result | sed s/[[:space:]]//g`
		data2=`echo $2 | sed s/[[:space:]]//g`
        if [ $data1 = $data2 ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    
    return 1
}

#check internal use area
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x00 0x00 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check internal use area pass" >> $SaveFile
	echo "Check internal use area pass"
else
	echo "Check internal use area failed" >> $SaveFile
	echo "Check internal use area failed"
	ret=1
fi

#Check chassis info area
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x01 0x00 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check chassis info area Chassis Type pass" >> $SaveFile
	echo "Check chassis info area Chassis Type pass"
else
	echo "Check chassis info area Chassis Type failed" >> $SaveFile
	echo "Check chassis info area Chassis Type failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x01 0x01 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check chassis info area Chassis Part Number pass" >> $SaveFile
	echo "Check chassis info area Chassis Part Number pass"
else
	echo "Check chassis info area Chassis Part Number failed" >> $SaveFile
	echo "Check chassis info area Chassis Part Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x01 0x02 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check chassis info area Chassis Serial Number pass" >> $SaveFile
	echo "Check chassis info area Chassis Serial Number pass"
else
	echo "Check chassis info area Chassis Serial Number failed" >> $SaveFile
	echo "Check chassis info area Chassis Serial Number failed"
	ret=1
fi

#Check specified board info area
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x02 0x01 0x00 0xff" "80 48 75 61 77 65 69" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check board info area Board Manufacturer pass" >> $SaveFile
	echo "Check board info area Board Manufacturer pass"
else
	echo "Check board info area Board Manufacturer failed" >> $SaveFile
	echo "Check board info area Board Manufacturer failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x02 0x04 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check board info area Board Part Number pass" >> $SaveFile
	echo "Check board info area Board Part Number pass"
else
	echo "Check board info area Board Part Number failed" >> $SaveFile
	echo "Check board info area Board Part Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x02 0x05 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check board info area FRU File ID pass" >> $SaveFile
	echo "Check board info area FRU File ID pass"
else
	echo "Check board info area FRU File ID failed" >> $SaveFile
	echo "Check board info area FRU File ID failed"
	ret=1
fi

#Check product info area
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x03 0x00 0x00 0xff" "80 48 75 61 77 65 69" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check product info area Product Manufacturer Name pass" >> $SaveFile
	echo "Check product info area Product Manufacturer Name pass"
else
	echo "Check product info area Product Manufacturer Name failed" >> $SaveFile
	echo "Check product info area Product Manufacturer Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x03 0x01 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check product info area Product Name pass" >> $SaveFile
	echo "Check product info area Product Name pass"
else
	echo "Check product info area Product Name failed" >> $SaveFile
	echo "Check product info area Product Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x03 0x02 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check product info area Product Part/Model Number pass" >> $SaveFile
	echo "Check product info area Product Part/Model Number pass"
else
	echo "Check product info area Product Part/Model Number failed" >> $SaveFile
	echo "Check product info area Product Part/Model Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x03 0x03 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check product info area Product Version pass" >> $SaveFile
	echo "Check product info area Product Version pass"
else
	echo "Check product info area Product Version failed" >> $SaveFile
	echo "Check product info area Product Version failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x03 0x04 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check product info area Product Serial Number pass" >> $SaveFile
	echo "Check product info area Product Serial Number pass"
else
	echo "Check product info area Product Serial Number failed" >> $SaveFile
	echo "Check product info area Product Serial Number failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x03 0x05 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check product info area Asset Tag pass" >> $SaveFile
	echo "Check product info area Asset Tag pass"
else
	echo "Check product info area Asset Tag failed" >> $SaveFile
	echo "Check product info area Asset Tag failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x03 0x06 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check product info area FRU File ID pass" >> $SaveFile
	echo "Check product info area FRU File ID pass"
else
	echo "Check product info area FRU File ID failed" >> $SaveFile
	echo "Check product info area FRU File ID failed"
	ret=1
fi

#Check extend elable
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x05 0x01 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check extend elable pass" >> $SaveFile
	echo "Check extend elable pass"
else
	echo "Check extend elable failed" >> $SaveFile
	echo "Check extend elable failed"
	ret=1
fi

RunCmdCheckResult "0x30 0x90 0x05 0x00 0x07 0x01 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check Internal extend elable pass" >> $SaveFile
	echo "Check Internal extend elable pass"
else
	echo "Check Internal extend elable failed" >> $SaveFile
	echo "Check Internal extend elable failed"
	ret=1
fi

RunCmdCheckResult "0x30 0x90 0x05 0x00 0x08 0x01 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check Chassis extend elable pass" >> $SaveFile
	echo "Check Chassis extend elable pass"
else
	echo "Check Chassis extend elable failed" >> $SaveFile
	echo "Check Chassis extend elable failed"
	ret=1
fi

RunCmdCheckResult "0x30 0x90 0x05 0x00 0x09 0x01 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check Product extend elable pass" >> $SaveFile
	echo "Check Product extend elable pass"
else
	echo "Check Product extend elable failed" >> $SaveFile
	echo "Check Product extend elable failed"
	ret=1
fi

#Check system info area
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x06 0x00 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check system info area System Manufacturer Name pass" >> $SaveFile
	echo "Check system info area System Manufacturer Name pass"
else
	echo "Check system info area System Manufacturer Name failed" >> $SaveFile
	echo "Check system info area System Manufacturer Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x06 0x01 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check system info area System Product Name pass" >> $SaveFile
	echo "Check system info area System Product Name pass"
else
	echo "Check system info area System Product Name failed" >> $SaveFile
	echo "Check system info area System Product Name failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x06 0x02 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check system info area System Version pass" >> $SaveFile
	echo "Check system info area System Version pass"
else
	echo "Check system info area System Version failed" >> $SaveFile
	echo "Check system info area System Version failed"
	ret=1
fi
RunCmdCheckResult "0x30 0x90 0x05 0x00 0x06 0x03 0x00 0xff" "80" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "Check system info area System Serial Number pass" >> $SaveFile
	echo "Check system info area System Serial Number pass"
else
	echo "Check system info area System Serial Number failed" >> $SaveFile
	echo "Check system info area System Serial Number failed"
	ret=1
fi

echo "done"
exit $ret
