#!/bin/bash
# This script build adaptive_lm component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="VPP"

src_dir=${root_dir}/../../${produce_name}
output_path=${root_dir}/../../share_path/vpp/usr/lib
include_path=${root_dir}/../../share_path/vpp/include
bin_path=${root_dir}/../../share_path/vpp/usr/bin
output_path_l=${root_dir}/../../temp/vpp

mkdir -p ${output_path}
mkdir -p ${include_path}/CMPV2/INCLUDE
cp -rf ${src_dir}/output/CMPV2/INCLUDE/* ${include_path}/CMPV2/INCLUDE
cp -rf ${src_dir}/output/CMPV2/IBMC/SUSE12_SP5_ARM32LE_IBMC/RELEASE/*.so ${output_path}

mkdir -p ${include_path}/CRYPTO/INCLUDE
cp -rf ${src_dir}/output/CRYPTO/INCLUDE/* ${include_path}/CRYPTO/INCLUDE
cp -rf ${src_dir}/output/CRYPTO/IBMC/SUSE12_SP5_ARM32LE_IBMC/RELEASE/*.so ${output_path}

mkdir -p ${include_path}/OpenSSL/INCLUDE
cp -rf ${src_dir}/output/OpenSSL/IBMC/INCLUDE/* ${include_path}/OpenSSL/INCLUDE
cp -rf ${src_dir}/output/OpenSSL/IBMC/SUSE12_SP5_ARM32LE_IBMC/libs/*.so ${output_path}

mkdir -p ${include_path}/OSAL/INCLUDE
cp -rf ${src_dir}/output/OSAL/INCLUDE/* ${include_path}/OSAL/INCLUDE
cp -rf ${src_dir}/output/OSAL/IBMC/SUSE12_SP5_ARM32LE_IBMC/RELEASE/*.so ${output_path}

mkdir -p ${include_path}/PSE/INCLUDE
cp -rf ${src_dir}/output/PSE/INCLUDE/* ${include_path}/PSE/INCLUDE
cp -rf ${src_dir}/output/PSE/IBMC/SUSE12_SP5_ARM32LE_IBMC/RELEASE/*.so ${output_path}

mkdir -p ${include_path}/XCAP/INCLUDE
cp -rf ${src_dir}/output/XCAP/INCLUDE/* ${include_path}/XCAP/INCLUDE
cp -rf ${src_dir}/output/XCAP/IBMC/SUSE12_SP5_ARM32LE_IBMC/RELEASE/*.so ${output_path}

mkdir -p ${bin_path}
cp -rf ${src_dir}/openssl_bin/apps/openssl ${bin_path}/openssl_vpp

mkdir -p ${output_path_l}
cp -rf ${root_dir}/../../share_path/vpp/* ${output_path_l}
echo "==============postbuild end =================================="
