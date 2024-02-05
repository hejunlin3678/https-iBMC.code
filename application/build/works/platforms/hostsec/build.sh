#!/bin/bash
# This script build hostsec component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============build begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="HostSec"

src_dir=${root_dir}/../../${produce_name}

cd ${src_dir}/hisec_ict_ne/necommon
sed -i "s/ibmc_dlsetenv_file=\/opt\/RTOS\/[0-9]\{3,\}.[0-9]\{1,\}.[0-9]\{1,\}\/dlsetenv.sh/ibmc_dlsetenv_file=\/opt\/RTOS\/208.5.0\/dlsetenv.sh/g" build.conf
python3 build.py -p ibmc -c arm64 -t false

cd ${src_dir}/hisec_ict_ne/neframe
sed -i "s/ibmc_dlsetenv_file=\/opt\/RTOS\/[0-9]\{3,\}.[0-9]\{1,\}.[0-9]\{1,\}\/dlsetenv.sh/ibmc_dlsetenv_file=\/opt\/RTOS\/208.5.0\/dlsetenv.sh/g" build.conf
python3 build.py -p ibmc -c arm64 -t false

cd ${src_dir}/hisec_ict_ne/hostsec
sed -i "s/ibmc_dlsetenv_file=\/opt\/RTOS\/[0-9]\{3,\}.[0-9]\{1,\}.[0-9]\{1,\}\/dlsetenv.sh/ibmc_dlsetenv_file=\/opt\/RTOS\/208.5.0\/dlsetenv.sh/g" build.conf
python3 build.py -p ibmc -c arm64 -t false
echo "==============build end =================================="
