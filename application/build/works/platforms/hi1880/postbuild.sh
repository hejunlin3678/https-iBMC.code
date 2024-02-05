#!/bin/bash
# This script build HI1880 component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="HI1880"

src_dir=${root_dir}/../../${produce_name}

output_path=${root_dir}/../../share_path/platforms/hs/host/arm
output_path_local=${root_dir}/../../temp/platforms/hs/host/arm
mkdir -p ${output_path}
mkdir -p ${output_path_local}

cp -rf ${src_dir}/hi1880_code/src/include/open_api/* ${output_path}
cp -rf ${src_dir}/hi1880_code/src/host/tools/lib/tool_lib.h ${output_path}
cp -rf ${src_dir}/hi1880_code/src/host/tools/lib/bin/* ${output_path}

cp -rf ${src_dir}/hi1880_code/src/include/open_api/* ${output_path_local}
cp -rf ${src_dir}/hi1880_code/src/host/tools/lib/tool_lib.h ${output_path_local}
cp -rf ${src_dir}/hi1880_code/src/host/tools/lib/bin/* ${output_path_local}

echo "==============postbuild end =================================="
