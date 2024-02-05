#!/bin/bash
# This script build hi1880 component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
CURRENT_PATH=$(readlink -f "$(dirname "$0")")
root_dir=${CURRENT_PATH}/../../..
produce_name="HI1880"

WORKSPACE=${root_dir}/../../${produce_name}
export PATH=/opt/hcc_arm64le/bin/:$PATH

cd ${WORKSPACE}/hi1880_code/src/host/tools/lib
cp -rf /opt/hcc_arm64le /opt/buildtools/hcc_arm64le
sed -i '69c all : $(OBJS64LE)' Makefile
sed -i '70,72d' Makefile
make

echo "==============build end =================================="
