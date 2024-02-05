#!/bin/bash
# This script build adaptive_lm component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="AdaptiveLM"

src_dir=${root_dir}/../../${produce_name}
output_path=${root_dir}/../../share_path/platforms/AdaptiveLM

mkdir -p ${output_path}
cp -rf ${src_dir}/AdaptiveLM/c-intf/output/* ${output_path}/arm64
cp -rf ${src_dir}/AdaptiveLM/c-intf/include  ${output_path}/include

echo "==============postbuild end =================================="
