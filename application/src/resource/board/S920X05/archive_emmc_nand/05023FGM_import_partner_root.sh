chmod +x *
############################################
# bash script: 导入伙伴根哈希
############################################
#把项目编码赋值好
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`

SAVEFILE=$project_code"_save.txt"

#查看日志文件是否存在
if [ ! -f $SAVEFILE ] ; then
    rm -rf $SAVEFILE
fi 

source ./common_transfer_flie.sh

source ./add_temp_user.sh

#加载驱动
load_driver

#使能DFT
DFT_enable

#升级支持合作伙伴的 efuse
upgrade_function "efuse-crypt-image-partner.hpm"
if [ $? != 0 ]; then
    echo "upgrade efuse fail"
    error_quit
fi

#查看/tmp/partner_pcert.bin文件是否存在
if [ ! -f "./partner_pcert.bin" ] ; then
    echo "./partner_pcert.bin file is not present!"
    echo "./partner_pcert.bin file is not present!" >>$SAVEFILE
    error_quit
fi 

#SCP命令发送伙伴根
send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" ./partner_pcert.bin /tmp/partner_pcert.bin send 
if [ $? != 0 ] ; then	 
    echo "send file fail!"
    echo "send file fail!" >>$SAVEFILE
    error_quit	
fi

# 导入根文件，返回值0x81:导入根成功 0x80:该efuse是旧的efuse  0x00：正在进行根的导入   0xFF：导入根失败
ipmicmd -k "0f 00 30 93 db 07 00 57 00 02 03 00 00 16 2f 74 6d 70 2f 70 61 72 74 6e 65 72 5f 70 63 65 72 74 2e 62 69 6e" smi 0 >temp.txt
# 如果返回81（导入成功）
grep -wq "0f 31 00 93 81" temp.txt
if [ $? == 0 ]; then
    echo_success "import huawei pcert success"
    ok_quit
fi

# 如果返回00（正在导入），会进行os的重启，这里进行sleep 等待（理论上是不会有sleep日志的，因为os会重启）
grep -wq "0f 31 00 93 00" temp.txt
if [ $? == 0 ]; then
    sleep_times=0
    echo_success "wait os restart, wait 10 minutes"
    for ((i=0; i<60; i++))
    do
        sleep_times=$((sleep_times+=10))
        echo "the wait times is $sleep_times second"
        if [ $sleep_times -gt 599 ]; then
            echo_fail "restart os fail, please check os system"
            echo "restart os fail, please check os system" >>$SAVEFILE
            error_quit
        fi
        sleep 10
    done
fi

grep -wq "0f 31 00 93 ff" temp.txt
if [ $? == 0 ]; then
    echo_fail "import huawei pcert fail"
    echo "ipmicmd import root hash failed" >>$SAVEFILE
    error_quit
fi

grep -wq "0f 31 00 93 80" temp.txt
if [ $? == 0 ]; then
    echo_success "it's old efuse"
    echo "it's old efuse" >>$SAVEFILE
    ok_quit
fi    

echo_fail "import back code is error"
cat temp.txt
error_quit
