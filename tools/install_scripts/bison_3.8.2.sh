#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
# script for install bison
# version: 3.8.2
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
# check install files
FILE_NAME=bison-3.8.2.tar.xz
DIR_NAME=bison-3.8.2

ls ${FILE_NAME}
# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

tar xf ${FILE_NAME}
cd ${DIR_NAME}
export M4=/usr/bin/m4
./configure
make -j$(nproc)
make install

#create link
ln -sf /usr/local/bin/bison /usr/bin/

#check version
bison --version
