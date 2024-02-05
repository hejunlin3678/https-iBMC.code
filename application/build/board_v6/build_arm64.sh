#!/bin/bash
# Used during MR building and invokes the Python script, read invoked python file to obtain details.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

# 参数有序，如果某个参数为空则不做处理
if [ -z $1 ]
then
    python3 ../build.py
elif [ -z $2 ]
then
    python3 ../build.py -a $1
elif [ -z $3 ]
then
    python3 ../build.py -a $1 -s $2 -b TaiShan1280v2
else
    python3 ../build.py -a $1 -s $2 -b $3
fi
