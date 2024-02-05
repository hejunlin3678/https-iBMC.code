#!/bin/bash
# 异常场景时, 分区目录文件无限增大写满分区, 导致挂死.
# 针对以上情况, 对`/data` `/dev/shm` `/opt/pme/pram` 3个分区进行最大文件大小的监控，超过阈值则删除
# 对`/data`分区, 要排除`/data/ibma` `/data/sp`分区目录
# 以下是各分区挂载情况
# ```bash
#    ~ # df -h
#    Filesystem                Size      Used Available Use% Mounted on
#    /dev/mmcblk0p5          464.5M    137.8M    298.3M  32% /
#    tmpfs                   202.4M         0    202.4M   0% /dev
#    tmpfs                   202.4M    376.0K    202.0M   0% /dev/shm
#    none                     96.0M      5.2M     90.8M   5% /tmp
#    /dev/mmcblk0p7            1.8G     26.6M      1.7G   1% /data
#    /dev/pramdisk0            7.7M      1.4M      6.3M  18% /opt/pme/pram
#    /dev/mmcblk0p3          306.6M     35.6M    271.0M  12% /data/ibma
#    /dev/mapper/mmcblksp      3.4G    428.3M      3.0G  12% /data/sp
# ```
DEFAULT_CHECK_PATH="/data"
DEFAULT_DIR_LIMIT_KiB="$((300 * 1024))" # 默认目录总大小闭区间上界, 单位是KiB

rm_if_exceed_limit() {
    # 删除`入参${1}:分区路径`下的文件中, 超过`入参${2}:最大大小限制,单位是KiB`的文件
    local check_dir="${1:-DEFAULT_CHECK_PATH}"
    local limit_KiB="${2:-DEFAULT_DIR_LIMIT_KiB}"

    if [ $1 == "/var/log/pme" ] ; then
		find ${check_dir} -perm -u=r -type f -size +${limit_KiB}k -type f -exec sh -c 'echo "" > "$1"' _ {} \; -exec sh -c 'echo "$(date "+%Y-%m-%d %H:%M:%S") partition_usage_monitor info: truncate "$1" to zero, file_size exceed limit." >>"/var/log/pme/app_debug_log_all"' _ {} \; 2>/dev/null
	else
        # 数据采集分区镜像文件不用删除
		find ${check_dir} ! -path '/data/sp/*' ! -path '/data/ibma/*' ! -path '/data/opt/pme/extern/data_acq.img' -perm -u=w -type f -size +${limit_KiB}k -exec sh -c 'echo "" > "$1"' _ {} \; -exec sh -c 'echo "$(date "+%Y-%m-%d %H:%M:%S") partition_usage_monitor info: truncate "$1" to zero, file_size exceed limit." >>"/var/log/pme/app_debug_log_all"' _ {} \; 2>/dev/null
	fi
}
main() {
    rm_if_exceed_limit "/dev/shm" 102400
    rm_if_exceed_limit "/data" 102400
    rm_if_exceed_limit "/opt/pme/pram" 2048
    rm_if_exceed_limit "/var/log/pme" 102400
}
main
