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

# 查看 partner_pcert_fvbv_s1.bin 文件是否存在
if [ ! -f "./partner_pcert_fvbv_s1.bin" ] ; then
    echo "./partner_pcert_fvbv_s1.bin file is not present!"
    echo "./partner_pcert_fvbv_s1.bin file is not present!" >>$SAVEFILE
    error_quit
fi

#查看partner_pcert_fvbv_s2.bin文件是否存在
if [ ! -f "./partner_pcert_fvbv_s2.bin" ] ; then
    echo "./partner_pcert_fvbv_s2.bin file is not present!"
    echo "./partner_pcert_fvbv_s2.bin file is not present!" >>$SAVEFILE
    error_quit
fi


return_value_check()
{
	# 如果返回00（导入成功）
	grep -wq "0f 31 00 93 00" temp.txt
	if [ $? == 0 ]; then
	    echo_success "import huawei pcert success"
	    return 0
	fi


	grep -wq "0f 31 00 93 ff" temp.txt
	if [ $? == 0 ]; then
	    echo_fail "import huawei pcert fail"
	    echo "ipmicmd import root hash failed" >>$SAVEFILE
	    error_quit
	fi

    # 老efuse
	grep -wq "0f 31 00 93 80" temp.txt
	if [ $? == 0 ]; then
	    echo_fail "it's old efuse"
	    echo "it's old efuse" >>$SAVEFILE
	    error_quit
	fi

    #  S1和S2已经导入
	grep -wq "0f 31 00 93 81" temp.txt
	if [ $? == 0 ]; then
	    echo_success "have been import s1 and s2 partner pcert"
	    echo "have been import s1 and s2 partner pcert" >>$SAVEFILE
	    ok_quit
	fi

    #  S1已经导入
	grep -wq "0f 31 00 93 83" temp.txt
	if [ $? == 0 ]; then
	    echo_success "have been import s1 partner pcert"
	    echo "have been import s1 partner pcert" >>$SAVEFILE
	    return 0
	fi

    #  S2已经导入
	grep -wq "0f 31 00 93 84" temp.txt
	if [ $? == 0 ]; then
	    echo_success "have been import s2 partner pcert"
	    echo "have been import s2 partner pcert" >>$SAVEFILE
	    return 0
	fi
    
    echo_fail "the import pcert return value is unknow"
    cat temp.txt
    error_quit
}

# SCP命令发送 s1 伙伴根
send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" ./partner_pcert_fvbv_s1.bin /tmp/partner_pcert.bin send 
if [ $? != 0 ] ; then
    echo "send file fail!"
    echo "send file fail!" >>$SAVEFILE
    error_quit	
fi
# 导入s1华为根文件
ipmicmd -k "0f 00 30 93 db 07 00 57 00 02 03 00 00 16 2f 74 6d 70 2f 70 61 72 74 6e 65 72 5f 70 63 65 72 74 2e 62 69 6e" smi 0 >temp.txt
return_value_check
# SCP命令发送 s2 伙伴根
send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" ./partner_pcert_fvbv_s2.bin /tmp/partner_pcert.bin send 
if [ $? != 0 ] ; then
    echo "send file fail!"
    echo "send file fail!" >>$SAVEFILE
    error_quit	
fi
# 导入s2华为根文件
ipmicmd -k "0f 00 30 93 db 07 00 57 00 02 03 00 00 16 2f 74 6d 70 2f 70 61 72 74 6e 65 72 5f 70 63 65 72 74 2e 62 69 6e" smi 0 >temp.txt
return_value_check

# 使能伙伴根
ipmicmd -k "0f 00 30 93 db 07 00 5a 2e 00" smi 0 > temp.txt
grep -wq "0f 31 00 93 00 db 07 00 00" temp.txt
if [ $? == 0 ]; then
    echo_success "enable partner pcert success"
    echo "enable partner pcert success" >>$SAVEFILE
    ok_quit
fi
echo_fail "enable partner pcert fail"
cat temp.txt
error_quit
