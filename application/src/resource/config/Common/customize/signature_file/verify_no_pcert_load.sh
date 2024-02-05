chmod +x *
############################################
# bash script: 校验环境无根
############################################
# 把项目编码赋值好
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`

SAVEFILE=$project_code"_save.txt"
IMPROT_NUM=0

# 查看日志文件是否存在
if [ ! -f $SAVEFILE ] ; then
    rm -rf $SAVEFILE
fi 

source ./common_transfer_flie.sh

source ./add_temp_user.sh

# 判断是否有efuse，如果无efuse则进行升级
judge_efuse()
{
    echo "start get efuse state"
    # 如果返回值为00，则代表不含有efuse
    run_cmd "0f 00 30 93 db 07 00 5b 2c 00" "0f 31 00 93 00 db 07 00 00"
    if [ $? == 0 ]; then
        echo_fail "the environment has no efuse"
        echo "the environment has efuse" >>$SAVEFILE
        error_quit
    else
        run_cmd "0f 00 30 93 db 07 00 5b 2b 00" "0f 31 00 93 00 db 07 00 00"
        if [ $? == 0 ]; then
            echo_fail "the environment is old efuse"
            echo "the environment is old efuse" >>$SAVEFILE
            error_quit
        else
            echo_success "the environment is new efuse"
        fi
    fi
}

# 判断是否有根,如果有根，则退出脚本
judge_pcert_state()
{
    if [ ! -f ./wbd_verify.hpm ]; then
        echo_fail "there is no wbd_verify.hpm"
        echo "there is no wbd_verify.hpm" >>$SAVEFILE
        error_quit
    fi
    
    ./upgrade_hpm.sh "wbd_verify.hpm" 0 0
    if [ $? -ne 0 ]; then
        echo_fail "this enivronment have been import pcert"
        echo "this enivronment have been import pcert" >>$SAVEFILE
        error_quit
    else
        echo_success "this enivronment no pcert"
    fi
}

# 加载驱动
load_driver

# 判断当前为新efuse，如果失败退出脚本
judge_efuse
# 判断是否有根,如果有根，则退出脚本
judge_pcert_state

ok_quit
