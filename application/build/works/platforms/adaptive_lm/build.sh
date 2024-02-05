#!/bin/bash
# This script build adaptive_lm component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="AdaptiveLM"

src_dir=${root_dir}/../../${produce_name}
export PATH=/opt/hcc_arm64le/bin/:$PATH

cd ${src_dir}/AdaptiveLM/c-intf/build/scripts
sh build.sh iBMC_suse12sp5_arm64le_hi1711 dynamic

echo "==============build end =================================="
