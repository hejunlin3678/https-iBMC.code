#!/bin/bash
# This script build rtos component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="RTOS"

src_dir=${root_dir}/../../${produce_name}

export PATH=/opt/hcc_arm64le/bin/:$PATH

userid=$(id -u)
if [ $userid -eq 0 ]; then
    useradd -m huawei || :
    chown -R huawei:users ${src_dir}/..
    su - huawei <<EOF
    cd ${src_dir}/build
    grep w6T.xzdio ../ -rl | xargs -i{} sed -i 's/w6T.xzdio/w0.gzdio/g' {}
    export HB_FULLBUILD_PRODUCT="ibmc"
    python prepare/prepare_code.py "arm64le-preempt-pro-5.10" "" "application/build/works/platforms/rtos/fullbuild_rtos_tailor"
    python3 build.py arm64le-preempt-pro-5.10
EOF
else
    cd ${src_dir}/build
    grep w6T.xzdio ../ -rl | xargs -i{} sed -i 's/w6T.xzdio/w0.gzdio/g' {}
    export HB_FULLBUILD_PRODUCT="ibmc"
    python prepare/prepare_code.py "arm64le-preempt-pro-5.10" "" "application/build/works/platforms/rtos/fullbuild_rtos_tailor"
    python3 build.py arm64le-preempt-pro-5.10
fi

echo "==============build end =================================="
