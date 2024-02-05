#!/bin/bash
# This scripts bulid huawei PME BMC.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

echo "#################################update farmework end###################################"
#compile application; 
echo "###############################compile application begin################################"
bash -x ./set_env_for_compile.sh
bash -x ./build_java.sh
bash -x ./build.sh arm
echo "###############################compile application end##################################"
