#!/bin/bash
# This script build pme component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="PME"

src_dir=${root_dir}/../../${produce_name}

sed -i 's#<userName>.*</userName>#<userName>lwx507251</userName>#g'  ${src_dir}/pme_code/CI/config/conf/cmc_settings.xml
sed -i 's#<password>.*</password>#<password>encryption:ETMsDgAAAX+Vuku/ABRBRVMvQ0JDL1BLQ1M1UGFkZGluZwCAABAAEJm1hNu8ElhTKaUU99F595kAAAAgQLfvGpjARhFansNL8cf0rLE7DFZclM0zRFkl2wyFJNoAFNysMs1d/XKXR/Pp7VDo/gmCeDv6</password>#g'  ${src_dir}/pme_code/CI/config/conf/cmc_settings.xml
sed -i 's#secBepkit-2.0.0#secBepkit-2.1.0#g' ${src_dir}/pme_code/framework/build/build.sh
sed -i 's/declare -A KERNEL_RTOS_MAP=.*$/declare -A KERNEL_RTOS_MAP=(["4.4"]=207.8.0 ["5.10"]=208.5.0)/g' ${src_dir}/pme_code/framework/build/build.sh
sed -i 's/artget.*$//g' ${src_dir}/pme_code/framework/build/build.sh
cp -rf ${src_dir}/../temp/build_tools/rtos-sdk/* ${src_dir}/pme_code/framework/platform/rtos_arm32/
cp -rf ${src_dir}/../temp/build_tools/rtos-sdk-arm64/* ${src_dir}/pme_code/framework/platform/rtos/

sed -i 's#exit 1#echo skip exit 1#g' ${src_dir}/pme_code/framework/build/build_exports
sed -i 's#chmod -R 777 /opt#chmod -R 777 /opt/RTOS /opt/hcc_arm*#g' ${src_dir}/pme_code/framework/build/build_exports

cd ${src_dir}/pme_code/framework/build
chmod +x createfile.sh 
sh -x createfile.sh
if [ "$1" == "V5" ]
then
    sh -x build.sh arm $2 $3
else
    sh -x build.sh arm64 $2 $3
fi

echo "==============build end =================================="
