#!/bin/bash
# Updating environment variables and compiling open source software
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

G_VERSION="$1"
G_CPU="$2"
G_KERNEL="$3"
CROSS_COMPILE_INSTALL_PATH="$4"
G_WORK_DIR_DEFAULT="$5"
G_FLAG_ARM64="$6"
G_PART_NAME="$7"

function set_evn() {

    # 生效编译工具链
    source /opt/RTOS/${G_VERSION}/dlsetenv.sh -p ${G_CPU}_${G_KERNEL} --sdk-path=${CROSS_COMPILE_INSTALL_PATH}
}

function build() {
    cd ${G_WORK_DIR_DEFAULT}/open_source
    if [ ${G_FLAG_ARM64} -eq "0" ]; then
        sh build.sh "${G_PART_NAME}"
    else
        sh build64.sh "${G_PART_NAME}"
    fi

}

set_evn
build
