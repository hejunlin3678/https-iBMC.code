#!/bin/bash
# Used during MR building and invokes the Python script, read invoked python file to obtain details.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

# ?? set_env_for_compile_arm32_64.py ??
echo "################ set_env_for_compile start ################"
if [ -z $1 ]; then
    python3 set_env_for_compile_arm32_64.py --board_name "TaiShan2280v2"
else
    python3 set_env_for_compile_arm32_64.py --board_name $1
fi
echo "################ set_env_for_compile end ################"
