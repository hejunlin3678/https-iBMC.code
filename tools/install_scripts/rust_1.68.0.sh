#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install rust-1.68.0
# version: 1.68.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1

# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

ls rust-1.68.0-x86_64-unknown-linux-gnu.tar.gz rust-std-1.68.0-armv7-unknown-linux-gnueabi.tar.gz rust-1.68.0-aarch64-unknown-linux-gnu.tar.gz

tar xzf rust-1.68.0-x86_64-unknown-linux-gnu.tar.gz        -C ${install_dir}
tar -xf rust-std-1.68.0-armv7-unknown-linux-gnueabi.tar.gz -C ${install_dir}
tar -xf rust-1.68.0-aarch64-unknown-linux-gnu.tar.gz  -C ${install_dir}

CACHE_DIR=/usr1/jenkins/workspace/compile/ibmc_cache
mkdir -p ${CACHE_DIR}/.cargo
export CARGO_HOME=${CACHE_DIR}/.cargo


cd ${install_dir}/rust-1.68.0-x86_64-unknown-linux-gnu/
./install.sh >/dev/null 2>&1

cd ${install_dir}/rust-std-1.68.0-armv7-unknown-linux-gnueabi/
./install.sh >/dev/null 2>&1

cd ${install_dir}/rust-1.68.0-aarch64-unknown-linux-gnu/
./install.sh --components=rust-std-aarch64-unknown-linux-gnu >/dev/null 2>&1

cat >${CACHE_DIR}/.cargo/config <<EOF
[source.huawei]
registry ="https://szv-open.codehub.huawei.com/cargo/cargo-center.git"
[source.crates-io]
registry ="https://github.com/rust-lang/crates.io-index"
replace-with = 'huawei'
[target.armv7-unknown-linux-gnueabi]
linker ="arm32-linux-gnueabi-gcc"
[target.aarch64-unknown-linux-gnu]
linker ="aarch64-target-linux-gnu-gcc"
[http]
multiplexing = true
timeout = 300
[net]
retry = 2
git-fetch-with-cli = true
EOF

cp -arf ${CACHE_DIR}/.cargo  /root/

echo "export CARGO_HOME=${CACHE_DIR}/.cargo" >>/etc/profile