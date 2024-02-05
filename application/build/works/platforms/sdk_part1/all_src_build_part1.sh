#!/bin/bash
# This script build sdk component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
# 此脚本为1711一键式入口脚本包括编译uboot,os,sdk,secure_m3,rootfs,tool,initrd,service_m3等，一个函数对应着一个编译模块，最后调取数字签名脚本，打包脚本
# 引入打包脚本和数字签名脚本
set -e
BUILD_DIR=$(pwd)
export BEP_PATH=/usr/local/lib/secBepkit1.3.1

module=$1  #[single,daily,version]
compile=$2 #[all,uboot,os,sdk,tool,initrd,rootfs,secure_m3,service_m3]
bep_switch=$3 #openbep,closebep


#首先创建编译产物归档路径
if [ ! -d ${BUILD_DIR}/../build/output ];then
    mkdir -p ${BUILD_DIR}/../build/output
fi
 
if [ ! -d ${BUILD_DIR}/../build/package/bin/original_bin ];then
    mkdir -p ${BUILD_DIR}/../build/package/bin/original_bin
fi

if [ ! -d ${BUILD_DIR}/../build/package/bin/unsigned_bin/sdk ];then
    mkdir -p ${BUILD_DIR}/../build/package/bin/unsigned_bin/sdk
fi 

if [ ! -d ${BUILD_DIR}/../build/package/update_bin/pme ];then
    mkdir -p ${BUILD_DIR}/../build/package/update_bin/pme
fi
if [ ! -d ${BUILD_DIR}/../build/package/update_bin/sdk ];then
   mkdir -p ${BUILD_DIR}/../build/package/update_bin/sdk
fi

function compile_uboot(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE UBOOT###################################\r\n"
    #拷贝一份代码，单独给u-boot编release版本
    mkdir ${BUILD_DIR}/../open_source/u-boot_
    cp -rf ${BUILD_DIR}/../open_source/u-boot/*  ${BUILD_DIR}/../open_source/u-boot_

    cd ${BUILD_DIR}/../build/build_uboot
    sh build_uboot.sh DEBUG_FW
    sleep 3
    mv ${BUILD_DIR}/../build/output/uboot.tar.gz  ${BUILD_DIR}/../build/output/uboot_debug.tar.gz
    sleep 3

    #使用未打patch版本编release
    mv ${BUILD_DIR}/../open_source/u-boot ${BUILD_DIR}/../open_source/u-boot_debug
    mv ${BUILD_DIR}/../open_source/u-boot_ ${BUILD_DIR}/../open_source/u-boot

    sh build_uboot.sh

    cp -rf ${BUILD_DIR}/../build/output/uboot.tar.gz ${BUILD_DIR}/../build/package/bin/original_bin
    cp -rf ${BUILD_DIR}/../build/output/uboot_debug.tar.gz ${BUILD_DIR}/../build/package/bin/original_bin
    cp -rf ${BUILD_DIR}/../build/output/u-boot.bin ${BUILD_DIR}/../build/package/bin/unsigned_bin/sdk
}

function compile_os(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE OS###################################\r\n"
    cd ${BUILD_DIR}/../build/build_rtos
    sh build_rtos.sh
    sleep 3
    cp -rf ${BUILD_DIR}/../build/output/* ${BUILD_DIR}/../build/package/bin/original_bin

}

function compile_sdk(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE SDK###################################\r\n"
    cd ${BUILD_DIR}/../build/build_sdk
    sh build_sdk.sh
    cp -rf ${BUILD_DIR}/../build/output/* ${BUILD_DIR}/../build/package/bin/original_bin
    cp -rf ${BUILD_DIR}/../build/output/hi1711sdk.tar.gz ${BUILD_DIR}/../build/package/update_bin/pme
    
}

function compile_tool(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE TOOL###################################\r\n"
    cd ${BUILD_DIR}/../build/build_tools
    sh build_tools.sh
    cp -rf ${BUILD_DIR}/../build/output/tools.tar.gz ${BUILD_DIR}/../build/package/bin/original_bin
}

function compile_initrd(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE INITRD###################################\r\n"
    cd ${BUILD_DIR}/../build/build_initrd
    #sed -i '2a tmp=$PATH' genrootfs1711
    #sed -i '3a export PATH=/opt/RTOS/207.6.0/bin/modules/bin/:$PATH' genrootfs1711
    #sed -i '10a export PATH=$tmp' genrootfs1711
    sh build_initrd.sh
    cp -rf ${BUILD_DIR}/../build/output/initrd_boot.cpio.gz ${BUILD_DIR}/../build/package/bin/original_bin

}

function compile_rootfs(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE ROOTFS###################################\r\n"
    cd ${BUILD_DIR}/../build/build_rootfs
    sh build_rootfs.sh
    cp -rf ${BUILD_DIR}/../build/output/rtos_original.tar.gz ${BUILD_DIR}/../build/package/bin/original_bin
    
}

function compile_secure_m3(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE SECURE_M3###################################\r\n"
    cd ${BUILD_DIR}/../build/build_security
    sh build_security.sh debug_fw
    sleep 5
    sh build_security.sh factory_fw
    sleep 2
    sh build_security.sh
    cp -rf ${BUILD_DIR}/../build/output/external_fw_debug.tar.gz ${BUILD_DIR}/../build/package/bin/original_bin
    cp -rf ${BUILD_DIR}/../build/output/external_fw.tar.gz ${BUILD_DIR}/../build/package/bin/original_bin
    cp -rf ${BUILD_DIR}/../build/output/external_fw_factory.tar.gz ${BUILD_DIR}/../build/package/bin/original_bin
    # cp -rf ${BUILD_DIR}/../build/output/index.log ${BUILD_DIR}/../build/package/bin/original_bin
}

function compile_service_m3(){
    printf "\r\n\r\n\r\n################################### Begin COMPILE SERVICE_M3###################################\r\n"
    cd ${BUILD_DIR}/../build/build_m3
    sh build_m3.sh
    cp -rf ${BUILD_DIR}/../build/output/* ${BUILD_DIR}/../build/package/bin/original_bin
    
}

function package_make_bin(){
    if [ ! -d ${BUILD_DIR}/../build/output/packet ];then 
        mkdir -p ${BUILD_DIR}/../build/output/packet
    fi
    if [ ! -d ${BUILD_DIR}/../build/package/bin/signed_bin/sdk ];then
        mkdir -p ${BUILD_DIR}/../build/package/bin/signed_bin/sdk
    fi
    
    cp -rf ${BUILD_DIR}/../build/package/bin/original_bin/* ${BUILD_DIR}/../build/output
    cd ${BUILD_DIR}/../build/build_packet
    sh build_packet.sh
    cp -rf ${BUILD_DIR}/../build/output/Hi1711_ext4fs.img ${BUILD_DIR}/../build/package/bin/signed_bin/sdk
    cp -rf ${BUILD_DIR}/../build/output/Hi1711_ext4fs.img ${BUILD_DIR}/../build/package/bin/unsigned_bin/sdk
    cp -rf ${BUILD_DIR}/../build/output/Hi1711_flash_ext4.bin ${BUILD_DIR}/../build/package/update_bin/sdk
    cp -rf ${BUILD_DIR}/../build/output/rtos.tar.gz ${BUILD_DIR}/../build/package/update_bin/pme
    
}

compile_uboot
#compile_os
compile_sdk
compile_tool
#compile_initrd
#compile_rootfs
compile_secure_m3
compile_service_m3

exit 0

if [ "${bep_switch}" == "openbep" ];then
    source bep_env.sh -u
fi

package_make_bin
if [ $? == 0 ];then
    echo "ALL COMPILE COMPLETE...................................................................................."
else
    exit 1
fi


sleep 5
printf "\r\n\r\n\r\n################################### Begin to sign###################################\r\n"
cd ${BUILD_DIR}
chmod +x 1711_sign_soc.sh 
chmod +x 1711_sign_cms.sh
sh 1711_sign_soc.sh 
if [ $? == 0 ];then
    echo "sign rsa successful................."
else
    exit 1
fi
sleep 3
printf "\r\n\r\n\r\n################################### Begin TO CMS###################################\r\n"
sh 1711_sign_cms.sh
if [ $? == 0 ];then
    echo "sign cms successful................."
else
    exit 1
fi
sleep 5

printf "\r\n\r\n\r\n################################### Begin PACKAGING###################################\r\n"
if [ ! -d ${BUILD_DIR}/../build/package/bin/signed_bin/secure_m3 ];then
    mkdir -p ${BUILD_DIR}/../build/package/bin/signed_bin/secure_m3
fi
#cp -rf ${BUILD_DIR}/../build/build_sign/l0fw*  ${BUILD_DIR}/../build/package/bin/signed_bin/secure_m3
#cp -rf ${BUILD_DIR}/../build/build_sign/l1fw*  ${BUILD_DIR}/../build/package/bin/signed_bin/secure_m3
chmod +x package_final.sh
sh package_final.sh
