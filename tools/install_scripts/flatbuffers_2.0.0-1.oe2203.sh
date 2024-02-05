#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install flatbuffers
# version: 2.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
# check install files

filename=flatbuffers-2.0.0-1.oe2203
rpm2cpio ${filename}.src.rpm | cpio -di
ls *.tar.gz
# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}

mkdir -p ${install_dir}

rm -rf flatbuffers-2.0.0
tar xf v2.0.0.tar.gz
rm -rf v2.0.0.tar.gz

cd flatbuffers-2.0.0
sed -i 's/-Werror -Wextra -Werror=shadow/-Wextra/g' CMakeLists.txt 
mkdir build && cd build && cmake .. && make install

#create link
ln -sf /usr/local/bin/flatc /usr/bin/


#check version
flatc --version