#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
# script for install yocto project-poky
# version: 4.0.2
# change log:
# ***********************************************************************
set -eux
set -o pipefail

software=poky-yocto-4.0.2

install_dir=$1
install_dir=/opt/buildtools/poky-yocto-4.0.2

# check install files
ls ${software}.tar.gz

# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

tar -zxf ${software}.tar.gz --strip-components 1 -C ${install_dir}