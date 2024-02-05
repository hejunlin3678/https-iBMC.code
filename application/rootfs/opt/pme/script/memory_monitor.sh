#!/bin/bash

G_PROGRAM_SRC="/opt/pme/extern/app.list"
G_PROGRAM_LIST="/dev/shm/program.list.tmp"
G_CUR_MEM_LIST="/dev/shm/mem.list.tmp"
G_MEM_RECORD="/dev/shm/mem_record"
MAINTENANCE_LOG_FILE="/opt/pme/pram/md_so_maintenance_log"
APP_DEBUG_LOG_FILE="/var/log/pme/app_debug_log_all"
MAX_MEMORY_USAGE=30720
DETECT_COUNT=6
FACTOR=3

MAX_MEM_USAGE_RATE=90
PROC_INFO_RECORD_NUM=3
SECURITY_LOG_FILE=/var/log/pme/security_log

security_log()
{
    echo -e `date  +"%Y-%m-%dT%T"`" $1" >> $SECURITY_LOG_FILE
}

check_memory_usage()
{
    for i in `seq $DETECT_COUNT`;do
        top_info=`top -b -n 1 -o %MEM`
        total_mem=`echo "$top_info" | head -n 4 | tail -n +4 | awk -F, '{print $1}'| awk '{print $4}'`
        free_mem=`echo "$top_info" | head -n 4 | tail -n +4 | awk -F, '{print $2}'| awk '{print $1}'`
        overall_usage=`awk -v x=$free_mem -v y=$total_mem 'BEGIN{printf "%.2f\n", 100*(1-x/y)}'`
        if [ `expr $overall_usage \> $MAX_MEM_USAGE_RATE` -eq 0 ];then
            break
        fi

        if [ $i -ne $DETECT_COUNT ]; then
            sleep 5
            continue
        fi

        top_proc_info=`echo "$top_info" | tail -n +8 | head -n $PROC_INFO_RECORD_NUM | awk '{print $1,$10,$12}'`
        log_info="memory_monitor ERROR: memory usage is too high, overall memory usage is $overall_usage%, top $PROC_INFO_RECORD_NUM procs' pid, usage, command: "
        for j in `seq $PROC_INFO_RECORD_NUM`;do
            proc_info=`echo "$top_proc_info" | head -n $j | tail -n +$j`
            log_info=${log_info}${proc_info}$([ "$j" -eq $PROC_INFO_RECORD_NUM ] && echo "" || echo ", ")
        done
        security_log "$log_info"
    done
}

check_memory_usage

get_init_mem (){
    mem=`grep -w "$1" "$G_MEM_RECORD"|grep -v "defunct"|awk '{print $2}'`
    if [ -z "$(echo $mem| sed -n "/^[0-9]\+$/p")" ]; then 
        echo -1
    fi
    echo $mem
}

get_cur_mem (){
    mem=`grep -w "$1" "$G_CUR_MEM_LIST"|grep -v "defunct"|awk '{print $2}'`
    if [ -z "$(echo $mem| sed -n "/^[0-9]\+$/p")" ]; then 
        echo -1
    fi
    echo $mem
}

uptime=`cat /proc/uptime |awk -F "." '{print $1}'`

if [ $uptime -lt 3600 ]; then
    echo `date "+%Y-%m-%d %H:%M:%S"`" memory_monitor INFO: uptime is less than 1 hour." >> $APP_DEBUG_LOG_FILE
    exit 0
fi

if [ ! -f "$G_MEM_RECORD" ]; then
    ps -axo pid,rss,cmd >$G_MEM_RECORD
    echo `date "+%Y-%m-%d %H:%M:%S"`" memory_monitor INFO: init memory record." >> $APP_DEBUG_LOG_FILE
    exit 0
fi

mem_total=`cat /proc/meminfo |grep MemTotal  | awk '{print $2}'`

if [ $mem_total -gt 524288 ];then
    MAX_MEMORY_USAGE=$((MAX_MEMORY_USAGE*8))
    FACTOR=8
elif [ $mem_total -gt 262144 ];then
    MAX_MEMORY_USAGE=$((MAX_MEMORY_USAGE*2))    
fi

grep -v '#' "$G_PROGRAM_SRC" > "$G_PROGRAM_LIST"

echo "syslog-ng" >> "$G_PROGRAM_LIST"
echo "snmpd" >> "$G_PROGRAM_LIST"

ps -axo pid,rss,cmd>$G_CUR_MEM_LIST

while read program
do
    if [ -n "$program" ]
    then
        init_mem=$(get_init_mem $program)
        if [ $init_mem -eq -1 ]; then
            continue
        fi
        
        cur_mem=$(get_cur_mem $program)
        
        if [ $cur_mem -gt $(($init_mem*$FACTOR)) -o $cur_mem -gt $MAX_MEMORY_USAGE ];then
            for i in `seq $DETECT_COUNT`  
            do   
                sleep 10         
                cur_mem=$(get_cur_mem $program)

                if [ $cur_mem -gt $(($init_mem*$FACTOR)) -o $cur_mem -gt $MAX_MEMORY_USAGE ]; then
                    if [ $i -ge $DETECT_COUNT ]; then
                        echo `date "+%Y-%m-%d %H:%M:%S"`" memory_monitor ERROR: The memory usage of ${program} is "$cur_mem" KB, which is above threshold (initial memory usage is $init_mem, max threshhold is $MAX_MEMORY_USAGE)." >> $APP_DEBUG_LOG_FILE
                        echo `date "+%Y-%m-%d %H:%M:%S"`" WARN : The memory usage of ${program} is "$cur_mem" KB, which is above threshold(initial memory usage is $init_mem, max threshhold is $MAX_MEMORY_USAGE). memory_monitor will restart ${program} immediately." >> $MAINTENANCE_LOG_FILE
                        killall $program
                    fi                
                else
                    break
                fi
            done
        fi
    fi
done < "$G_PROGRAM_LIST"

rm -f "$G_PROGRAM_LIST" > /dev/null 2>&1
rm -f "$G_CUR_MEM_LIST" > /dev/null 2>&1

exit 0