#!/bin/sh
chmod +x *
SAVEFILE="common_factory_restore_save.txt"

wait_sec()
{
    wait_time_sec=$1

    for((i=0;i<$wait_time_sec;i++))
    do
        echo -n -e "*"
        sleep 1
    done
}

factory_restore()
{
    echo ""
    echo "Start factory restore!"

    for((i=0;i<3;i++))
    do
        run_cmd "f 0 30 93 DB 07 00 08 10 01" "0f 31 00 93 00 db 07 00 01"
        if [ $? != 0 ] ; then
            sleep 5
            continue
        fi
        break
    done

    if [ $i -eq 3 ] ; then
        echo_fail "Factory restore failed"
        echo "Factory restore failed" >>$SAVEFILE
        error_quit
    fi

    echo "Wait BMC reset!"
    local reset_time=180
    if [ EMP$project_code == "EMP05021MCQ" ] ; then
        reset_time=480
    fi
    if [ EMP$project_code == "EMP05022EUH" ] ; then
        reset_time=360
    fi

    wait_sec $reset_time

    for((i=0;i<18;i++))
    do
        run_cmd "f 0 06 01" "0f 07 00 01 00"
        if [ $? == 0 ] ; then
            break
        fi

        wait_sec 10
    done

    echo ""

    if [ $i -eq 18 ] ; then
        echo_fail "Factory restore failed"
        echo "Factory restore failed" >>$SAVEFILE
        error_quit
    fi

    echo_success "Factory restore success"
    echo "Factory restore success" >>$SAVEFILE

    return 0
}

#解压依赖包
tar zxvf function.tar.gz > /dev/null 2>&1

#加载公共脚本
source ./function/common.sh

main()
{
    #加载文件传输脚本
    ./dft_enable.sh

	# DTS202101190FXED3P1F00 等待清理升级标志
    local upgrade_wait_time=15
	echo "will wait ${upgrade_wait_time}s for dftimage upgrade process ending totally"
	sleep $upgrade_wait_time  

    #恢复出厂配置
    factory_restore
 
}


main