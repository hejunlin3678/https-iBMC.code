#!/bin/bash
# This script build sdk component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="SiteAI"

src_dir=${root_dir}/../../${produce_name}

# 临时目录
tmp_tar=${root_dir}/../../${produce_name}/DOPRA_DOPAI_MASTER/../tmp_tar
# tools
mkdir -p ${tmp_tar}/tools/cml_converter/fbs
cp -rf ${src_dir}/DOPRA_DOPAI_MASTER/src/machine_learning/CML.fbs ${tmp_tar}/tools/cml_converter/fbs
# src
mkdir -p ${tmp_tar}/src/cml/rtos_arm64a55le
cp -rf ${src_dir}/DOPRA_DOPAI_MASTER/output/src/rtos_arm64a55le_Release/cml/* ${tmp_tar}/src/cml/rtos_arm64a55le

# 压缩打包
#新编译框架需在20220501后的版本中生效
output_path=${root_dir}/../../share_path/platforms/site_ai
mkdir -p ${output_path}
tar -zcf ${output_path}/"SiteAI V100R023C00B500.tar.gz" -C ${src_dir}/output/siteai .
echo "==============postbuild end =================================="
