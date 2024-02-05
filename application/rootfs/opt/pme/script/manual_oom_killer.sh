#!/bin/bash
MAINTENANCE_LOG_FILE=/opt/pme/pram/md_so_maintenance_log
APP_DEBUG_LOG_FILE=/var/log/pme/app_debug_log_all
POLLING_INTERVAL=30
QUEUE_COUNT=0
MAX_QUEUE_RECOVERY=180
# MAX_QUEUE_LOGCNT值小于MAX_QUEUE_RECOVERY，kill动作之前，提前记录几次日志
MAX_QUEUE_LOGCNT=176

SEM_ERROR_COUNT=0
SEM_PID=-1
MAX_SEM_RECOVERY=5

RECOVERY_CONF_FILE=/data/opt/pme/queue_reset_cnt
MAX_RECOVERY=1000
KILL_APP_COUNT=0
KILL_APP_FLAG=0

IPCS_INFO=/dev/shm/ipcs_info
SEM_INFO=/dev/shm/ipcs_s

app_log()
{
    echo  -e `date  +"%Y-%m-%d %T %N"`" $1" >> $APP_DEBUG_LOG_FILE
}

mt_log()
{
    echo  -e `date  +"%Y-%m-%d %T %N"`" $1"  >> $MAINTENANCE_LOG_FILE
    app_log "$1"
}

check_recovery_times()
{
    recovery=0

    if [ -f $RECOVERY_CONF_FILE ]; then
           recovery=`cat $RECOVERY_CONF_FILE`
    fi
        
    app_log "The system has tried to recover $recovery times"
    if [ $recovery -ge $MAX_RECOVERY ]; then
         return 1
    fi

    return 0
}

set_recover_times()
{
    recovery=0

    if [ -f $RECOVERY_CONF_FILE ]; then
           recovery=`cat $RECOVERY_CONF_FILE`
    fi
        
    mt_log "The system has tried to recover $recovery times"

    recovery=$(($recovery+1))
    echo $recovery > $RECOVERY_CONF_FILE
    
    return 0
}

check_msg_queue()
{
    ipcsinfo=/dev/shm/queue.log
    
    /usr/bin/ipcs -u > $ipcsinfo
    chmod 640 $ipcsinfo
    
    result=`grep -i "used headers = 0" $ipcsinfo`
    if [ $? -eq 0 ]; then
        QUEUE_COUNT=0
    else
        QUEUE_COUNT=$(($QUEUE_COUNT+1))
    fi
    
    if [ $QUEUE_COUNT -ge $MAX_QUEUE_LOGCNT ]; then
        app_log "queue not empty count=$QUEUE_COUNT, /usr/bin/ipcs -u info:"
        cat $ipcsinfo >> $APP_DEBUG_LOG_FILE
    fi
    if [ $QUEUE_COUNT -ge $MAX_QUEUE_RECOVERY ]; then
        QUEUE_COUNT=0
        return 0
    else
        return 1
    fi
}

check_sem_and_kill()
{
    # 如需修改此文件名，注意需要符合sem_finder.cpp中的字符白名单校验
    SEM_INFO_DETAIL=/dev/shm/ipcs_s_detail
    /usr/bin/ipcs -s | grep -v Semaphore | grep -v key > $SEM_INFO
    echo "" > $SEM_INFO_DETAIL # clear first
    while read line
    do
        if [ -n "$line" ]
        then
            echo "$line" | awk '{print $2}' | xargs /usr/bin/ipcs -s -i >> $SEM_INFO_DETAIL
            echo "" >> $SEM_INFO_DETAIL
            sleep 1
        fi
    done < $SEM_INFO
    
    # check sem block , find pid 
    pid=`/usr/sbin/sem_finder $SEM_INFO_DETAIL`
    
    # check pid , make sure pid is number
    expr $pid "+" 0 &> /dev/null
    if [ $? -ne 0 ]; then
        app_log "sem_finder proceed fail pid=${pid}"
        return 0
    fi
    
    # check pid, make sure pid meet expectations
    echo "${pid}"|grep ^0
    if [[ $? -eq 0 || $pid -lt -1 ]];then
        app_log "pid does not meet expectations pid=${pid}"
        return 0
    fi
    if [ $pid = -1 ]; then
        # no app is locked, set it free
        if [ $SEM_PID != -1 ]; then
            SEM_PID=-1
            SEM_ERROR_COUNT=0
        fi
        return 0
    fi
    
    if [ $SEM_PID = -1 ]; then
        SEM_PID=$pid
        SEM_ERROR_COUNT=1
        return 0
    else
        if [ $SEM_PID = $pid ]; then
            SEM_ERROR_COUNT=$(($SEM_ERROR_COUNT+1))
            app_log "count=$SEM_ERROR_COUNT found pid=$pid sem block"
            # kill it if exceed
            if [ $SEM_ERROR_COUNT -ge $MAX_SEM_RECOVERY ]; then
                mt_log "sem locked, kill app pid=${pid}, kill count=$KILL_APP_COUNT"
                KILL_APP_COUNT=$(($KILL_APP_COUNT+1))
                app_log "/usr/bin/ipcs -s info:"
                cat $SEM_INFO >> $APP_DEBUG_LOG_FILE
                app_log "/usr/bin/ipcs -s -i info:"
                cat $SEM_INFO_DETAIL >> $APP_DEBUG_LOG_FILE
                SEM_PID=-1
                SEM_ERROR_COUNT=0
                app_log "begin to kill pid=$pid, ps info:"
                ps -elf >> $APP_DEBUG_LOG_FILE
                kill -9 ${pid} > /dev/null 2>&1
            fi
        else
            # if another app is locked, change it
            SEM_PID=-1
            SEM_ERROR_COUNT=0
        fi
        return 0
    fi
}

dump_queue_info()
{
    echo > $IPCS_INFO
    local i=1
    while [ $i -lt 4 ]
    do
    echo "=================================$i==============================" >> $IPCS_INFO
        /usr/bin/ipcs -u >> $IPCS_INFO
        /usr/bin/ipcs -a >> $IPCS_INFO
        /usr/bin/ipcs -p >> $IPCS_INFO
        sleep 1
        let i=i+1
    done
}

kill_app()
{
    dump_queue_info
    
    queue_id_info=/dev/shm/queue_id_info
    
    kill_app_pid_info=/dev/shm/kill_app_pid_info
    echo > /dev/shm/kill_app_pid_info
    
    #avoid killing dfm etc.
    app_list=/dev/shm/app_temp_list
    cat /opt/pme/extern/app.list > $app_list
    sed -i "/#/"d $app_list
    sed -i '/^$/'d $app_list
    
    ipcs_p_info=/dev/shm/ipcs_p_info
    /usr/bin/ipcs -p > $ipcs_p_info
    
    ipcs_q_info=/dev/shm/ipcs_q_info
    /usr/bin/ipcs -q > $ipcs_q_info
    
    #get qid
    awk '{if ( $6!="0" && $2!="msqid" && $2!="Message" && $2!="" ) print $2}' /dev/shm/ipcs_q_info >$queue_id_info
    
    while read line
    do
        #get process id from task/process id, avoid taskid 0
        #get lspid
        taskid=`awk '{if ( $1==queue_id ) {print $3;}}' queue_id=$line $ipcs_p_info`
        if [ "$taskid" != "0" ]; then
            ps -AL | grep $taskid | grep -v grep | head -1 | awk '{print $1}' >>$kill_app_pid_info
        fi
        #get lrpid
        taskid=`awk '{if ( $1==queue_id ) {print $4;}}' queue_id=$line $ipcs_p_info`
        if [ "$taskid" != "0" ]; then
            ps -AL | grep $taskid | grep -v grep | head -1 | awk '{print $1}' >>$kill_app_pid_info
        fi
    done < $queue_id_info
    
    while read line
    do
        if [ "$line" = "" ]; then
            continue
        fi
        app_name=`ps -el | grep $line | awk '{if ($4==app_pid) {print $14;}}' app_pid=$line`
        grep $app_name $app_list
        if [ $? -eq 0 ]; then
            if [ $KILL_APP_FLAG -eq 0 ]; then
                app_log "before kill ipmi queue info:"
                cat $IPCS_INFO >> $APP_DEBUG_LOG_FILE
                app_log "before kill ipmi ps info:"
                ps -elf >> $APP_DEBUG_LOG_FILE
                #kill ipmi
                killall ipmi  > /dev/null 2>&1
            fi
            app_log "begin to kill pid=$line, ps info:"
            ps -elf >> $APP_DEBUG_LOG_FILE
            kill -9 ${line} > /dev/null 2>&1
            KILL_APP_FLAG=$(($KILL_APP_FLAG+1))
        fi
    done < $kill_app_pid_info

    return 1
}

dump_ipcs_info()
{
    log_file=/data/var/log/pme/ipcs_info

    rm $log_file
    if [ -f $IPCS_INFO ]; then
        echo -e "========= ipcs queue info =======\n" >> $log_file
        cat $IPCS_INFO >> $log_file
    fi

    if [ -f $SEM_INFO ]; then
        echo -e "========= ipcs -s =======\n" >> $log_file
        cat $SEM_INFO >> $log_file
    fi

    if [ -f /dev/shm/ipcs_q_info ]; then
        echo -e "========= ipcs -q =======\n" >> $log_file
        cat /dev/shm/ipcs_q_info >> $log_file
    fi
}

reset_bmc()
{
    mt_log "before restart BMC dump queue info"
    dump_queue_info
    dump_ipcs_info
    set_recover_times
    mt_log "begin to reset bmc"
    sleep 2
    if [ -f /opt/pme/bin/ipmcset ]; then
        echo y | /opt/pme/bin/ipmcset -d reset &
    else
        echo y | ipmcset -d reset &
    fi

    sleep 600
    mt_log "/bin/busybox reboot"
    /bin/busybox reboot
}

main()
{    
    #wait 10 minutes
    sleep 600
    app_log  "starting app self monitor"

    check_recovery_times

    if [ $? -eq 1 ]; then
        mt_log "exit manual oom killer"
        exit 0
    fi
    
    while [ 1 ]
    do
        sleep $POLLING_INTERVAL    
        
        if [ $KILL_APP_COUNT -eq 10 ] ; then
            #reset BMC
            KILL_APP_COUNT=0
            reset_bmc
            
        else
            check_msg_queue
            if [ $? -eq 0 ];then
                #kill app
                kill_app
                if [ $KILL_APP_FLAG -ne 0 ]; then
                    KILL_APP_COUNT=$(($KILL_APP_COUNT+1))
                    mt_log "kill ${KILL_APP_FLAG} apps, count=$KILL_APP_COUNT"
                    KILL_APP_FLAG=0
                fi
            fi
            
            check_sem_and_kill
        fi
    done
}

main




