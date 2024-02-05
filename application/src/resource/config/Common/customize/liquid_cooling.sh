#!/bin/bash


#******************  default info cann't be changed  ****************
TuName=`echo $0 | awk -F "/" '{print $NF}' | awk -F "." '{print $1}'`			# Get script name
SoftID=`echo $TuName | awk -F "_" '{print $1}'`									# Get 0502XXXX softcode
SaveFile="${SoftID}_save.txt"													# File to save info

#*******************************  end  ******************************

#命令执行函数  参数为：命令  正确返回值,  保存的文件（temp.txt）
REPEAT=3
REPEAT_TIME=1

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

SmmRunCmdCheckResult()
{
    for((n=0;n<=$REPEAT;n++))
    do
        result=`ipmitool -b 8 -t 0x20 -m 0x21 raw $1`
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

#命令执行函数  参数为：命令  正确返回值  保存的文件（temp.txt）
run_cmd()
{
    for((j=0;j<$REPEAT;j++))
    do
        ipmitool raw $1 >temp.txt
        grep -wq "$2" temp.txt
        if [ $? = 0 ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    
    return 1
}

function GetSystemProductName()
{
    run_cmd  "0x30 0x90 0x05 0x00 0x06 0x01 0x00 0x80"  ""
    if [ $? != 0 ] ; then
        echo "get system product name fail!"
        echo "get system product name fail!" >>$SAVEFILE
        error_quit
    fi
	return 0
}

function SetSystemProductName()
{
    declare -a bytes
    bytes=($(cat temp.txt))
    ipmi_cmd_pre='0x30 0x90 0x04 0x00 0x06 0x01 0x00'
    length=$((${#bytes[@]}-1))

    if [ ${length} -ge 48 ]
    then
        echo "length error ${length}"
		return 1
    fi

    data="${ipmi_cmd_pre} ${length}"

    for(( i=1; i<=$length; i++))
	do
		data="${data} 0x${bytes[$i]}"
	done

	RunCmdCheckResult "${data}" "" >> $SaveFile
	return 0
}

function CheckBlade()
{
    run_cmd  "0X06 0X01"  "db 07 00"
    if [ $? != 0 ] ; then
        echo "get device ID fail!"
        echo "get device ID fail!" >>$SAVEFILE
        error_quit
    else 
        temp=`cat  temp.txt | awk '{print $10}' | sed s/[[:space:]]//g `
		if [ $temp = "04" ] ; then		     
            return 0
        fi
		
        return 1
    fi
}

#update liquid cooling mode 设置液冷模式
RunCmdCheckResult "0x30 0x93 0xdb 0x07 0x00 0x35 0x05a 0x00 0x01 0x00 0x00 0x00 0xff 0xff 0x00 0x01 0x0 0x04 0x00 0x0 0x01 0x01" "db 07 00" >> $SaveFile
if [ $? -eq 0 ]
then
	echo "update liquid cooling mode pass" >> $SaveFile
	echo "update liquid cooling mode pass"
else
	echo "update liquid cooling mode failed" >> $SaveFile
	echo "update liquid cooling mode failed"
	exit 1
fi


# 刀片要透传设置液冷标志到管理板
CheckBlade
if [ $? -eq 0 ]
then
	SmmRunCmdCheckResult "0x30 0x93 0xdb 0x07 0x00 0x35 0x05a 0x00 0x01 0x00 0x00 0x00 0xff 0xff 0x00 0x01 0x0 0x04 0x00 0x0 0x01 0x01" "db 07 00" >> $SaveFile
	if [ $? -eq 0 ]
	then
		echo "update liquid cooling mode to smm pass" >> $SaveFile
		echo "update liquid cooling mode to smm pass"
	else
		echo "update liquid cooling mode to smm failed" >> $SaveFile
		echo "update liquid cooling mode to smm failed"
		exit 1
	fi
fi

# 等液冷模式设置后，产品名称更新
echo "wait to be effective"
sleep 10

# 获取产品名称，在设置，因为没有定制化时，会更新为默认产品名称 
GetSystemProductName
if [ $? -eq 0 ]
then
    SetSystemProductName
fi

if [ $? -eq 0 ]
then
	echo "update system product name pass" >> $SaveFile
	echo "update system product name pass"
else
	echo "update system product name failed" >> $SaveFile
	echo "update system product name failed"
	exit 1
fi

#获取产品的平台ID和Board ID的组合
PRODUCT_BOARD_ID=0
get_product_board_id()
{
	PRODUCT_BOARD_ID=0
	run_cmd  "0x06 0x01" "db 07 00"
    if [ $? != 0 ] ; then
        echo "get device ID fail!"
        echo "get device ID fail!" >> $SaveFile
        error_quit
    else
		tmp_ret=`cat temp.txt`
		PRODUCT_BOARD_ID=`echo ${tmp_ret: -18 : 12} | sed s/[[:space:]]//g`
		
		echo "PRODUCT_BOARD_ID: $PRODUCT_BOARD_ID"
    fi
}

#复位BMC
reset_bmc()
{
    echo "please wait for 6 second and reset bmc!"
    for((i=0;i<6;i++))
    do
	    echo -n -e "\b\b$i"
	    sleep 1
    done

    for((i=0;i<3;i++))
    do
        ipmicmd -k "f 0 06 02" smi 0 > temp.txt
        grep -wq "0f 07 00 02 00" temp.txt
        if [ $? -ne 0 ] ; then
            sleep 1
            continue
        fi
        break
    done

    if [ $i -eq 3 ] ; then
        echo "BMC reset test Fail" >> $SaveFile
        return 1
    fi

    echo "please wait for 180 second and reset bmc!"
    for((i=0;i<180;i++))
    do
	    echo -n -e "\b\b$i"
	    sleep 1
    done

    for((i=0;i<20;i++))
    {
        run_cmd  "0x06 0x01" "db 07 00"
        if [ $? -ne 0 ] ; then
            echo "please wait for 10 second!"
            sleep 10
        else 
            break
        fi
    }

    if [ $i -eq 20 ] ; then
        echo "BMC reset test Fail" >> $SaveFile
        return 1
    fi

    echo "BMC reset ok" >> $SaveFile
}

#以下为设置液冷后需要立即重启BMC生效的机型
FusionPoD_DA120V2_ID=0700b100
FusionPoD_DH120V5_ID=01000100

#由于FusionPoD节点DA120 V2、DH120 V5与其液冷机型DH120C V2、DH120C V5配置的风扇个数不同（风冷7个，液冷5个）
#因此设置液冷标志后需要立即复位BMC生效液冷，否则会出现风扇冗余丢失的告警
get_product_board_id
if [ $? == 0 ] ; then
    #只对DA120 V2和DH120 V5执行立即复位BMC，其他液冷机型保持原有策略不变
    if [ $PRODUCT_BOARD_ID == $FusionPoD_DA120V2_ID ] || [ $PRODUCT_BOARD_ID == $FusionPoD_DH120V5_ID ] ; then
        reset_bmc
        if [ $? != 0 ] ; then
            exit 1
        fi
    fi
fi

echo "done"
exit 0


