#!/bin/bash
# This scripts bulid huawei PME BMC.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

echo "#################################update farmework end###################################"
#compile application; 
echo "###############################compile application begin################################"
bash -x ./set_env_for_compile.sh
bash -x ./build_java.sh

export ENABLE_GCOV=1
touch ./enable_gcov # 创建一个文件gcov标记
bash -x ./build.sh gcov
echo "###############################compile application end##################################"
