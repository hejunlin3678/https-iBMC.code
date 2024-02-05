#!/bin/bash
# This script build third part component (Storelib) for hi1710 arch cpu.
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

cd ./versionChangeSet/common_storelib_unf_rel


# Change line endings
find . -type f -print0 | xargs -0 dos2unix

# complie and install software.
cd app_util/common_storelib_unf/build/oob/
sed -i "s/-mstructure-size-boundary/-fpack-struct/" $(grep -rl "\-mstructure-size-boundary")
make clean
make mode=i2c cfg=release arch=arm64 
if [ $? != 0 ]; then
    exit 1
fi

cd ${sto_dir}
rm -rf output > /dev/null 2>&1
mkdir output
cp -a ./versionChangeSet/common_storelib_unf_rel/app_util/common_storelib_unf/build/oob/libstorelib.so* ./output/

${STRIP} ./output/*

exit 0

