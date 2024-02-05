#!/bin/sh
chmod +x *
############################################
# bash script: 定制化配置脚本
# Usage: ./config.sh customset.ini serial.ini 05020VNL
# Author: w00208253
############################################
#把项目编码赋值好
#输入参数
#$1  customset.ini的内容就是BOQ的信息  
#$2 serial.ini的内容就是一个整机条码 
#$3 项目编码 
project_code=$3
VERSIONNAME=$project_code"_version.ini"
BLADE_TYPE=1
CHASSIS_TYPE=0

#每次修改脚本必须修改版本号及readme
version_out()
{
    echo -e "\033[31;1m ############################################ \033[0m"
	echo -e "\033[31;1m # bash script: config.sh                   # \033[0m" 
	echo -e "\033[31;1m # Version:     V1.28                       # \033[0m" 
	echo -e "\033[31;1m ############################################ \033[0m" 
}

#打印版本信息
version_out 

#解压依赖包
tar zxvf function.tar.gz > /dev/null 2>&1

#加载公共脚本
source ./function/common.sh
init_common $1 $2 config

#清除白牌信息,包括logo，证书，传感器定制文件
need_special_dispose
if [ $? -eq 1 ];
then
	clear_white_branding_config
fi

#定制化电子标签信息
source ./function/assets_config.sh

#定制化用户信息
source ./function/user_config.sh

#定制化系统信息
source ./function/sys_config.sh

#定制化网络信息
source ./function/net_config.sh

#定制化电源能耗等信息
source ./function/power_config.sh

#定制化LDAP信息
source ./function/LDAP_config.sh





