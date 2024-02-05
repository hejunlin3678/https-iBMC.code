#!/bin/bash
# This script build third part component (StorelibIR3) for hi1710 arch cpu.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

# May you do good and not evil.
# May you find forgiveness for yourself and forgive others.
# May you share freely, never taking more than you give.

# author: hucheng 188524
# date: Wed Dec 18 14:14:09 CST 2013
# version: 0.8.0-1

# make global set ready.
source ./global64.sh
#echo "$G_BOOST_DIR"
#echo "$G_PREFIX"

sto_dir=$(pwd)

cd ./versionChangeSet/storelibir3_bmc_rel/storelib_src_rel

# Change line endings
find . -type f -print0 | xargs -0 dos2unix

# complie and install software.
cd ./apps/storelibir3_bmc/src/project/linux/I2C/
sed -i "s/-mstructure-size-boundary/-fpack-struct/" $(grep -rl "\-mstructure-size-boundary")
make arch=arm64 cfg=release builder=i2c -j$(nproc)
if [ $? != 0 ]; then
    exit 1
fi

cd ${sto_dir}
rm -rf output > /dev/null 2>&1
mkdir output
cp -a ./versionChangeSet/storelibir3_bmc_rel/storelib_src_rel/apps/storelibir3_bmc/src/project/linux/I2C/libstorelibir3.so* ./output/

${STRIP} ./output/*

exit 0

