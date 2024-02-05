#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install patchelf-0.15.0
# version: 1.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1

[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

tmp_cpus=$(grep -w processor /proc/cpuinfo|wc -l)

ls patchelf-0.15.0.tar.gz
tar -zxf patchelf-0.15.0.tar.gz
soft_dir="patchelf-0.15.0"
pushd "${soft_dir}"
# ./bootstrap.sh
./configure --prefix=${install_dir}
make -j ${tmp_cpus} 
make install
popd
rm -rf "${soft_dir}"