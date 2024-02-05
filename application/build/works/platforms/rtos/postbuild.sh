#!/bin/bash
# This script build rtos component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="RTOS"

src_dir=${root_dir}/../../${produce_name}
# 结果仅给SDK使用，不需要放platforms目录下
output_path=${root_dir}/../../share_path/${produce_name}

mkdir -p ${output_path}
cp -rf ${src_dir}/output/rtos_dist/arm64le_5.10_ek_preempt_pro-suse12.5/*  ${output_path}

echo "==============postbuild end =================================="
