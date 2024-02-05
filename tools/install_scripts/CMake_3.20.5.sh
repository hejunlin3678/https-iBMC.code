#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install cmake
# version: 3.20.5
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
FILE_NAME=cmake-3.20.5.tar.gz
DIR_NAME=cmake-3.20.5

# check install files
ls ${FILE_NAME}
# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

tar xf ${FILE_NAME}
pushd ${DIR_NAME}
sed -i 's#cmake_options="-DCMAKE_BOOTSTRAP=1"#cmake_options="-DCMAKE_BOOTSTRAP=1 -DCMAKE_BUILD_TYPE=Release"#1' bootstrap
./configure --prefix=${install_dir}
make -j$(nproc)
make install
popd
rm -rf ${DIR_NAME}
#link
ln -sf ${install_dir}/bin/* /usr/local/bin

echo "Successfully installed the tool cmake."

