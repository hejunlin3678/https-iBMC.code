#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install automake-1.16.5
# version: 1.16.5
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
# check install files
filename=automake-1.16.5
srcrpmfile=${filename}-4.oe2203sp1.src.rpm
ls ${srcrpmfile}

# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}

mkdir -p ${install_dir}
rpm -ivh ${srcrpmfile}
ls /usr/src/packages/SOURCES/${filename}.tar.xz
tar -xf /usr/src/packages/SOURCES/${filename}.tar.xz -C /opt/buildtools/
rm  -rf /usr/src/packages/SOURCES/${filename}.tar.xz

cd /opt/buildtools/${filename}
./configure
make -j$(nproc)
make install

rm -rf /opt/buildtools/${filename}

#check version
automake --version | head -1