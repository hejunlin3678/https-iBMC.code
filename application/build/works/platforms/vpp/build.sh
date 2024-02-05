#!/bin/bash
# This script build adaptive_lm component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="VPP"

src_dir=${root_dir}/../../${produce_name}

cp -rf ./works/platforms/vpp/cmake ${src_dir}
cp -rf ./works/platforms/vpp/script ${src_dir}
cp ./works/platforms/vpp/config.xml ${src_dir}
cp ./works/platforms/vpp/toolchainrelation.xml ${src_dir}

cp -rf ./works/platforms/vpp/cmake ${src_dir}/platform/HiSec-ICTTLS/
cp -rf ./works/platforms/vpp/script ${src_dir}/platform/HiSec-ICTTLS/
cp ./works/platforms/vpp/config.xml ${src_dir}/platform/HiSec-ICTTLS/
cp ./works/platforms/vpp/toolchainrelation.xml ${src_dir}/platform/HiSec-ICTTLS/

cd ${src_dir}
python3 build.py --section=IBMC_SUSE_ARM -p IBMC -e SUSE12_SP5_ARM32LE_IBMC

echo "============== openssl bin build begin =================================="

cp -rf ./open_source/openssl openssl_bin
cd openssl_bin
export TARGET=linux-armv4
export NO_OSSL_RENAME_VERSION=1
CROSS_COMPILE_INSTALL_PATH=/opt/hcc_arm32le
G_VERSION="208.5.0"
G_CPU="arm32A9le"
G_KERNEL="5.10_ek_preempt_pro"

source /opt/RTOS/${G_VERSION}/dlsetenv.sh -p ${G_CPU}_${G_KERNEL} --sdk-path=${CROSS_COMPILE_INSTALL_PATH}
./Configure --prefix=/usr --openssldir=/usr/lib/ssl-3 --libdir=/usr/lib $TARGET no-afalgeng -fPIC -fstack-protector-strong -shared -Wl,-z,relro,-z,noexecstack,-z,now -rdynamic
make -j32

echo "==============build end =================================="
