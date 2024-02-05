#!/bin/bash
# This script build sdk component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============prebuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="SDK"

username=pARM64
password='encryption:ETMsDgAAAX1hVwscABRBRVMvQ0JDL1BLQ1M1UGFkZGluZwCAABAAEJUlOpZdcXyO1tkwHLVflCAAAAAgUgprt37BNj22kintlWyVMu7iCUF+ZmUa0wtn++6XgRUAFGxukyEVE/CmqXArzgLlBLZfum6k'
sudo sed -i "/<userName>/,/<\/userName>/s#<userName>.*</userName>#<userName>${username}</userName>#g" /root/.ArtGet/conf/Setting.xml
sudo sed -i "/<password>/,/<\/password>/s#<password>.*</password>#<password>${password}</password>#g" /root/.ArtGet/conf/Setting.xml

cd ${root_dir}
if [ "$1" = "-o" ]; then
    python3 works/platforms/download_from_cmc_SrcBaseline.py -p ${produce_name} -x ${cur_dir}/srcbaseline.xml -o
fi
# dep_file="${root_dir}/../../${produce_name}/.cloudbuild/config/dependency_3rd.xml"
agentpath="${root_dir}/../../${produce_name}/Hi1711_chip_solution"
# artget pull -d "${dep_file}" -ap "${agentpath}/vendor/cpld"

# dep_file="${root_dir}/../../${produce_name}/Hi1711_chip_solution/ci/down/cmc_platform/Hi1711_platform_dependency_5.10.xml"
# agentpath="${root_dir}/../../${produce_name}/Hi1711_chip_solution"
# artget pull -d "${dep_file}" -ap "${agentpath}/platform/rtos"

# 拷贝SDK提供的执行脚本（SDK提供新版本后，该步骤删除）
cp ${cur_dir}/all_src_build_part1.sh ${agentpath}/.cloudbuild

echo "==============prebuild end =================================="