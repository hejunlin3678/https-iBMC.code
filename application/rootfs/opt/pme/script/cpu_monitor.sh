#!/bin/bash
APP_DEBUG_LOG_FILE=/var/log/pme/app_debug_log_all
SECURITY_LOG_FILE=/var/log/pme/security_log
COUNT=0
ROUND=1
#单进程单核cpu最大占用率
MAX_CPU_USAGE=90
SLEEP_BASE_TIME=10
PROC_INFO_RECORD_NUM=3
CHECK_COUNT_PER_ROUND=6

app_log()
{
    echo -e `date  +"%Y-%m-%d %T"`" $1" >> $APP_DEBUG_LOG_FILE
}

security_log()
{
    echo -e `date  +"%Y-%m-%dT%T"`" $1" >> $SECURITY_LOG_FILE
}

check_cpu_usage()
{
    top_info=`top -b -n 1 -o %CPU`
    max_usage=`echo "$top_info" | head -n 8 | tail -n +8 | awk '{print $9}'`

    if [ `expr $max_usage \> $MAX_CPU_USAGE` -eq 0 ];then
        if [ $COUNT -ne 0 ] || [ $ROUND -ne 1 ];then
            app_log  "cpu_monitor INFO: cpu usage decrease from high in round $ROUND"
            COUNT=0
            ROUND=1
        fi
        sleep `expr $SLEEP_BASE_TIME \* $ROUND`
        return 0
    fi

    COUNT=`expr $COUNT + 1`
    if [ $COUNT -eq $CHECK_COUNT_PER_ROUND ];then
        COUNT=0
        idle_rate=`echo "$top_info" | head -n 3 | tail -n +3 | awk -F, '{print $4}'| awk '{print $1}'`
        overall_usage=`awk -v x=100 -v y=$idle_rate 'BEGIN{printf "%.2f\n", x-y}'`
        top_proc_info=`echo "$top_info" | tail -n +8 | head -n $PROC_INFO_RECORD_NUM | awk '{print $1,$9,$12}'`
        log_info="cpu_monitor ERROR: cpu usage is too high in round $ROUND, overall cpu usage is $overall_usage%, top $PROC_INFO_RECORD_NUM procs' pid, usage, command: "
        for i in `seq $PROC_INFO_RECORD_NUM`;do
            proc_info=`echo "$top_proc_info" | head -n $i | tail -n +$i`
            log_info=${log_info}${proc_info}$([ "$i" -eq $PROC_INFO_RECORD_NUM ] && echo "" || echo ", ")
        done
        security_log "$log_info"
    fi
    
    sleep `expr $SLEEP_BASE_TIME \* $ROUND`

    if [ $COUNT -eq 0 ];then
        ROUND=`expr $ROUND + 1`
    fi
}

main()
{
    app_log  "cpu_monitor INFO: starting cpu monitor"

    while [ 1 ]
    do
        check_cpu_usage
    done
}

main