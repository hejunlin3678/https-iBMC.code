#!/bin/bash
# This script build sdk component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="SiteData"
src_dir=${root_dir}/../../${produce_name}
# 临时目录
tmp_tar=${root_dir}/../../${produce_name}/SiteData/../tmp_tar
# src
mkdir -p ${tmp_tar}/src/cdf/rtos_arm64a55le
cp -rf ${src_dir}/SiteData/output/src/rtos_arm64a55le_Release/cdp/cdf/* ${tmp_tar}/src/cdf/rtos_arm64a55le
# 压缩打包
#新编译框架需在20220501后的版本中生效
output_path=${root_dir}/../../share_path/platforms/site_data
mkdir -p ${output_path}
tar -zcf ${output_path}/"SiteData V100R023C00B500.tar.gz" -C ${src_dir}/output/sitedata .
echo "==============postbuild end =================================="