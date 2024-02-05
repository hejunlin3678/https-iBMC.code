#!/bin/bash
# This script build pme component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="PME"

src_dir=${root_dir}/../../${produce_name}
if [ "$1" == "V5" ]
then
    output_path=${root_dir}/../../share_path/platforms/platform_v5/Platform_V5
else
    output_path=${root_dir}/../../share_path/platforms/platform_v6
fi

mkdir -p ${output_path}
cp -rf ${src_dir}/pme_code/framework/build/output/*.tar.gz ${output_path}

echo "==============postbuild end =================================="
