#!/bin/sh
# M3日志收集
function dump_m3_log()
{
    if [ -f /usr/bin/logs_tool ]; then
        local m3_log_limited_size=409600
        local m3_all_log="${1}/m3_all_log"

        if [ -d ${1} ]; then
            rm ${1}/m3_log* > /dev/null 2>&1
        else
            mkdir "${1}" -p > /dev/null 2>&1
        fi

        cd /usr/bin/
        /usr/bin/logs_tool read /usr/bin/index.log ${1}/m3_log  ${1}/m3_log.1 > /dev/null 2>&1
        cat ${1}/m3_log.1 >> ${1}/m3_log /dev/null 2>&1

        # 超过大小限制需要截断
        file_size=$(stat -c %s ${1}/m3_log)
        if [ ${file_size} -gt ${m3_log_limited_size} ]; then
            tail -c ${m3_log_limited_size} ${1}/m3_log >${m3_all_log} /dev/null 2>&1
        else
            mv ${1}/m3_log ${m3_all_log}
        fi

        chmod 440 ${m3_all_log} > /dev/null 2>&1
        rm ${1}/m3_log* > /dev/null 2>&1
        cd - > /dev/null 2>&1
    fi
}

dump_m3_log ${1}
