#!/bin/bash
# This scripts bulid huawei PME BMC.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

set -e

BUILD_PATH=$(pwd)
# this file need
G_OS_TYPE=""
G_WORK_DIR=$(pwd)
G_STRIP=strip


build() {
    rm -rf ${G_WORK_DIR}/target >/dev/null 2>&1
    mkdir ${G_WORK_DIR}/target

    cd ${G_WORK_DIR}/target

    #copy test file to target directory
    rm -rf ${G_WORK_DIR}/../src/user_interface/cli/dft_user_tool
    cp -r ${G_WORK_DIR}/../../tools/dft_tools/src/dft_user_tool ${G_WORK_DIR}/../src/user_interface/cli/

    rm -rf ${G_WORK_DIR}/../src/user_interface/cli/soltool
    cp -r ${G_WORK_DIR}/../../tools/dft_tools/src/soltool ${G_WORK_DIR}/../src/user_interface/cli/

    make_1710_package

    make -j 2>&1
    if [ $? != 0 ]; then
        exit 2
    fi

    make install
    if [ $? != 0 ]; then
        exit 2
    fi

}

make_1710_package() {
   # make sure CMAKE_LIBRARY_PATH is right.
    export CMAKE_LIBRARY_PATH=${G_WORK_DIR}/../open_source/lib
    source /opt/RTOS/dlsetenv.sh -p armA9le_3.10_ek -v V100R005C00
    cmake -DCMAKE_TOOLCHAIN_FILE=../arch/arm/arm-linux-gnueabi-ultimate.cmake  -DCMAKE_INSTALL_PREFIX=${G_WORK_DIR}/target/tmp_root -DWITH_DFT_TOOL=on ../..
}

# this is main
build;

exit 0


