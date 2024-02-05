#!/bin/bash


#命令失败时的重试次数 和间隔 
REPEAT=6
REPEAT_TIME=1 

# ipmi 命令前缀
ipmi_cmd_pre='0x30 0x92 0xdb 0x07 0x00 0x09 0x00 0x1e'


error_quit()
{
    exit 1
}

ok_quit()
{
    exit 0
}

#命令执行函数  参数为：命令  
run_cmd()
{
    for((j=0;j<$REPEAT;j++))
    do
        ipmitool raw $1 >/dev/null 2>&1
        if [ $? = 0 ] ; then
            return 0
        fi
        sleep $REPEAT_TIME
    done
    
    return 1
}

# 检查输入参数,需要带一个文件名作为输入参数
if [ $# != 1 ]
then
    echo "usage: $0 cmes_file_name"
    error_quit
fi
cems_file_name=$1


load_cmes_to_bmc()
{
    # 检查文件是否存在
    if [ ! -f "$cems_file_name" ] ; then
        echo $cems_file_name ' file is not present!'
        error_quit
    fi

    declare -a bytes
    bytes=(`xxd -ps -c 1 $cems_file_name`)

    length=(`stat -c %s  $cems_file_name`)
    # 发送write prepare命令
    prepare_cmd=`printf '0x%02x 0x%02x 0x%02x 0x%02x'  $(($length & 255))   $((($length >>8)& 255)) $((($length >>16)& 255)) $((($length >>24)& 255))`
    prepare_cmd="$ipmi_cmd_pre 0x00 0x00 ${prepare_cmd}"
    run_cmd "$prepare_cmd" 
    if [ $? != 0 ] ; then
        echo 'write prepare error ' $prepare_cmd
        error_quit
    fi

    # 发送write data命令, 当文件长度过大时，需要分帧传送
    offset=0
    data_checksum=0
    PACKAGE_MAX=200
    while [  $offset -lt $length ]; do  
        next_offset=$(($offset+$PACKAGE_MAX))

        if [ $next_offset -gt $length ]
        then
            package_length=$(($length - $offset))
        else
            package_length=$PACKAGE_MAX
        fi

        data_pre="$ipmi_cmd_pre 0x01"
        checksum=0
        data=""
        for(( i=0; i<$package_length; i++))
        do
            data="$data 0x${bytes[(($i+$offset))]}"
            checksum=$(($checksum + 0x${bytes[(($i+$offset))]}))
            checksum=$(($checksum & 255))
        done
        checksum=`printf '0x%02x' $checksum`
        date_offset=`printf ' 0x%02x 0x%02x 0x%02x 0x%02x'    $(($offset & 255))   $((($offset >>8)& 255)) $((($offset >>16)& 255)) $((($offset >>24)& 255))`
        data_cmd="$data_pre ${checksum} $date_offset $data"
        run_cmd "$data_cmd"
        if [ $? != 0 ] ; then
            echo 'write data error ' $data_cmd
            error_quit
        fi
        let offset=$offset+$package_length   
    done  


    total=0;
    for(( i=0; i<${#bytes[@]}; i++))
    do
        total=$(($total + 0x${bytes[i]}))
        total=$(($total & 255))
    done

    # 发送write finish命令
    checksum=`printf '0x%02x' $total`
    finish_cmd="$ipmi_cmd_pre 0x03 ${checksum}"

    run_cmd "$finish_cmd" 
    if [ $? != 0 ] ; then
        echo 'write finish error ' $finish_cmd
        error_quit
    fi
}

load_cmes_to_bmc
echo 'success'
ok_quit
