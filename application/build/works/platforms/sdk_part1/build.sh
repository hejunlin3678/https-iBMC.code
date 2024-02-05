#!/bin/bash
# This script build sdk component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="SDK"

src_dir=${root_dir}/../../${produce_name}

mkdir -p ${src_dir}/output
chmod +x -R ${src_dir}/*
cd ${src_dir}/Hi1711_chip_solution/.cloudbuild

export USER=$(whoami)
ln -sf /opt/buildtools/adoptopenjdk/openjdk8-binaries-jdk8u312-b07/bin/java /usr/bin
rm -rf /opt/buildtools/signatureclient-5.1.0-kunpeng
ln -sf /usr/local/signature-jenkins-slave /opt/buildtools/signatureclient-5.1.0-kunpeng

source ${src_dir}/Hi1711_chip_solution/build/build_prepare.sh 5.10
sh -x ./all_src_build_part1.sh daily all openbep

echo "==============build end =================================="