#!/bin/bash
# This script build sdk component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="SDK"

src_dir=${root_dir}/../../${produce_name}
output_path=${root_dir}/../../share_path/sdk_part1
mkdir -p ${output_path}

# SDK与rtos无关的部分
cp -rf ${src_dir}/Hi1711_chip_solution/build/package ${output_path}
cp -rf ${src_dir}/Hi1711_chip_solution/build/output ${output_path}

echo "==============postbuild end =================================="
