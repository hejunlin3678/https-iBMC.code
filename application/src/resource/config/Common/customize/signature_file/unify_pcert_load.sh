chmod +x *
############################################
# bash script: 导入伙伴根哈希
############################################
#把项目编码赋值好
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`

SAVEFILE=$project_code"_save.txt"
IMPROT_NUM=0
disenable_pcert_flag=0

#查看日志文件是否存在
if [ ! -f $SAVEFILE ] ; then
    rm -rf $SAVEFILE
fi 

if [[ $1 != "" ]]; then
    disenable_pcert_flag="$1"
else
    disenable_pcert_flag=0
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
	    ok_quit
	fi

    #  S1和S2已经导入
	grep -wq "0f 31 00 93 81" temp.txt
	if [ $? == 0 ]; then
	    echo_success "have been import s1 and s2 partner pcert"
	    echo "have been import s1 and s2 partner pcert" >>$SAVEFILE
	    exit 2
	fi

    #  S1已经导入
	grep -wq "0f 31 00 93 83" temp.txt
	if [ $? == 0 ]; then
	    echo_success "have been import s1 partner pcert"
	    echo "have been import s1 partner pcert" >>$SAVEFILE
        IMPROT_NUM=$((IMPROT_NUM+=1))
	    return 0
	fi

    #  S2已经导入
	grep -wq "0f 31 00 93 84" temp.txt
	if [ $? == 0 ]; then
	    echo_success "have been import s2 partner pcert"
	    echo "have been import s2 partner pcert" >>$SAVEFILE
        IMPROT_NUM=$((IMPROT_NUM+=1))
	    return 0
	fi
    
    echo_fail "the import pcert return value is unknow"
    echo "the import pcert return value is unknow" >>$SAVEFILE
    cat temp.txt
    error_quit
}

# 判断efuse状态
efuse_type_judge()
{
    echo "start get efuse state"
    # 如果返回值为01，则代表含有efuse
    run_cmd "0f 00 30 93 db 07 00 5b 2c 00" "0f 31 00 93 00 db 07 00 01"
    if [ $? == 0 ]; then
        for((i=0;i<$REPEAT;i++))
        do
            ipmicmd -k "0f 00 30 93 db 07 00 5b 2b 00" smi 0 > efuse_type_judge.txt
            if [ $? = 0 ] ; then
                EFUSE_TYPE=`cat efuse_type_judge.txt | awk '{print $NF}'`
                rm -f efuse_type_judge.txt
                #判断是支持伙伴模式的efuse还是不支持伙伴模式的efuse
                if [ "$EFUSE_TYPE" == "00"  -o "$EFUSE_TYPE" == "10" ] ; then
                    echo "it's old efuse"
                    ok_quit
                fi
                echo "it's new efuse"
                return 0
            fi
            sleep $REPEAT_TIME
        done
        echo "get efuse type fail"
        error_quit
    else
        echo_fail "there is no efuse, please make sure this environment have efuse"
        error_quit
    fi
}

# 判断是否是老efuse，如果是老efuse或者没有efuse则直接退出，不能进行导根操作
efuse_type_judge

for ((load_times=0; load_times<5; load_times++))
do
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
    if [ $? == 0 ]; then
        echo_success "import s1 pcert success"
        echo "import s1 pcert success" >>$SAVEFILE
        break
    fi

    # m3的重启时间为大概为120s，预留10s
    sleep 130

    echo "the import s1 pcert times is ${load_times}"
    if [ ${load_times} == 4 ]; then
	echo_fail "import s1 pcert fail"
	echo "import s1 pcert fail" >SAVEFILE
	error_quit
    fi
done

for ((load_times=0; load_times<5; load_times++))
do
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
    if [ $? == 0 ]; then
        echo_success "import s2 pcert success"
        echo "import s2 pcert success" >>$SAVEFILE
        break
    fi

    # m3的重启时间大概为120s，预留10s
    sleep 130

    echo "the import s2 pcert times is ${load_times}"

    if [ ${load_times} == 4 ]; then
	echo_fail "import s2 pcert fail"
	echo "import s2 pcert fail" >$SAVEFILE
	error_quit
    fi
done

if [ $IMPROT_NUM -lt 2 ] && [ $disenable_pcert_flag == 0 ]; then
    # 使能伙伴根
    run_cmd "0f 00 30 93 db 07 00 5a 2e 00" "0f 31 00 93 00 db 07 00 00"
    if [ $? == 0 ]; then
        echo_success "enable partner pcert success"
        echo "enable partner pcert success" >>$SAVEFILE
        ok_quit
    fi
    echo_fail "enable partner pcert fail"
    echo "enable partner pcert fail" >>$SAVEFILE
    cat temp.txt
    error_quit
fi

echo_success "execute import pcert script success"
