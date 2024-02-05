#!/bin/bash
# This script build adaptive_lm component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="CBB"

src_dir=${root_dir}/../../${produce_name}/CMSCBB_SourceCodeList/CMS-C

output_path=${root_dir}/../../share_path/cms_vrf_cbb
output_path_local=${root_dir}/../../temp/cms_vrf_cbb

[ -n "${output_path}" ] && rm -rf ${output_path}
mkdir -p ${output_path}
[ -n "${output_path_local}" ] && rm -rf ${output_path_local}
mkdir -p ${output_path_local}

# 一份传递出去，一份本地使用
cp -rf ${src_dir}/* ${output_path}
cp -rf ${src_dir}/* ${output_path_local}

echo "==============postbuild end =================================="
