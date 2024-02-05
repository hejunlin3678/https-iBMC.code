#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install openEuler:gperf-3.1-7.oe2203sp1.src.rpm
# version: 1.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
# check install files
ls gperf-3.1-7.oe2203sp1.src.rpm

# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

tmp_cpus=$(grep -w processor /proc/cpuinfo|wc -l)

rpm2cpio gperf-3.1-7.oe2203sp1.src.rpm | cpio -di

ls gperf-3.1.tar.gz
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

tar xvf gperf-3.1.tar.gz
pushd gperf-3.1
./configure --prefix="${install_dir}"
make
make install
popd
rm -rf gperf-3.1

#link
ln -sf ${install_dir}/bin/* /usr/local/bin