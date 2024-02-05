#!/bin/bash
# Used during MR building and invokes the Python script, read invoked python file to obtain details.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

cd ..
if [ -z $1 ]; then
    echo "################ set_env_for_compile start ################"
    python3 set_env_for_compile_arm32_64.py -b TaiShan2480v2
    echo "################ set_env_for_compile end ################"
elif [ -z $2 ]; then
    echo "################ set_env_for_compile start ################"
    python3 set_env_for_compile_arm32_64.py -b $1
    echo "################ set_env_for_compile end ################"
elif [ -z $3 ];then
    if [ $2 == "from_source" ];then
    echo "################ set_env_for_compile start ################"
    python3 set_env_for_compile_arm32_64.py -b $1 -f $2
    echo "################ set_env_for_compile end ################"
    elif [ $2 == "local" ];then
        python3 set_env_for_compile_arm32_64.py -b $1 -l
    fi
else
    echo "################ set_env_for_compile start ################"
    python3 set_env_for_compile_arm32_64.py -b $1 -f $2 -l
    echo "################ set_env_for_compile end ################"
fi
