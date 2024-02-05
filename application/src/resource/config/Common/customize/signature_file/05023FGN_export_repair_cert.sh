chmod +x *
############################################
# bash script: 伙伴维修凭证信息导出0502
############################################
#把项目编码赋值好
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`

SAVEFILE=$project_code"_save.txt"
RINFO_FILE_NAME=partner_rinfo.bin
RINFO_RSA_SIGN_ALG=00000000

#命令执行函数  参数为：命令  正确返回值  保存的文件（temp.txt）
run_cmd()
{
    for((j=0;j<$REPEAT;j++))
    do
        ipmicmd -k "$1" smi 0 >temp.txt
        grep -wq "$2" temp.txt
        if [ $? = 0 ] ; then
            return 0
        fi
        grep -wq "$3" temp.txt
        if [ $? = 0 ] ; then
            return 1
        fi		
        sleep $REPEAT_TIME
    done
    return 2
}

# 判断是否为无efuse或者老efuse环境，无efuse或者老efuse则直接正常退出
judge_efuse()
{
    echo "start get efuse state"
    # 如果返回值为00，则代表不含有efuse
    run_cmd "0f 00 30 93 db 07 00 5b 2c 00" "0f 31 00 93 00 db 07 00 00"
    if [ $? == 0 ]; then
        echo_fail "the environment has no efuse"
        echo "the environment has no efuse" >>$SAVEFILE
        exit 0
    else
        run_cmd "0f 00 30 93 db 07 00 5b 2b 00" "0f 31 00 93 00 db 07 00 00"
        if [ $? == 0 ]; then
            echo_fail "the environment is old efuse"
            echo "the environment is old efuse" >>$SAVEFILE
            exit 0
        fi
    fi
}

source ./common_transfer_flie.sh

source ./add_temp_user.sh

#加载驱动
load_driver

# 判断是否为无efuse环境，无efuse则直接正常退出
judge_efuse

#ipmi命令导出维修凭证
run_cmd "f 0 30 93 db 07 00 57 00 01 04 00 00 16 2f 74 6d 70 2f 70 61 72 74 6e 65 72 5f 72 69 6e 66 6f 2e 62 69 6e" "0f 31 00 93 81" "0f 31 00 93 00 db 07 00"
if [ $? == 0 ] ; then
    echo "No need to delete"
    exit 0
elif[$? != 1]
    echo "ipmicmd export Repair certificate failed"
    echo "ipmicmd export Repair certificate failed" >>$SAVEFILE
    exit 2
fi

#传输文件
send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" /tmp/partner_rinfo.bin ${G_WORK_DIR}/$RINFO_FILE_NAME "get"	 
if [ $? != 0 ] ; then	 
    echo "get file fail!"
    echo "get file fail!" >>$SAVEFILE
    exit 2	
fi

echo -n $RINFO_RSA_SIGN_ALG >> .stage.bin

hex2bin .stage.bin .stage.bin.tmp

cat .stage.bin.tmp >> $RINFO_FILE_NAME

rm .stage.bin*

exit 1
