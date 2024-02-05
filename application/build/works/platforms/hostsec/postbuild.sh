#!/bin/bash
# This script build hostsec component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="HostSec"

src_dir=${root_dir}/../../${produce_name}
output_path=${root_dir}/../../share_path/platforms/${produce_name}

mkdir -p ${output_path}
cp -rf ${src_dir}/hisec_ict_ne/hostsec/output/iBMC/* ${output_path}

echo "==============postbuild end =================================="
