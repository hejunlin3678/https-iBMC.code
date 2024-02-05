#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
# script for install BiSheng_Embedded_CPU_301.0.0.B015
# version: 1.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail
 
install_dir=$1
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}
cp -rf hcc_*.tar.gz ${install_dir}/

tar -zxf  hcc_arm64le.tar.gz -C ${install_dir}
cp -rf ${install_dir}/* /opt
ln -sf /opt/hcc_arm64le/bin/aarch64-linux-gnu-strip /usr/bin/aarch64-linux-gnu-strip
