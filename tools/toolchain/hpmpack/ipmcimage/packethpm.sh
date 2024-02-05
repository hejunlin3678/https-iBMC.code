#!/bin/bash
# Package the firmware upgrade scripts in the IPMC and DATA areas.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

#使用方法:
#
#
#包组成:1、config.tar.gz 2、升级固件的升级文件
#
#调用本脚本的格式: packethpm.sh $1 $2 $3 $4 [$5]
#                 参数1:升级固件的升级文件 
#                 参数2:升级固件对应的app区
#				  参数3:升级固件对应的配置区
#                 参数4:hpm包的配置文件
#                 参数5:config.tar.gz 中的配置文件名,service包的话名字可能不一样,
#                       此参数可以不设置,不设置的话默认为:CfgFileList.conf
#                 如: packethpm.sh  ipmc.jffs2  app.jffs2 cfg.jffs2 hpm_ipmc.hpm CfgFileList.conf
#

str_success="\033[32;1m[success]\033[0m"
str_fail="\033[31;1m[Fail]\033[0m"
APPHPM="ipmcimage.hpm"
IPMCFILE="ipmc.jffs2"
APPFILE="app.jffs2"
CFGFILE="cfg.jffs2"
HPM="ipmc-crypt-image.hpm"
CONF="config.tar.gz"

rm -rf *.hpm

#1、参数1判断需要打包的升级文件是否正确
if [ ! -f $1 ]
then
   echo ERROR:$1 file is not exist!
   exit
fi
if [ ! -f $2 ]
then
   echo ERROR:$2 file is not exist!
   exit
fi

if [ ! -f $3 ]
then
   echo ERROR:$3 file is not exist!
   exit
fi

#2、参数2判断hpm包的配置文件是否存在
if [ ! -f $4 ]
then
   echo ERROR:$4 file is not exist!
   exit
fi

#3、参数3确认support.tar.gz 配置文件
if [ -n $5 ]
then
   if [ ! -f CfgFileList.conf ]
   then
      echo ERROR:CfgFileList.conf is not exist!
      exit
   fi
   CFGFILELIST=CfgFileList.conf
else
   if [ ! -f $5 ]
   then
      echo ERROR:$5 is not exist!
      exit
   fi
   CFGFILELIST=$5
fi

#4、判断filesizecheck是否存在,这个文件是保证升级配置文件的正确性,比较重要
if [ ! -f filesizecheck ]
then
   echo ERROR:filesizecheck is not exist!
   exit
fi
#增加可执行权限
chmod 777 filesizecheck

#5、判断脚本文件是否都存在
#确认beforeaction.sh
if [ ! -f beforeaction.sh ]
then
   echo ERROR:beforeaction.sh is not exist!
   exit
fi
chmod 777 beforeaction.sh
#确认afteraction.sh
if [ ! -f afteraction.sh ]
then
   echo ERROR:afteraction.sh is not exist!
   exit
fi
chmod 777 afteraction.sh

#确认update.cfg
if [ ! -f update.cfg ]
then
   echo ERROR:update.cfg is not exist!
   exit
fi

#6、判断打成hpm包的工具是否存在
if [ ! -f hpmimage ]
then
   echo ERROR:hpmimage is not exist!
   exit
fi

#7、校验support.tar.gz 中的配置文件为CfgFileList.conf或者为指定的参数3
./filesizecheck $CFGFILELIST > checkresult.txt
grep -wq "OK" checkresult.txt
if [ $? != 0 ]
then 
   cat checkresult.txt
   echo ERROR:filesizecheck failed!
   exit
fi
#校验完毕后正确的话删除临时文件checkresult.txt
rm checkresult.txt

#8、全部文件均ok后开始打包support.tar.gz
tar --format=gnu -czvf "$CONF" beforeaction.sh afteraction.sh $CFGFILELIST update.cfg
sleep 2

#8.1加密配置文件
chmod +x crypto_tool
./crypto_tool encrypt "$CONF" "conf.tar.gz"


#8.2加密升级文件
./crypto_tool encrypt "$1" "$IPMCFILE"
./crypto_tool encrypt "$2" "$APPFILE"
./crypto_tool encrypt "$3" "$CFGFILE"


#9、打成整个包
chmod +x hpmimage
./hpmimage $4
chmod 444 $APPHPM

#文件签名,签名密钥收编到公司平台,本地工具不需且不再支持签名
#./crypto_tool sign $APPHPM $HPM
mv -f $APPHPM $HPM

#11、判断是否已生成包
if [ ! -f $HPM ]
then
   echo -e "create ipmc update file: $HPM ----> $str_fail"
   exit
else
   echo -e "create ipmc update file: $HPM ----> $str_success"
fi
