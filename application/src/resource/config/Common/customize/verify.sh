#!/bin/sh
chmod +x *
############################################
# bash script: 定制化功能定制检测脚本
# Usage: ./verify.sh customset.ini serial.ini 05020VNL
# Author: w00208253
############################################
#赋值项目编码
project_code=$3

VERSIONNAME=$project_code"_version.ini"

#解压依赖包
tar zxvf function.tar.gz > /dev/null 2>&1

#加载公共脚本
source ./function/common.sh
#复位BMC
reset_bmc

init_common $1 $2 verify

#定制化电子标签信息
source ./function/assets_check.sh

#定制化用户信息
source ./function/user_check.sh

#定制化系统信息
source ./function/sys_check.sh

#定制化网络信息
source ./function/net_check.sh

#定制化电源能耗等信息
source ./function/power_check.sh

#定制化LDAP信息
source ./function/LDAP_check.sh

#定制化机箱SN同步到主板比较耗时，如果提前校验SN可能未能同步完成，等待其他校验操完后再校验
source ./function/chassis_sn_check.sh

#hostname在这个脚本才写入和验证，验证OK后对配置项设置备份点
set_bak_point

#生产加工临时文件清理
source ./function/file_restore.sh