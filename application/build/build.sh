#!/bin/bash
# Used during MR building and invokes the Python script, read invoked python file to obtain details.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

set -e
# 参数有序，如果某个参数为空则不做处理
G_WORK_DIR=$(pwd)

function clean()
{
    CLEAN_PATH=${G_WORK_DIR}/../../temp/target
    [ -n "${CLEAN_PATH}" ] && rm -rf ${CLEAN_PATH}
}
if [ -z $1 ]
then
    python3 build.py
elif [ -z $2 ]
then
    if [ $1 == "clean" ]
    then
        clean
    else
        python3 build.py -a $1
    fi
elif [ -z $3 ]
then
    python3 build.py -a $1 -s $2
else
    python3 build.py -a $1 -s $2 -b $3
fi