#!/bin/sh
REPEAT=6
REPEAT_TIME=1 
IPMI_CMD_LOG_FILE=custom.log
IPMI_CMD_RETURN_FILE=ipmi_cmd_return_string.txt
SAVEFILE="while_branding_save.txt"
TMP_BIN=temp.bin
declare -A FILE_TYPE
FILE_TYPE=([logo]="0x1f" [favicon]="0x20" [code]="0x21" [web]="0x22" [sensor]="0x23" [style]="0x25" [login]="0x26" [llogo]="0x27" [img]="0x28" [hlogo]="0x29")
MAX_STRING_LEN=224
SEND_DATA_FILE="ipmi_send_data.txt"
SEND_LINE_COUNT=0
SEND_INDEX=0
FILE_SIZE=0
# ret_string 用于存放获取厂商id的返回字符串
ret_string=0
manu_id_string=0
manu_id_hex_string=0

error_quit()
{
    exit 1
}
ok_quit()
{
    exit 0
}
echo_fail()
{
    echo -e "\033[31;1m$1\033[0m" | tee -a $SAVEFILE
}
echo_success()
{
    echo -e "\033[32;1m$1\033[0m" | tee -a $SAVEFILE
}
#去掉前后空格
remove_space()
{
    local covert_string="$1"
    covert_string=`echo "$covert_string" | sed 's/^[ ]*//g' | sed 's/[ ]*$//g'`
    echo "$covert_string"
}
#将id转化成string
word2string()
{
    local LONGB0=`printf "%08x" $1 | cut -b 7-8`
    local LONGB1=`printf "%08x" $1 | cut -b 5-6`
    local LONGB2=`printf "%08x" $1 | cut -b 3-4`
    local LONGB3=`printf "%08x" $1 | cut -b 1-2`
    local id_string="0x$LONGB3 0x$LONGB2 0x$LONGB1 0x$LONGB0"
    echo "$id_string"
}
#将字符串转化为16进制
string2hex()
{
    STRING=$1
    echo $STRING > temp.txt
    #对于字符串STRING最大长度为255做判断
    if [ "${#STRING}" -gt 255 ] ; then
        echo_fail "Surpasses the greatest length limit 255!"
        error_quit
    fi
    #转化字符串为16进制字符串
    ./conver temp.txt 0 1 > temp_tmp.txt
    if [ $? != 0 ] ; then
        echo_fail "conver $STRING Fail"
        error_quit
    fi
    TMP_STRING=`cat temp_tmp.txt`
    TMP_STRING=$(remove_space "$TMP_STRING")
    echo "$TMP_STRING"
}
converstring2hex()
{
    STRING=$1
    echo $STRING >temp.txt
    #转化字符串为16进制字符串
    ./conver temp.txt 0 1 >temp_tmp.txt
    if [ $? != 0 ] ; then
        echo_fail  "conver $STRING Fail"
        error_quit
    fi
    TMP_STRING=`cat temp_tmp.txt`
}

# 获取自定义厂商id
get_manu_id()
{
	ret_string=`ipmitool raw 0x30 0x90 0x22 0x08`
	if [ $? != 0 ] ; then
		return 1
	fi
	
	# 12 是获取厂商id返回字符串的长度(包括结束符), 比如 "11 db 07 00"
	if [ ${#ret_string} != 12 ] ; then
		# 失败退出,返回1
		return 1
	fi
	
	i=0
	for element in $ret_string
	do
		i=$[$i+1]
		ipmi_array[$i]=$element
	done
	
	# ipmi_array[1] 是ipmi版本, ipmi_array[2] ipmi_array[3] ipmi_array[4] 是厂商id
	manu_id_string="${ipmi_array[2]} ${ipmi_array[3]} ${ipmi_array[4]}"
	manu_id_hex_string="0x${ipmi_array[2]} 0x${ipmi_array[3]} 0x${ipmi_array[4]}"
	return 0
	
}

#使用ipmitool命令执行函数  参数为：命令  正确返回值
run_ipmitool()
{
    local i
    echo "ipmitool $1 : $2" >>$IPMI_CMD_LOG_FILE
    for((i=0;i<$REPEAT;i++))
    do
        ipmitool $1 > $IPMI_CMD_RETURN_FILE
        #ipmitool 返回值超过16字节时换行，合并为一行
        echo `cat $IPMI_CMD_RETURN_FILE | sed 's/^//g'` > $IPMI_CMD_RETURN_FILE
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

#命令执行函数  参数为：文件类型
write_file_data()
{
    local file_type_hex=${FILE_TYPE[$1]}
    local send_prop_data_len=0
    local offset=0
    local offset_s=0
    tmp_line_count=0
    cur_line=""

    while read line
    do
        tmp_line_count=$((tmp_line_count+=1))
        echo 
        echo "the tmp_line count is $tmp_line_count"
        echo "the send line is $SEND_LINE_COUNT"
        if [ $tmp_line_count == $SEND_LINE_COUNT ]; then
            SEND_INDEX=`expr $FILE_SIZE + 224 - $SEND_LINE_COUNT \* 224`
            send_prop_data_len=`printf "0x%02x" $SEND_INDEX`
        else
            send_prop_data_len=`printf "0x%02x" $MAX_STRING_LEN`
        fi
        cur_line="$line"
        offset_s=$(word2string $offset)
        run_ipmitool "raw 0x30 0x93 $manu_id_hex_string 0x69 $file_type_hex 0x00 0x01 $send_prop_data_len $offset_s $cur_line" "$manu_id_string"
        if [ $? != 0 ] ; then
                return 1
        fi
        if [ $tmp_line_count == $SEND_LINE_COUNT ]; then
            offset=$((offset+=$SEND_INDEX))
            if [ $offset != $FILE_SIZE ]; then
                echo "write_file_data with i : $i and FILE_SIZE: $FILE_SIZE"
                error_quit
            fi
            return 0
        fi
        offset=$((offset+=$MAX_STRING_LEN))
    done < $SEND_DATA_FILE
}
upload_file_prepare()
{
    #计算文件大小
    FILE_SIZE=`wc -c $2 | awk '{print $1}'`
    local offset_s=$(word2string $FILE_SIZE)
    #获取文件类型
    local file_type_hex=${FILE_TYPE[$1]}
    run_ipmitool "raw 0x30 0x93 $manu_id_hex_string 0x69 $file_type_hex 0x00 0x00 0x00 $offset_s" "$manu_id_string"
    if [ $? != 0 ] ; then
        return 1
    fi
    return 0
}
make_file_data()
{
    rm -rf "$SEND_DATA_FILE"
    local array=`cat $TMP_BIN`
    index_num=0
    middle_string=""
    sub_string=""

    for index in ${array[@]}
    do
        hex_index="0x$index"
        index_num=$((index_num+=1))
        echo "the index_num is $index_num"
        if [ $SEND_INDEX -ge $MAX_STRING_LEN ] || [ $index_num == $FILE_SIZE ]; then
            if [ $index_num == $FILE_SIZE ]; then
                if [ $SEND_INDEX == 0 ]; then
                    sub_string="$hex_index"
                else
                    middle_string="$sub_string"
                    sub_string="$middle_string $hex_index"
                fi
            fi
            SEND_LINE_COUNT=$((SEND_LINE_COUNT+=1))
            echo "$sub_string" >> "$SEND_DATA_FILE"
            sub_string=""
            if [ $index == FILE_SIZE ]; then
                SEND_INDEX=$((SEND_INDEX+=1))
                return 0
            fi
            SEND_INDEX=0
        fi

        if [ $SEND_INDEX -lt $MAX_STRING_LEN ]; then
            SEND_INDEX=$((SEND_INDEX+=1))
            middle_string="$sub_string"
            if [ $SEND_INDEX == 1 ]; then
                sub_string="$hex_index"
            else
                sub_string="$middle_string $hex_index"
            fi
        fi
    done
}

upload_file_data()
{
    write_file_data "$1"
}
upload_file_finish()
{
    local file_type_hex=${FILE_TYPE[$1]}
    local check_sum=`sha256sum $2 | awk '{print $1}'`
    local check_sum_hex=$(string2hex "$check_sum")
    check_sum_hex=`echo "$check_sum_hex" >tmp_check_sum.txt`
    sed -i 's/^/0x/g' tmp_check_sum.txt
    sed -i 's/ / 0x/g' tmp_check_sum.txt
    check_sum_hex=`cat tmp_check_sum.txt`
    local check_sum_len=`echo ${#check_sum}`
    local check_sum_len_hex=`printf "0x%02x" $check_sum_len`
    run_ipmitool "raw 0x30 0x93 $manu_id_hex_string 0x69 $file_type_hex 0x00 0x03 $check_sum_len_hex 0x00 0x00 0x00 $check_sum_len_hex $check_sum_hex" "$manu_id_string"
    if [ $? != 0 ] ; then
        return 1
    fi
    return 0
}
file_to_bin()
{
    local file_name=$1
    hexdump -v -e '16/1 "%02X " " "' $file_name > $TMP_BIN
}
# 入参1判空 上传类型宏
if [ -z "$1" ] ; then
    echo_fail "\$1 is not exist!"
    error_quit
fi
# 入参2判空 文件名
if [ -z "$2" ] ; then
    echo_fail "\$2 is not exist!"
    error_quit
fi

# 入参2不为文件,则报异常
if [ ! -f "$2" ]; then
    echo_fail "$2 file is not exist!"
    error_quit
fi
file_to_bin "$2"
get_manu_id
if [ $? != 0 ] ; then
    echo_fail "get manu id fail!"
    error_quit
fi

upload_file_prepare "$1" "$2"
if [ $? != 0 ] ; then
    echo_fail "upload $2 file prepare fail!"
    error_quit
fi
make_file_data
echo_success "upload $2 file prepare success!"
upload_file_data "$1"
if [ $? != 0 ] ; then
    echo_fail "upload $2 file data fail!"
    error_quit
fi
echo_success "upload $2 file data success!"
upload_file_finish "$1" "$2"
if [ $? != 0 ] ; then
    echo_fail "upload $2 file finish fail!"
    error_quit
fi
echo_success "upload $2 file finish success!"
