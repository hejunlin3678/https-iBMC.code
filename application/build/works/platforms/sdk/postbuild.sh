#!/bin/bash
# This script build sdk component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
set -e
echo "==============postbuild begin =================================="
cur_dir=$(readlink -f "$(dirname "$0")")
root_dir=${cur_dir}/../../..
produce_name="SDK"

src_dir=${root_dir}/../../${produce_name}
output_path=${root_dir}/../../share_path/platforms/platform_v6
mkdir -p ${output_path}

ls -R ${src_dir}/Hi1711_chip_solution/build/output/packet/update_bin/
cp -rf ${src_dir}/Hi1711_chip_solution/build/output/packet/update_bin/pme/* ${output_path}
mv ${output_path}/pkcs_Hi1711_boot_4096.bin  ${output_path}/Hi1711_boot_4096.bin
mv ${output_path}/pkcs_Hi1711_boot_4096_debug.bin ${output_path}/Hi1711_boot_4096_debug.bin
mv ${output_path}/pkcs_Hi1711_boot_4096_factory.bin ${output_path}/Hi1711_boot_4096_factory.bin
mv ${output_path}/pkcs_Hi1711_boot_4096_pmode.bin ${output_path}/Hi1711_boot_4096_pmode.bin
mv ${output_path}/pkcs_Hi1711_boot_4096_pmode_debug.bin ${output_path}/Hi1711_boot_4096_pmode_debug.bin
mv ${output_path}/pkcs_Hi1711_boot_4096_pmode_factory.bin ${output_path}/Hi1711_boot_4096_pmode_factory.bin

echo "==============postbuild end =================================="
