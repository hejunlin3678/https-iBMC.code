#!/bin/bash
# This scripts bulid huawei PME BMC.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

arg=$1

# this file need
G_OS_TYPE=""
G_WORK_DIR=$(pwd)
G_ARCH="$1"  # valid option: "", "arm"
G_STEP="$2"  # valid option: "", "build", "install"
G_STRIP=arm32-linux-gnueabi-strip

if [ $arg == 1 ]; then
    array=$(ls ${G_WORK_DIR}/../../toolchain/itest)
    #
    # 复制so库到rootfs
    #
    cp ${G_WORK_DIR}/../../toolchain/itest/lib/libiTest_linux_arm_32.so ${G_WORK_DIR}/../rootfs/opt/pme/lib/
    #chown root:root ${G_WORK_DIR}/../rootfs/opt/pme/lib/libiTest_linux_arm_32.so
    #chmod 744 ${G_WORK_DIR}/../rootfs/opt/pme/lib/libiTest_linux_arm_32.so

    #
    # 复制itest代码文件到各目录下
    #
    for a in $array; do
	#echo "${G_WORK_DIR}/../toolchain/itest/$a"
	#echo "${G_WORK_DIR}/../$a"
	#echo "cp -rfa ${G_WORK_DIR}/../toolchain/itest/$a/* ${G_WORK_DIR}/../$a/*"
	cp -rfa ${G_WORK_DIR}/../../toolchain/itest/$a/* ${G_WORK_DIR}/../../$a/ > /dev/null 2>&1
    done
elif [ $arg ==  2 ]; then
    $(find ${G_WORK_DIR}/../apps/ -name "ibmc_itest*" | xargs rm -f) > /dev/null 2>&1
    $(find ${G_WORK_DIR}/../libs/ -name "ibmc_itest*" | xargs rm -f) > /dev/null 2>&1    
    $(find ${G_WORK_DIR}/../user_interface/ -name "ibmc_itest*" | xargs rm -f) > /dev/null 2>&1
fi
echo "done!"
	
