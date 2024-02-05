#!/bin/sh

# 日志的长度限制
LOG_SINGLE_MAX_LENGTH=256000
LOG_SHELL_OPT_MAX_LENGTH=61440
LOG_EKBOX_MAX_LENGTH=204800
LOG_KBOX_MAX_LENGTH=102400
LOG_CPLDREG_MAX_LENGTH=8192
LOG_FLASH_MAX_LENGTH=3072
LOG_PSU_MAX_LENGTH=3072
LOG_BIOS_MAX_LENGTH=7864320
LOG_PTHREAD_MAX_LENGTH=51200
LOG_DYING_MAX_LENGTH=20480
LOG_KS_0_MAX_LENGTH=20480
LOG_KS_MAIN_MAX_LENGTH=204800
IMU_LOG_MAX_LENGTH=102400
ARM_INFO_LOG_MAX_LENGTH=102400
LOG_BBU_MAX_LENGTH=262144
LOG_PID_MAX_LENGTH=262144
LOG_FANSPEED_LENGTH=262144
LOG_MEMERR_MAX_LENGTH=262144
M7_LOG_MAX_LENGTH=25600
PS_LOG_MAX_LENGTH=25600
APP_LOG_MAX_LENGTH=51200
CORE_MAX_LENGTH=51200
LOG_VERSION_LENGTH=8192
RTOS_INFO_LENGTH=20480

RET_SUCCESS=0
RET_INVALID_SRC=1
RET_SRC_OUT_OF_RANGE=2
RET_MD5_SAME=3
# 打包发生变化的日志到指定目录
# 参数：源文件路径，目标路径，大小限制，超过大小是否删源文件，删除后是否重启syslog-ng
function packing()
{
    local src_file=${1}
    local dst_file=${2}
    local tmp_file="/dev/shm/PangeaLog/pangea_log_tmp.tar.gz"
    if [ ! -f ${src_file} ]; then
        return $RET_INVALID_SRC
    fi

    if [ ! -d ${dst_file%/*} ]; then
        mkdir -p ${dst_file%/*}
    fi

    # 如果需要压缩文件，压缩后再判断长度、MD5
	if [ "${src_file##*.}"x != "gz"x ] && [ "${dst_file##*.}"x = "gz"x ]; then
        if [ -f ${tmp_file} ]; then
            rm ${tmp_file}
        fi
		tar -zcvf ${tmp_file} ${src_file} > /dev/null 2>&1
        chmod 440 ${tmp_file}
		src_file=${tmp_file}
	fi

    # 文件长度是否在合理范围之内, 避免copy到/tmp下占过大内存，导致升级失败
    src_file_size=$(ls -l ${src_file} | awk '{print $5}')
    if [ ${src_file_size} -gt ${3} ]; then
        if [ ${4} -eq 0 ]; then
            return $RET_SRC_OUT_OF_RANGE
        fi
        rm -rf ${src_file}
        if [ ${5} -eq 1 ]; then
            killall -9 syslog-ng > /dev/null 2>&1
        fi
        return $RET_SRC_OUT_OF_RANGE
    fi

    # 源文件存在，判断md5是否一致，若相同，不需要拷贝
    if [ -f ${dst_file} ]; then
        # 先压缩，然后再算md5
        src_md5=$(md5sum ${src_file} | awk '{print $1}')
        des_md5=$(md5sum ${dst_file} | awk '{print $1}')
        if [ $src_md5 = $des_md5 ]; then
            return $RET_MD5_SAME
        fi
        rm ${dst_file}
    fi

    cp -p ${src_file} ${dst_file} > /dev/null 2>&1

    return $RET_SUCCESS
}


# APP日志
app_dump="${1}/AppDump"
src_dir="/dev/shm/PangeaLog/AppDump"

# bmc_global日志
bmc_app_src="${src_dir}/BMC"
dst_file="${app_dump}/BMC"
packing "${bmc_app_src}/mcinfo.txt" "${dst_file}/mcinfo.txt" $APP_LOG_MAX_LENGTH 0 0
packing "${bmc_app_src}/nandflash_info.txt" "${dst_file}/nandflash_info.txt" $LOG_FLASH_MAX_LENGTH 0 0
packing "${bmc_app_src}/time_zone.txt" "${dst_file}/time_zone.txt" $APP_LOG_MAX_LENGTH 0 0
packing "${bmc_app_src}/psu_info.txt" "${dst_file}/psu_info.txt" $LOG_PSU_MAX_LENGTH 0 0

# fru_data日志
fru_data_src="${src_dir}/FruData"
dst_file="${app_dump}/FruData"
packing "${fru_data_src}/fruinfo.txt" "${dst_file}/fruinfo.txt" $APP_LOG_MAX_LENGTH 0 0

# power_mgnt日志
power_mgnt_src="${src_dir}/PowerMgnt"
dst_file="${app_dump}/PowerMgnt"
packing "${power_mgnt_src}/power_bbu_info.log" "${dst_file}/power_bbu_info.tar.gz" $APP_LOG_MAX_LENGTH 0 0

# upgrade日志
upgrade_src="${src_dir}/UPGRADE"
dst_file="${app_dump}/UPGRADE"
packing "${upgrade_src}/upgrade_info" "${dst_file}/upgrade_info" $APP_LOG_MAX_LENGTH 0 0

# sensor_alarm日志
sensor_src="${src_dir}/sensor_alarm"
dst_file="${app_dump}/sensor_alarm"
packing "${sensor_src}/sensor_info.txt" "${dst_file}/sensor_info.txt" $APP_LOG_MAX_LENGTH 0 0
packing "${sensor_src}/current_event.txt" "${dst_file}/current_event.txt" $APP_LOG_MAX_LENGTH 0 0
packing "${sensor_src}/sel.tar" "${dst_file}/sel.tar" $APP_LOG_MAX_LENGTH 0 0
packing "${sensor_src}/sel.db" "${dst_file}/sel.db" $APP_LOG_MAX_LENGTH 0 0
packing "${sensor_src}/LedInfo" "${dst_file}/LedInfo" $APP_LOG_MAX_LENGTH 0 0

# core文件
core_dump="${1}/CoreDump"
core_file_count=`ls /var/coredump/ | grep core- | wc -w`
if [ "$core_file_count" -gt 0 ]
then
    cd /var/coredump
    tar -zcvf core-files.tar.gz core-* > /dev/null 2>&1
    packing "core-files.tar.gz" "${core_dump}/core-files.tar.gz" $CORE_MAX_LENGTH 0 0
    rm core-files.tar.gz
    cd - > /dev/null 2>&1
fi
sleep 0.005

# RTOSDump
rtos_dump="${1}/RTOSDump"
src_dir="/dev/shm/PangeaLog/RTOSDump"
mkdir -p ${src_dir}
cd "${src_dir}"

# 系统信息
cat /proc/kbox/regions/panic > ./kbox.txt 2>&1
cat /proc/ekbox > ./ekbox.txt 2>&1
cat /proc/kbox/regions/dying > ./dying.txt 2>&1
packing "kbox.txt" "${rtos_dump}/kbox.tar.gz" $LOG_KBOX_MAX_LENGTH 1 0
packing "ekbox.txt" "${rtos_dump}/ekbox.tar.gz" $LOG_EKBOX_MAX_LENGTH 1 0
packing "dying.txt" "${rtos_dump}/dying.tar.gz" $LOG_DYING_MAX_LENGTH 1 0
packing "/proc/version" "${rtos_dump}/version" $LOG_VERSION_LENGTH 0 0
sleep 0.005

i=0
max_core_num=$(cat /proc/cpuinfo | grep processor | wc -l)
while [ $i -lt $max_core_num ]; do
    if [ -f "/proc/kbox/regions/ks_"$i ]; then
        cat /proc/kbox/regions/ks_$i >> ./ks_${i}.txt 2>&1
        packing "ks_${i}.txt" "${rtos_dump}/ks_${i}.tar.gz" $LOG_KS_0_MAX_LENGTH 1 0
    fi
    i=$(( $i + 1 ))
done

if [ -f "/proc/kbox/regions/ks_main" ]; then
    cat /proc/kbox/regions/ks_main | tail -n 3000 >> ./ks_main.txt 2>&1
    packing "ks_main.txt" "${rtos_dump}/ks_main.tar.gz" $LOG_KS_MAIN_MAX_LENGTH 1 0
fi

if [ -f "pthread1.txt" ]; then
    cp pthread1.txt pthread2.txt > /dev/null 2>&1
fi

if [ -f "pthread0.txt" ]; then
    cp pthread0.txt pthread1.txt > /dev/null 2>&1
fi
ps -elf > pthread0.txt
tar -zvcf pthread.tar.gz pthread* > /dev/null 2>&1
packing "pthread.tar.gz" "${rtos_dump}/pthread.tar.gz" $LOG_PTHREAD_MAX_LENGTH 0 0

# 版本信息
platform_file=/opt/pme/conf/profile/platform.xml
bmc_name=`cat ${platform_file} | grep CustomLogIbmcName | awk -F '[<>]' '{print tolower($5)}'`
if [ "$bmc_name" = "" ] ; then
	bmc_name="ibmc"
fi
cp /dev/shm/PangeaLog/AppDump/BMC/bmc_info ./app_revision.txt
cp /opt/pme/ibmc-revision.txt ./${bmc_name}_revision.txt > /dev/null 2>&1
cp /opt/pme/build_date.txt ./ > /dev/null 2>&1
cp /etc/RTOS-Release ./RTOS-Release > /dev/null 2>&1
cp /etc/RTOS-Revision ./RTOS-Revision > /dev/null 2>&1
packing "app_revision.txt" "${rtos_dump}/app_revision.txt" $LOG_VERSION_LENGTH 0 0
packing "${bmc_name}_revision.txt" "${rtos_dump}/${bmc_name}_revision.txt" $LOG_VERSION_LENGTH 0 0
packing "build_date.txt" "${rtos_dump}/build_date.txt" $LOG_VERSION_LENGTH 0 0
packing "RTOS-Release" "${rtos_dump}/RTOS-Release" $RTOS_INFO_LENGTH 0 0
packing "RTOS-Revision" "${rtos_dump}/RTOS-Revision" $RTOS_INFO_LENGTH 0 0

rm -r $(ls | grep -vw pthread..txt) > /dev/null 2>&1
cd - > /dev/null 2>&1
sleep 0.005

# LogDump
src_dir="/var/log/pme"
log_dump="${1}/LogDump"
m3_dir="/dev/shm/PangeaLog/M3LogDump"
if [ -f "/dev/shm/bmccom.dat" ]; then
    packing /dev/shm/bmccom.dat ${log_dump}/bmccom.tar.gz $LOG_BIOS_MAX_LENGTH 1 0
    rm /dev/shm/bmccom.dat > /dev/null 2>&1
fi

packing "${src_dir}/app_debug_log_all" "${log_dump}/app_curlog.tar.gz"      $LOG_SINGLE_MAX_LENGTH 0 0
packing "${src_dir}/maintenance_log"   "${log_dump}/maint_curlog.tar.gz"    $LOG_SINGLE_MAX_LENGTH 0 0
packing "${src_dir}/strategy_log"      "${log_dump}/strategy_curlog.tar.gz" $LOG_SINGLE_MAX_LENGTH 0 0
packing "${src_dir}/operate_log"       "${log_dump}/operate_curlog.tar.gz"  $LOG_SINGLE_MAX_LENGTH 0 0
packing "${src_dir}/security_log"      "${log_dump}/secure_curlog.tar.gz"   $LOG_SINGLE_MAX_LENGTH 1 1
packing "${src_dir}/remote_log"        "${log_dump}/remote_curlog.tar.gz"   $LOG_SINGLE_MAX_LENGTH 0 0
packing "${src_dir}/linux_kernel_log"  "${log_dump}/linux_curlog.tar.gz"    $LOG_SINGLE_MAX_LENGTH 1 1
packing "${src_dir}/dfm_debug_log"     "${log_dump}/dfm_curlog.tar.gz"      $LOG_SINGLE_MAX_LENGTH 1 1

# m7
i=1
while [ $i -lt 5 ]; do
    packing "${src_dir}/cpu${i}_m7_log" "${log_dump}/m7.${i}_debug_log.tar.gz" $M7_LOG_MAX_LENGTH 0 0
    packing "${src_dir}/cpu${i}_m7_log.tar.gz" "${log_dump}/m7.${i}_debug_log_his.tar.gz" $M7_LOG_MAX_LENGTH 1 0
    i=$(( $i + 1 ))
done

# ps_black_box
packing "${src_dir}/ps_black_box.log" "${log_dump}/ps_black_box.tar.gz" $PS_LOG_MAX_LENGTH 0 0
i=1
while [ $i -lt 4 ]; do
    packing "${src_dir}/ps_black_box.log.${i}.gz" "${log_dump}/ps_black_box.log.${i}.gz" $PS_LOG_MAX_LENGTH 1 0
    i=$(( $i + 1 ))
done

# pid 日志
packing "${src_dir}/pid_log.txt" "${log_dump}/pid_log.tar.gz" $LOG_PID_MAX_LENGTH 0 0
packing "${src_dir}/pid_log1.tar.gz" "${log_dump}/pid_log1.tar.gz" $LOG_PID_MAX_LENGTH 1 0

# 管理板调速日志
packing "${src_dir}/fanspeed_output.txt" "${log_dump}/fanspeed_output.tar.gz" $LOG_FANSPEED_LENGTH 0 0
packing "${src_dir}/fanspeed_output1.tar.gz" "${log_dump}/fanspeed_output1.tar.gz" $LOG_FANSPEED_LENGTH 1 0

# m3
packing "${m3_dir}/m3_all_log" "${log_dump}/m3_all_log.tar.gz" $LOG_SHELL_OPT_MAX_LENGTH 1 0

#bbu 日志
packing "${src_dir}/bbu_realtime_log" "${log_dump}/bbu_realtime_log.tar.gz" $LOG_BBU_MAX_LENGTH 1 0

# imu
packing "${src_dir}/imu_log" "${log_dump}/imu_debug_log.tar.gz" $IMU_LOG_MAX_LENGTH 0 0
packing "${src_dir}/imu_log.tar.gz" "${log_dump}/imu_debug_log_his.tar.gz" $IMU_LOG_MAX_LENGTH 1 0
sleep 0.005

# arminfo_debug_log
packing "${src_dir}/arminfo_debug_log" "${log_dump}/arminfo_debug_log.tar.gz" $ARM_INFO_LOG_MAX_LENGTH 0 0
packing "${src_dir}/arminfo_debug_log_his.tar.gz" "${log_dump}/arminfo_debug_log_his.tar.gz" $ARM_INFO_LOG_MAX_LENGTH 1 0

# HistoryDump
function dump_his_log()
{
    his_dir="/dev/shm/PangeaLog/OldLog"
    src_file=${1}
    file_name=${src_file##*/}
    dst_name=`echo ${file_name} | awk -F '_' '{print $1}'`
    dst_dir=${2}
    packing ${src_file} "${his_dir}/${dst_name}/${file_name}" $LOG_SINGLE_MAX_LENGTH ${3} ${4}
    # 执行了拷贝动作，说明历史日志不一样了，需要重新使用时间戳命名。
    if [ $? -eq $RET_SUCCESS ]; then
        delete_file=$(ls ${dst_dir} | grep ${dst_name}[0-9])
        rm ${dst_dir}/${delete_file} > /dev/null 2>&1
        if [ "${file_name##*.}"x != "gz"x ]; then
            tar -zcvf ${dst_dir}/${dst_name}$(busybox date +%Y%m%d%H%M%S).tar.gz ${src_file} > /dev/null 2>&1
        # app历史日志是gz格式，重命名为tar.gz会解压失败
        elif [ "${file_name#*.}"x != "tar.gz"x ]; then
            cp ${src_file} ${dst_dir}/${dst_name}$(busybox date +%Y%m%d%H%M%S).gz > /dev/null 2>&1
        else
            cp ${src_file} ${dst_dir}/${dst_name}$(busybox date +%Y%m%d%H%M%S).tar.gz > /dev/null 2>&1
        fi
    fi
}

dump_his_log "${src_dir}/app_debug_log_all.1.gz" ${log_dump} 0 0
dump_his_log "${src_dir}/maintenance_log.tar.gz" ${log_dump} 0 0
dump_his_log "${src_dir}/strategy_log.tar.gz" ${log_dump} 0 0
dump_his_log "${src_dir}/operate_log.tar.gz" ${log_dump} 0 0
dump_his_log "${src_dir}/linux_kernel_log.1" ${log_dump} 1 1
dump_his_log "${src_dir}/dfm_debug_log.1" ${log_dump} 1 1
sleep 0.005

# OSDump
os_dump="${1}/OSDump"
# sol串口日志,与集中式一致
packing "/dev/shm/bios_uart_print.tar.gz" "${os_dump}/bios_uart_print.tar.gz" $LOG_BIOS_MAX_LENGTH 0 0
if [ -f "${os_dump}/bios_uart_print.tar.gz" ]; then
    chown root:root "${os_dump}/bios_uart_print.tar.gz"
fi

# Register
register="${1}/Register"
packing "/dev/shm/cpld_reg_info" "${register}/cpld_reg_info" $LOG_CPLDREG_MAX_LENGTH 1 0

# 空文件
# 因为mpa周期检查BMC大小时，会以空文件为界限删除比这些
# 文件更新的压缩包, 若无这些文件，则会一直删除app*.gz直到只剩余两个,
# 出于兼容性考虑, touch这些文件，但内容为空, 不占空间
touch /tmp/BMCLOG/LogDump/app_debug_log
touch /tmp/BMCLOG/LogDump/maintenance_log
touch /tmp/BMCLOG/LogDump/strategy_log
touch /tmp/BMCLOG/LogDump/operate_log
touch /tmp/BMCLOG/LogDump/linux_kernel_log
touch /tmp/BMCLOG/LogDump/dfm_debug_log
touch /tmp/BMCLOG/LogDump/security_log

tar -zcvf /tmp/BMCLOG.tar.gz "${1}" > /dev/null 2>&1
chmod 640 /tmp/BMCLOG.tar.gz
chown ibc_os_hs:admin /tmp/BMCLOG.tar.gz
