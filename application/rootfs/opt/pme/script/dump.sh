
#用***替换文件中的敏感信息 参数: "key" "filename"
replace_sensitive_info()
{
    sed -i "s/^${1}.*$/${1}***/" $2
}


sleep 0.05
cd ${1}

#core文件
core_dump="${1}/CoreDump"
mkdir ${core_dump}

#压缩core文件
core_file_count=`ls /var/coredump/ | grep core- | wc -w`
if [ "$core_file_count" -gt 0 ]
then
    cd /var/coredump
    tar -zcvf ${core_dump}/core-files.tar.gz core-* > /dev/null 2>&1
fi

sleep 0.05
#脚本dump
rtos_dump="${1}/RTOSDump"
mkdir "$rtos_dump"

#系统信息
sysinfo="$rtos_dump/sysinfo"
mkdir "$sysinfo"
cd "$sysinfo"

cp /proc/meminfo ./meminfo > /dev/null 2>&1
cp /proc/cmdline ./cmdline > /dev/null 2>&1
cp /proc/cpuinfo ./cpuinfo > /dev/null 2>&1
#增加Hi1711平台收集的BMC CPU核信息
if [ ! -L "../../AppDump/BMC/cpuinfo" ]; then
	if [ -f "../../AppDump/BMC/cpuinfo" ]; then
		cat ../../AppDump/BMC/cpuinfo >> ./cpuinfo
		rm ../../AppDump/BMC/cpuinfo
	fi
fi
sleep 0.05
cp /proc/devices ./devices > /dev/null 2>&1
cp /proc/diskstats ./diskstats > /dev/null 2>&1
cp /proc/filesystems ./filesystems > /dev/null 2>&1
cp /proc/interrupts ./interrupts > /dev/null 2>&1
cp /proc/loadavg ./loadavg > /dev/null 2>&1
sleep 0.05
cp /proc/locks ./locks > /dev/null 2>&1
cp /proc/meminfo ./meminfo > /dev/null 2>&1
cp /proc/modules ./modules > /dev/null 2>&1
cp /proc/mtd ./mtd > /dev/null 2>&1
cp /proc/partitions ./partitions > /dev/null 2>&1
cp /proc/slabinfo ./slabinfo > /dev/null 2>&1
cp /proc/stat ./stat > /dev/null 2>&1
cp /proc/uptime ./uptime > /dev/null 2>&1
cp /proc/version ./version > /dev/null 2>&1
cp /proc/vmstat ./vmstat > /dev/null 2>&1
sleep 0.05
df > ./df_info
ps -elf > ./ps_info
free > ./free_info
top -bn 1 > ./top_info
uname -a > ./uname_info


busybox ipcs -q | grep -v Message | grep -v key  > ./ipcs_q
while read line
do
	if [ -n "$line" ]
	then
		echo "$line" | awk '{print $2}' | xargs busybox ipcs -q -i >> ./ipcs_q_detail
		echo "" >> ./ipcs_q_detail
	fi
done < "./ipcs_q"
sleep 0.05
busybox ipcs -s | grep -v Semaphore | grep -v key > ./ipcs_s
while read line
do
	if [ -n "$line" ]
	then
		echo "$line" | awk '{print $2}' | xargs busybox ipcs -s -i >> ./ipcs_s_detail
		echo "" >> ./ipcs_s_detail
	fi
done < "./ipcs_s"
sleep 0.05

#版本信息
verinfo="$rtos_dump/versioninfo"
mkdir "$verinfo"
cd "$verinfo"
platform_file=/opt/pme/conf/profile/platform.xml
bmc_name=`cat ${platform_file} | grep CustomLogIbmcName | awk -F '[<>]' '{print tolower($5)}'`
if [ "$bmc_name" = "" ] ; then
	bmc_name="ibmc"
fi
mv ../../AppDump/BMC/bmc_info ./
mv bmc_info ./app_revision.txt
cp /opt/pme/ibmc-revision.txt ./${bmc_name}_revision.txt > /dev/null 2>&1
cp /opt/pme/build_date.txt ./ > /dev/null 2>&1
cp /etc/RTOS-Release ./RTOS-Release > /dev/null 2>&1
cp /etc/RTOS-Revision ./RTOS-Revision > /dev/null 2>&1
cp ../../AppDump/FruData/fruinfo.txt ./

sleep 0.05

if [ ! -L "../../AppDump/CpuMem/cpu_info" ]; then
	if [  -f "../../AppDump/CpuMem/cpu_info" ]; then
		echo "---------------------------------------------------------------------------------Cpu  info----------------------------------------------------------------------------" >> server_config.txt
		cat ../../AppDump/CpuMem/cpu_info >> server_config.txt
		echo "" >> server_config.txt
	fi
fi

if [ ! -L "../../AppDump/CpuMem/mem_info" ]; then
	if [  -f "../../AppDump/CpuMem/mem_info" ]; then
		echo "--------------------------------------------------------------------------------Memory info----------------------------------------------------------------------------" >> server_config.txt
		cat ../../AppDump/CpuMem/mem_info >> server_config.txt
		echo "" >> server_config.txt
	fi
fi

if [ ! -L "../../AppDump/card_manage/card_info" ]; then
	if [  -f "../../AppDump/card_manage/card_info" ]; then
		echo "---------------------------------------------------------------------------------Card info----------------------------------------------------------------------------" >> server_config.txt
		cat ../../AppDump/card_manage/card_info >> server_config.txt
		echo "" >> server_config.txt
	fi
fi

if [ ! -L "../../AppDump/cooling_app/fan_info.txt" ]; then
	if [  -f "../../AppDump/cooling_app/fan_info.txt" ]; then
		echo "---------------------------------------------------------------------------------Fan  info----------------------------------------------------------------------------" >> server_config.txt
		cat ../../AppDump/cooling_app/fan_info.txt >> server_config.txt
		echo "" >> server_config.txt
	fi
fi

if [ ! -L "../../AppDump/BMC/psu_info.txt" ]; then
	if [  -f "../../AppDump/BMC/psu_info.txt" ]; then
		echo "---------------------------------------------------------------------------------PSU  info----------------------------------------------------------------------------" >> server_config.txt
		cat ../../AppDump/BMC/psu_info.txt >> server_config.txt
		echo "" >> server_config.txt
	fi
fi

if [ ! -L "../../AppDump/StorageMgnt/RAID_Controller_Info.txt" ]; then
	if [  -f "../../AppDump/StorageMgnt/RAID_Controller_Info.txt" ]; then
		echo "--------------------------------------------------------------------------------Storage info----------------------------------------------------------------------------" >> server_config.txt
		cat ../../AppDump/StorageMgnt/RAID_Controller_Info.txt >> server_config.txt
	fi
fi

sleep 0.05
#net
net_info="$rtos_dump/networkinfo"
mkdir "$net_info"
cd "$net_info"
ifconfig > ./ifconfig_info
cp ../../AppDump/NetConfig/net_info.txt ./
mv net_info.txt ./ipinfo_info
route > ./route_info
route -A inet6 >> ./route_info
netstat -an > ./netstat_info
cp /data/etc/dhclient.conf ./dhclient.conf > /dev/null 2>&1
cp /data/etc/dhclient6.conf ./dhclient6.conf > /dev/null 2>&1
cp /data/etc/dhclient6_ip.conf ./dhclient6_ip.conf > /dev/null 2>&1
cp /data/etc/dhclient_ip.conf ./dhclient_ip.conf > /dev/null 2>&1
cp /data/etc/resolv.conf ./resolv.conf > /dev/null 2>&1
cp /data/etc/services ./services > /dev/null 2>&1
cp /data/var/db/dhclient.leases ./dhclient.leases > /dev/null 2>&1
cp /data/var/db/dhclient6.leases ./dhclient6.leases > /dev/null 2>&1
cp /data/var/db/dhclient6_eth0.leases ./dhclient6_eth0.leases > /dev/null 2>&1
cp /data/var/db/dhclient6_eth1.leases ./dhclient6_eth1.leases > /dev/null 2>&1
cp /data/var/db/dhclient6_eth2.leases ./dhclient6_eth2.leases > /dev/null 2>&1
cp /data/var/dhcp/dhclient.leases ./_data_var_dhcp_dhclient.leases > /dev/null 2>&1
sleep 0.05

#driver
drv_info="$rtos_dump/driver_info"
mkdir "$drv_info"
cd "$drv_info"
dmesg > ./dmesg_info
lsmod > ./lsmod_info

echo "cat /proc/ekbox" >> ./kbox_info
cat /proc/ekbox >> ./kbox_info 2>&1
echo "" >> ./kbox_info
sleep 0.05
echo "cat /proc/kbox/regions/panic" >> ./kbox_info
cat /proc/kbox/regions/panic >> ./kbox_info 2>&1
echo "" >> ./kbox_info
echo "cat /proc/kbox/regions/dying" >> ./kbox_info
cat /proc/kbox/regions/dying >> ./kbox_info 2>&1

i=0
max_core_num=$(cat /proc/cpuinfo | grep processor | wc -l)
while [ $i -lt $max_core_num ]; do
    if [  -f "/proc/kbox/regions/ks_"$i ]; then
        echo "cat /proc/kbox/regions/ks_"$i >> ./kbox_info
        cat /proc/kbox/regions/ks_$i >> ./kbox_info 2>&1
        echo "" >> ./kbox_info
    fi
    i=$(( $i + 1 ))
done

if [  -f "/proc/kbox/regions/ks_main" ]; then
    echo "cat /proc/kbox/regions/ks_main" >> ./kbox_info
    cat /proc/kbox/regions/ks_main >> ./kbox_info 2>&1
    echo "" >> ./kbox_info
fi

echo "cat /sys/module/bmc_edma_drv/parameters/statistics" >> ./edma_drv_info
cat /sys/module/bmc_edma_drv/parameters/statistics >> ./edma_drv_info 2>&1

echo "cat /sys/module/bmc_cdev_drv/parameters/statistics" >> ./cdev_drv_info
cat /sys/module/bmc_cdev_drv/parameters/statistics >> ./cdev_drv_info 2>&1

echo "cat /sys/module/bmc_veth_drv/parameters/statistics" >> ./veth_drv_info
cat /sys/module/bmc_veth_drv/parameters/statistics >> ./veth_drv_info 2>&1

#web 配置文件
sleep 0.05
web_info="${1}/3rdDump"
cd "$web_info"
cp /opt/pme/web/conf/nginx_http_server.conf ./nginx_http_server.conf > /dev/null 2>&1
cp /opt/pme/web/conf/nginx_https_server.conf ./nginx_https_server.conf > /dev/null 2>&1
cp /opt/pme/web/conf/nginx_ssl_ciphersuite.conf ./nginx_ssl_ciphersuite.conf > /dev/null 2>&1
cp /opt/pme/web/conf/nginx_ssl_protocol.conf ./nginx_ssl_protocol.conf > /dev/null 2>&1
cp /opt/pme/web/conf/nginx_ssl_verify_client.conf ./nginx_ssl_verify_client.conf > /dev/null 2>&1
cp /opt/pme/web/conf/nginx.conf ./nginx.conf > /dev/null 2>&1
cp /opt/pme/web/conf/nginx_https_server_ext.conf ./nginx_https_server_ext.conf > /dev/null 2>&1
cp /opt/pme/web/conf/nginx_https_defualt_server_ext.conf ./nginx_https_defualt_server_ext.conf >/dev/null 2>&1



#other
other_info="$rtos_dump/other_info"
mkdir "$other_info"
cd "$other_info"
cp /data/etc/extern.conf ./extern.conf > /dev/null 2>&1
cp /data/etc/remotelog.conf ./remotelog.conf > /dev/null 2>&1
cp /data/etc/pam.d/login ./login > /dev/null 2>&1
cp /data/etc/pam.d/sshd ./sshd > /dev/null 2>&1
cp /data/etc/pam.d/vsftpd ./vsftpd > /dev/null 2>&1
cp /opt/pme/pram/pam_tally2 ./pam_tally2 > /dev/null 2>&1
cp /data/etc/ssh/sshd_config ./sshd_config > /dev/null 2>&1
cp /data/var/lib/logrotate.status ./logrotate.status > /dev/null 2>&1
cp /data/var/log/datafs_log ./datafs_log > /dev/null 2>&1
cp /data/etc/ntp.conf ./ntp.conf > /dev/null 2>&1


#日志dump
logdump="${1}/LogDump"
mkdir "$logdump"
cd "$logdump"
if [  -f "/dev/shm/bmccom.dat" ]; then
cp /dev/shm/bmccom.dat ./bmccom.dat -a > /dev/null 2>&1
rm /dev/shm/bmccom.dat > /dev/null 2>&1
fi
if [  -f "/dev/shm/uart3com.dat" ]; then
    cp /dev/shm/uart3com.dat ./uart3com.dat -a > /dev/null 2>&1
    rm /dev/shm/uart3com.dat > /dev/null 2>&1
fi
if [  -f "/dev/shm/uart4com.dat" ]; then
    cp /dev/shm/uart4com.dat ./uart4com.dat -a > /dev/null 2>&1
    rm /dev/shm/uart4com.dat > /dev/null 2>&1
fi
if [  -f "/dev/shm/imucom.dat" ]; then
cp /dev/shm/imucom.dat ./imucom.dat -a > /dev/null 2>&1
rm /dev/shm/imucom.dat > /dev/null 2>&1
fi
cp /var/log/pme/* ./  > /dev/null 2>&1
sleep 0.05
cp /data/var/log/third_party_file_bak.log ./ > /dev/null 2>&1
cp -rf /var/log/pme/lsw ./  > /dev/null 2>&1
cp -rf /var/log/pciecard ./  > /dev/null 2>&1
cp -rf /var/log/Retimer ./  > /dev/null 2>&1
cp -rf /var/log/netcard ./  > /dev/null 2>&1
cp -rf /var/log/socboard ./  > /dev/null 2>&1
sleep 0.05
(cd /var/log; cp -rf --parents $(find ./storage -type f ! -name "*.bin*") "$logdump";
cp -rf ./storage/ctrllog "$logdump/storage/") > /dev/null 2>&1
(find "$logdump"/storage/* -type d|xargs chmod 755) > /dev/null 2>&1
sleep 0.05
mkdir "${1}/AppDump" > /dev/null 2>&1
mkdir "${1}/AppDump/dfm" > /dev/null 2>&1
mv ./dfm* "${1}/AppDump/dfm" > /dev/null 2>&1
rm record.xml > /dev/null 2>&1
mkdir "${1}/BMALogDump" > /dev/null 2>&1
mv ./bma_debug_log* "${1}/BMALogDump" > /dev/null 2>&1

sleep 0.05

#M3日志dump
if [ -f /usr/bin/logs_tool ]; then
        mkdir "${1}/LogDump/M3LogDump" -p > /dev/null 2>&1
        cd /usr/bin/
        /usr/bin/logs_tool read /usr/bin/index.log ${1}/LogDump/M3LogDump/m3_log  ${1}/LogDump/M3LogDump/m3_log.1 > /dev/null 2>&1
        chmod 400 ${1}/LogDump/M3LogDump/m3_log
        chmod 400 ${1}/LogDump/M3LogDump/m3_log.1
        cd - > /dev/null 2>&1
fi

sleep 0.05

#sp日志dump
splogdump="${1}/SpLogDump"
mkdir "$splogdump"
cd "$splogdump"
if [  -d "/data/sp/spforbmc/spinfo/" ]; then
    spcapacity=$(du -sh  /data/sp/spforbmc/spinfo/ -k | awk  '{print $1}')
    #只有日志总大小小于10MB才进行拷贝
    if [ $spcapacity -lt 10000 ];then
        cp -r /data/sp/spforbmc/spinfo/* ./  > /dev/null 2>&1
    fi
fi

sleep 0.05

#x86 OS dump
osdump="${1}/OSDump"
mkdir -p "$osdump"
cd "$osdump"
#sol串口数据
#systemcom拷贝压缩包，和原来保持一致
cp /dev/shm/systemcom.tar ./systemcom.tar -a > /dev/null 2>&1
rm /dev/shm/systemcom.tar > /dev/null 2>&1

#计算型存储打包bios启动日志
if [ -f "/dev/shm/bios_uart_print.tar.gz" ]; then
    cp /dev/shm/bios_uart_print.tar.gz ./bios_uart_print.tar.gz > /dev/null 2>&1
    chmod 400 ./bios_uart_print.tar.gz
fi

#屏幕自动录像
cp /tmp/osreset/*.rep ./ -a > /dev/null 2>&1
cp /tmp/poweroff/*.rep ./ -a > /dev/null 2>&1
cp /data/share/video/*.rep ./ -a > /dev/null 2>&1
cp /data/share/video/*.info ./ -a > /dev/null 2>&1

sleep 0.05

#管理板日志
rimm_dump_dir="${1}/AppDump/rimm"
mv /tmp/dump_info_SMU* $rimm_dump_dir > /dev/null 2>&1

#设备调试信息
device_dump="${1}/DeviceDump"
mkdir "$device_dump"
cd "$device_dump"



###############################
#/opt/pme
###############################
opt_pme_dump="${1}/OptPme"
mkdir "$opt_pme_dump"
cd "$opt_pme_dump"

#一、pram目录
mkdir pram
cd pram
cp -a /opt/pme/pram/BIOS_FileName                    ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BIOS_OptionFileName              ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/BMC_dhclient.conf                ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BMC_dhclient.conf.md5            ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/BMC_dhclient.conf.sha256         ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BMC_dhclient6.conf               ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/BMC_dhclient6.conf.md5           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BMC_dhclient6.conf.sha256        ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/BMC_dhclient_ip.conf             ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BMC_dhclient_ip.conf.md5         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/BMC_dhclient_ip.conf.sha256      ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BMC_dhclient6_ip.conf            ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/BMC_dhclient6_ip.conf.md5        ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BMC_dhclient6_ip.conf.sha256     ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/NetConfig_dhclient.conf          ./ > /dev/null 2>&1
cp -a /opt/pme/pram/NetConfig_dhclient.conf.md5      ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/NetConfig_dhclient_ip.conf       ./ > /dev/null 2>&1
cp -a /opt/pme/pram/NetConfig_dhclient_ip.conf.md5   ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/BMC_HOSTNAME                     ./ > /dev/null 2>&1
cp -a /opt/pme/pram/BMC_HOSTNAME.md5                 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/BMC_HOSTNAME.sha256              ./ > /dev/null 2>&1
cp -a /opt/pme/pram/CpuMem_cpu_utilise               ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/CpuMem_mem_utilise               ./ > /dev/null 2>&1
cp -a /opt/pme/pram/memory_webview.dat               ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/cpu_utilise_webview.dat          ./ > /dev/null 2>&1
cp -a /opt/pme/pram/env_web_view.dat                 ./ > /dev/null 2>&1
sleep 0.05
cp -a /data/var/log/pme/fspeed.dat                   ./ > /dev/null 2>&1
cp -a /data/var/log/pme/fspeed_his.tar.gz            ./ > /dev/null 2>&1
sleep 0.05
cp -a /data/var/log/pme/sensor_record.dat            ./ > /dev/null 2>&1
cp -a /data/var/log/pme/sensor_record_his.tar.gz     ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/fsync_reg.ini                    ./ > /dev/null 2>&1
cp -a /opt/pme/pram/HwScan_EnvRecWave                ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/lost+found                       ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_cpu_diag_log               ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/md_so_cpu_diag_log.tar.gz        ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_app_debug_log              ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/md_so_app_debug_log.tar.gz       ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_ipmi_debug_log             ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/md_so_ipmi_debug_log.tar.gz      ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_maintenance_log            ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/md_so_maintenance_log.tar.gz     ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_mass_operate_log           ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/md_so_mass_operate_log.tar.gz    ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_operate_log                ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/md_so_operate_log.md5            ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_operate_log.sha256         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/md_so_operate_log.tar.gz         ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_strategy_log               ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/md_so_strategy_log.md5           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/md_so_strategy_log.sha256        ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/md_so_strategy_log.tar.gz        ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_config.ini                   ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_config.ini.md5               ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_config.ini.sha256            ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/per_config_permanent.ini         ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_config_permanent.ini.md5     ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_config_permanent.ini.sha256  ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_config_reset.ini             ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/per_config_reset.ini.bak         ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_config_reset.ini.bak.md5     ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_config_reset.ini.bak.sha256  ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_config_reset.ini.md5         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/per_config_reset.ini.sha256      ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_def_config.ini               ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_def_config.ini.md5           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_def_config.ini.sha256        ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/per_def_config_permanent.ini     ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_def_config_permanent.ini.md5 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_def_config_permanent.ini.sha256 ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_def_config_reset.ini         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/per_def_config_reset.ini.bak     ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_def_config_reset.ini.bak.md5 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_def_config_reset.ini.bak.sha256 ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_def_config_reset.ini.md5     ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/per_def_config_reset.ini.sha256  ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_power_off.ini                ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_power_off.ini.md5            ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_power_off.ini.sha256         ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_reset.ini                    ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_reset.ini.bak                ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/per_reset.ini.bak.md5            ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_reset.ini.bak.sha256         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/per_reset.ini.md5                ./ > /dev/null 2>&1
cp -a /opt/pme/pram/per_reset.ini.sha256             ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/pflash_lock                      ./ > /dev/null 2>&1
cp -a /opt/pme/pram/PowerMgnt_record                 ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/powerview.txt                    ./ > /dev/null 2>&1
cp -a /opt/pme/pram/proc_queue                       ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/ps_web_view.dat                  ./ > /dev/null 2>&1
cp -a /opt/pme/pram/sel.db                           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/sel_db_sync                      ./ > /dev/null 2>&1
cp -a /opt/pme/pram/semid                            ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/sensor_alarm_sel.bin             ./ > /dev/null 2>&1
cp -a /opt/pme/pram/sensor_alarm_sel.bin.md5         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/sensor_alarm_sel.bin.sha256      ./ > /dev/null 2>&1
cp -a /opt/pme/pram/sensor_alarm_sel.bin.tar.gz      ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/up_cfg                           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/User_login                       ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/User_login.md5                   ./ > /dev/null 2>&1
cp -a /opt/pme/pram/User_login.sha256                ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/User_sshd                        ./ > /dev/null 2>&1
cp -a /opt/pme/pram/User_sshd.md5                    ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/User_sshd.sha256                 ./ > /dev/null 2>&1
cp -a /opt/pme/pram/User_sshd_config                 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/User_sshd_config.md5             ./ > /dev/null 2>&1
cp -a /opt/pme/pram/User_sshd_config.sha256          ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/User_vsftp                       ./ > /dev/null 2>&1
cp -a /opt/pme/pram/User_vsftp.md5                   ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/User_vsftp.sha256                ./ > /dev/null 2>&1
cp -a /opt/pme/pram/eo.db                       	 ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/fdm_history.db                   ./ > /dev/null 2>&1
cp -a /opt/pme/pram/Snmp_snmpd.conf                  ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/Snmp_snmpd.conf.md5              ./ > /dev/null 2>&1
cp -a /opt/pme/pram/Snmp_snmpd.conf.sha256           ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/Snmp_http_configure              ./ > /dev/null 2>&1
cp -a /opt/pme/pram/Snmp_http_configure.md5          ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/Snmp_http_configure.sha256       ./ > /dev/null 2>&1
cp -a /opt/pme/pram/Snmp_https_configure             ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/Snmp_https_configure.md5         ./ > /dev/null 2>&1
cp -a /opt/pme/pram/Snmp_https_configure.sha256      ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/Snmp_https_tsl                   ./ > /dev/null 2>&1
cp -a /opt/pme/pram/Snmp_https_tsl.md5               ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/Snmp_https_tsl.sha256            ./ > /dev/null 2>&1
cp -a /opt/pme/pram/NPU1-1_hbm_webview.dat           ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/NPU1-2_hbm_webview.dat           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/NPU2-1_hbm_webview.dat           ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/pram/NPU2-2_hbm_webview.dat           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/NPU3-1_hbm_webview.dat           ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/pram/NPU3-2_hbm_webview.dat           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/NPU4-1_hbm_webview.dat           ./ > /dev/null 2>&1
cp -a /opt/pme/pram/NPU4-2_hbm_webview.dat           ./ > /dev/null 2>&1
sleep 0.05

ls -al > filelist
sleep 0.05
#带敏感信息的文件处理1: 指定条目替换为*
#Snmp_snmpd.conf
replace_sensitive_info "com2sec rocomm default " ./Snmp_snmpd.conf
replace_sensitive_info "com2sec rwcomm default " ./Snmp_snmpd.conf
replace_sensitive_info "com2sec6 rocomm default " ./Snmp_snmpd.conf
replace_sensitive_info "com2sec6 rwcomm default " ./Snmp_snmpd.conf
replace_sensitive_info "usmUser " ./Snmp_snmpd.conf

sleep 0.05

#将ps_web_view.dat数据转换成csv格式，并copy到PowerMgnt目录下
cp  ps_web_view.dat power_statistics.csv
#删除第一行数据
sed -i '1d' power_statistics.csv
#在第一行加上表头
sed -i '1i\Time, Current Power, Average Power, Peak Power' power_statistics.csv
#把#替换成,
sed -i 's/#/,/g' power_statistics.csv
#把文件移到PowerMgnt文件夹中
mv  power_statistics.csv  ../../AppDump/PowerMgnt/

sleep 0.05

#二、save目录
cd "$opt_pme_dump"
mkdir save
cd save

cp -a /opt/pme/save/BIOS_FileName                          ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient.conf.bak                  ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/BMC_dhclient.conf.bak.md5              ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient.conf.bak.sha256           ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/BMC_dhclient.conf.md5                  ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient.conf.sha256               ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/BMC_dhclient6.conf.bak                 ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient6.conf.bak.md5             ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/BMC_dhclient6.conf.bak.sha256          ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient6.conf.md5                 ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/BMC_dhclient6.conf.sha256              ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient_ip.conf.bak               ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/BMC_dhclient_ip.conf.bak.md5           ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient_ip.conf.bak.sha256        ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/BMC_dhclient_ip.conf.md5               ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient_ip.conf.sha256            ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/BMC_dhclient6_ip.conf.bak              ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient6_ip.conf.bak.md5          ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/BMC_dhclient6_ip.conf.bak.sha256       ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_dhclient6_ip.conf.md5              ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/BMC_dhclient6_ip.conf.sha256           ./ > /dev/null 2>&1
cp -a /opt/pme/save/NetConfig_dhclient.conf.bak            ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/NetConfig_dhclient.conf.bak.md5        ./ > /dev/null 2>&1
cp -a /opt/pme/save/NetConfig_dhclient.conf.md5            ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/NetConfig_dhclient_ip.conf.bak         ./ > /dev/null 2>&1
cp -a /opt/pme/save/NetConfig_dhclient_ip.conf.bak.md5     ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/NetConfig_dhclient_ip.conf.md5         ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_HOSTNAME.bak                       ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/BMC_HOSTNAME.bak.md5                   ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_HOSTNAME.bak.sha256                ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/BMC_HOSTNAME.md5                       ./ > /dev/null 2>&1
cp -a /opt/pme/save/BMC_HOSTNAME.sha256                    ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/CpuMem_cpu_utilise                     ./ > /dev/null 2>&1
cp -a /opt/pme/save/CpuMem_mem_utilise                     ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/HwScan_EnvRecWave                      ./ > /dev/null 2>&1
cp -a /opt/pme/save/md_so_operate_log.bak                  ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/md_so_operate_log.bak.md5              ./ > /dev/null 2>&1
cp -a /opt/pme/save/md_so_operate_log.bak.sha256           ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/md_so_operate_log.md5                  ./ > /dev/null 2>&1
cp -a /opt/pme/save/md_so_operate_log.sha256               ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/md_so_strategy_log.bak                 ./ > /dev/null 2>&1
cp -a /opt/pme/save/md_so_strategy_log.bak.md5             ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/md_so_strategy_log.bak.sha256          ./ > /dev/null 2>&1
cp -a /opt/pme/save/md_so_strategy_log.md5                 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/md_so_strategy_log.sha256              ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_config.ini                         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/per_config.ini.bak                     ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_config.ini.bak.md5                 ./ > /dev/null 2>&1

cp -a /opt/pme/save/per_config.ini.bak.sha256              ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_config.ini.md5                     ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/per_config.ini.sha256                  ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_def_config.ini                     ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/per_def_config.ini.bak                 ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_def_config.ini.bak.md5             ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/per_def_config.ini.bak.sha256          ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_def_config.ini.md5                 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/per_def_config.ini.sha256              ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_power_off.ini                      ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/per_power_off.ini.bak                  ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_power_off.ini.bak.md5              ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/per_power_off.ini.bak.sha256           ./ > /dev/null 2>&1
cp -a /opt/pme/save/per_power_off.ini.md5                  ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/per_power_off.ini.sha256               ./ > /dev/null 2>&1
cp -a /opt/pme/save/PowerMgnt_record                       ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/sensor_alarm_sel.bin                   ./ > /dev/null 2>&1
cp -a /opt/pme/save/sensor_alarm_sel.bin.bak               ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/sensor_alarm_sel.bin.bak.md5           ./ > /dev/null 2>&1
cp -a /opt/pme/save/sensor_alarm_sel.bin.bak.sha256        ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/sensor_alarm_sel.bin.md5               ./ > /dev/null 2>&1
cp -a /opt/pme/save/sensor_alarm_sel.bin.sha256            ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/sensor_alarm_sel.bin.tar.gz            ./ > /dev/null 2>&1
cp -a /opt/pme/save/eo.db                                  ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/backup/eo.db               ./eo.db_backup > /dev/null 2>&1
cp -a /opt/pme/save/hash/master/eo.db.sha256    ./eo.db.sha256 > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/hash/backup/eo.db.sha256   ./eo.db.sha256_backup > /dev/null 2>&1
cp -a /opt/pme/save/backup/fdm_history.db       ./fdm_history.db_backup > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/hash/master/fdm_history.db.sha256      ./fdm_history.db.sha256 > /dev/null 2>&1
cp -a /opt/pme/save/fdm_history.db                         ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/hash/backup/fdm_history.db.sha256   ./fdm_history.db.sha256_backup > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_snmpd.conf.sha256                 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/Snmp_snmpd.conf.bak                    ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_snmpd.conf.bak.md5                ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/Snmp_snmpd.conf.bak.sha256             ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_snmpd.conf.md5                    ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/Snmp_http_configure.bak                ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_http_configure.bak.md5            ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/Snmp_http_configure.bak.sha256         ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_http_configure.md5                ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/Snmp_http_configure.sha256             ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_https_configure.bak               ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/Snmp_https_configure.bak.md5           ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_https_configure.bak.sha256        ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/Snmp_https_configure.md5               ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_https_configure.sha256            ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_https_tsl.bak                     ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_https_tsl.bak.md5                 ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/Snmp_https_tsl.bak.sha256              ./ > /dev/null 2>&1
cp -a /opt/pme/save/Snmp_https_tsl.md5                     ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/Snmp_https_tsl.sha256                  ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_login.bak                         ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/User_login.bak.md5                     ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_login.bak.sha256                  ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/User_login.md5                         ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_login.sha256                      ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/User_sshd.bak                          ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_sshd.bak.md5                      ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/User_sshd.bak.sha256                   ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_sshd.md5                          ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/User_sshd.sha256                       ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_sshd_config.bak                   ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_sshd_config.bak.md5               ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_sshd_config.bak.sha256            ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/User_sshd_config.md5                   ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_sshd_config.sha256                ./ > /dev/null 2>&1
sleep 0.05
cp -a /opt/pme/save/User_vsftp.bak                         ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_vsftp.bak.md5                     ./ > /dev/null 2>&1
sleep 0.05

cp -a /opt/pme/save/User_vsftp.bak.sha256                  ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_vsftp.md5                         ./ > /dev/null 2>&1
cp -a /opt/pme/save/User_vsftp.sha256                      ./ > /dev/null 2>&1

sleep 0.05

ls -al > filelist

#带敏感信息的文件处理1: 指定条目替换为*
#Snmp_snmpd.conf
replace_sensitive_info "com2sec rocomm default " ./Snmp_snmpd.conf.bak
replace_sensitive_info "com2sec rwcomm default " ./Snmp_snmpd.conf.bak
replace_sensitive_info "com2sec6 rocomm default " ./Snmp_snmpd.conf.bak
replace_sensitive_info "com2sec6 rwcomm default " ./Snmp_snmpd.conf.bak
replace_sensitive_info "usmUser " ./Snmp_snmpd.conf.bak
sleep 0.05


