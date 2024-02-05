#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
# script for install ninja-1.11.0
# version: 1.11.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
ls ninja-1.11.0.tar.gz

[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

mkdir -p "${install_dir}/bin"
tar xvf ninja-1.11.0.tar.gz
pushd ninja-1.11.0
sed -ie '1c#!/usr/bin/env python3' ./*.py
./configure.py --bootstrap
install -m 0755 ./ninja "${install_dir}/bin/"
ln -s ${install_dir}/bin/ninja /usr/local/bin/ninja
popd
rm -rf ninja-1.11.0