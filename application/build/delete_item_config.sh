#!/bin/bash
# Cut out redundant files irrelevant to boards.
# Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
# Description: 裁剪单板无关的冗余文件的配置文件，仅声明变量
# Author: z00518714 
# Create: 2020-04-07 
# Notes: 
# History: 2020-04-07 z00518714 适配单板`TaiShan2280v2`
################################################################################

G_SUPPORTED_LIST=(
    "TaiShan2280v2"
)
# 从源码构建时，需要删除编译开源软件和第三方软件等过程中，
# 为了一致性而创建的目录，具体参见application/build_open_source.sh
DELETE_OPENSOURCE_FILE_LIST=(
    "/home/3rd/"
    "/home/public/workspace/application/"
    "/home/public/workspace/application/open_source/build/"
)
# 所有单板都要删除的文件及目录
delete_allboard_file=(
    #! Note: 
    # "tools/build_tools" 该目录不可以删除，否则个人构建失败
    ".git"
    "internal_release/dev_tools"
    "tools/dev_tools"
    "tools/release" 
    "vendor/450940-API_CS4227_CS4223_CS4343_PHY_API_Release_3.7.8"
)
# 各个单板需要删除的文件及目录
# 虽然脚本中仍会显示被删除的每个文件，
# 但还是建议在此显式地声明每个文件、谨慎使用目录名或通配符，以便调试扩展时更准确地控制该行为
delete_taishan2280v2_file=(
    #! Note：目录名与app.list中部分内容有差异, 后续改进要注意
    "application/src/apps/data_sync"
    "application/src/apps/data_sync2"
    "application/src/apps/lcd"
    "application/src/apps/rack_mgnt"
    "application/src/apps/rimm"
    "application/src/apps/smm_lsw"
    "application/src/apps/smm_mgnt"
    "application/src/apps/smm_sol"
    "application/src/apps/switch_card"
    # 字典序排列
    "application/src/apps/upgrade/inc/*lcd*"
    "application/src/apps/upgrade/src/upgrade_lcd"
    "application/src/libs/cs4343"
    "application/src/user_interface/kvm_client/KVM/src/com/kvm/resource/helpdoc/**/*.js" # 删除*.js; 与application/src/user_interface/kvm_client/KVM/build_client.xml的内容强相关
    "application/src/web/htdocs/emm"
    "application/src/web/webapp_irm" # iRM使用
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_retimer/2288hv5"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_retimer/atlas"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_retimer/TaiShan2280v2/"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch_9008advance"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch_9032"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch/atlas"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch/PEX8725_ch220_*.bin"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch/PEX8796_ch220_*.bin"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch/PEX8796_ch225_*.bin"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch/PEX9765_BP_7B_1.bin"
    "bin_cmc/application/src_bin/rootfs/opt/pme/conf/pcie_switch/PEX9765_BP_7B_2.bin"
    "bin_cmc/application/src_bin/third_party_groupware/arm64/StoreLib/libstorelib.so.07.0604.0100.1000"
    "bin_cmc/application/src_bin/third_party_groupware/arm64/StoreLib/libstorelib.so.07.1602.0100.0000"
    "bin_cmc/application/src_bin/third_party_groupware/arm64/StoreLibIR3/libstorelibir3.so.16.02-0"
    "bin_cmc/application/src_bin/third_party_groupware/arm64/StoreLibIT/libstorelibit.so.07.0700.0200.0600"
    "bin_cmc/tools/toolchain/gppbin/gpp_header"
    "bin_cmc/tools/toolchain/package_emmc_nand_img_v6/converfile"
    "internal_release/dft_tools/GetBmcHealthevents/*"
    "internal_release/dft_tools/hpmfwupg/x86/*"
    "internal_release/dft_tools/src/driver_agentless_x86"
    "internal_release/dft_tools/src/soltool/soltool.c"
    "internal_release/dft_tools/src/soltool/soltool.h"
    "internal_release/release/kvm_client/skeletons/autodownload-wrapper/autodownload.skel"
    "internal_release/release/kvm_client/skeletons/autodownload-wrapper/customdownload.skel"
    "internal_release/release/kvm_client/skeletons/console-wrapper/description.skel"
    "internal_release/release/kvm_client/skeletons/windowed-wrapper/description.skel"
    "internal_release/release/kvm_client/skeletons/winservice-wrapper/description.skel"
    "platforms/lan_switch/HiLink30LRV101_Adapt_SRAM_2.0.1_0904_1923.bin"
    "platforms/platform_v6/Hi1711_boot_4096_debug.bin"
    "platforms/platform_v6/Hi1711_boot_4096.bin"
    "platforms/platform_v6/hi1711sdk.tar.gz"
    "platforms/platform_v6/PME_SDK.tar.gz"
    "platforms/platform_v6/pme_strip.tar.gz"
    "platforms/platform_v6/pme.tar.gz"
    "platforms/platform_v6/rtos.tar.gz"
    "tools/dev_tools/hpmpack_release/common/beforeaction.sh"  
    "tools/toolchain/CMakeLists.txt"
    "tools/toolchain/hpmimage/hpm.config"
    "tools/toolchain/hpmimage/ReadMe.txt"
    "tools/toolchain/package_emmc_nand_img_v6"
    "tools/toolchain/package_emmc_nand_img"
    "tools/toolchain/package"
    "vendor/CS4343_PHY_API"
    "vendor/StoreLibIT/00016631-eUSDK-release-v1.1.4-svn98922.rar"
    "vendor/HUYANG010_HY010-4304-0001"
    "vendor/MSWITCH-8380M-SOFTWARE"
    "vendor/Realtek_RTL8367RB_Firmware"
)
