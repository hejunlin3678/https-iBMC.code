#!/bin/bash
# Copy source file and configure-scripts into current directory
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

NAME=$1
GLOBAL_SH=$2
if [ -z ${NAME} ] || [ -z ${GLOBAL_SH} ]; then
    echo "input error"
    exit 1
else
    cp ../../open_source/${GLOBAL_SH} .
fi