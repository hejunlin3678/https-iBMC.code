#!/bin/bash
# This scripts test huawei PME BMC.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

arg=$1

BUILD_PATH=$(pwd)

export LD_LIBRARY_PATH=/usr/local/pme/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUILD_PATH/../sdk/PME/V100R001C10/x86/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUILD_PATH/target/unit_test/opt/pme/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUILD_PATH/target/unit_test/usr/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUILD_PATH/../../tools/dev_tools/llt/pme/lib/

gdb ./target/src/apps/test_code/test_all core
