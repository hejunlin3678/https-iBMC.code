#!/bin/bash
# This script build site_data component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="SiteData"

src_dir=${root_dir}/../../${produce_name}
export PATH=/opt/hcc_arm64le/bin/:$PATH
#rtos和cpu需要环境已经安装

cd ${src_dir}
cmake -DFULL_BUILD_ROOT_DIR=${src_dir} -DCMAKE_INSTALL_PREFIX=${src_dir} ${src_dir}/SiteData/cmake/superbuild
make -j$(nproc) sitedata_ibmc
echo "==============build end =================================="
