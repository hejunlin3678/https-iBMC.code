#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install arm-himix100-linux
# version: 1.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1

# clear
[-d "${install_dir}/tmp"] && rm -rf ${install_dir}/tmp
mkdir -p ${install_dir}/tmp

# Check the correct location of the tool installation location and its version
ls arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz

sudo tar -xvf arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz -C $1

rm -rf ${install_dir}/tmp

ln -sf ${install_dir}/bin/* /usr/local/bin