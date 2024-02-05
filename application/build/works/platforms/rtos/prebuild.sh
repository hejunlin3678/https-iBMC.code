#!/bin/bash
# This script build rtos component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============prebuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="RTOS"

cd ${root_dir}
if [ "$1" = "-o" ]; then
    python3 works/platforms/download_from_cmc_SrcBaseline.py -p ${produce_name} -x ${cur_dir}/srcbaseline.xml -o
fi
echo "==============prebuild end =================================="