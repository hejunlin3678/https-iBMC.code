#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install autoconf-archive-2022.09.03
# version: 1.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
# check install files
filename=autoconf-archive-2022.09.03
ls ${filename}.tar.gz
# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}

# install
mkdir -p ${install_dir}
rm -rf ${filename}
tar xzf ${filename}.tar.gz -C /opt/buildtools
mkdir -p /usr/local/share/aclocal
cp -rf ${install_dir}/m4/* /usr/local/share/aclocal/


# check version
ls -l /usr/local/share/aclocal/ax_compiler_vendor.m4