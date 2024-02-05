#!/usr/bin/env python
# coding: utf-8

"""
文件名：infotree_post_process.py
功能：对信息树上下载下来的文件进行配置
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import sys
import shutil
import subprocess
import argparse
import tarfile
import threading

from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

# 从当前路径导入 common 类，引用公共路径及方法
sys.path.append(r".")

# 获取命令行参数(调用脚本时的位置参数)参数
parser = argparse.ArgumentParser(description="Excute set_env_for_compile.py")
parser.add_argument("-l", "--local", const="-l", action="store_const", help="File in from source", default="")
args = parser.parse_args()

logger = Logger().logger
def set_global():
    """
    获取工作目录
    """
    global g_common
    global g_local
    global g_workspace
    g_common = Common()
    g_local = args.local
    g_workspace = g_common.code_path
    return

def check_dir(dir_name):
    """
    判断文件夹是否存在，如果存在则删除重建，不存在直接创建
    """
    logger.info("Excute function check_dir")
    if os.path.isdir(dir_name):
        shutil.rmtree(dir_name)
        os.makedirs(dir_name)
    else:
        os.makedirs(dir_name)
    return

def move_pme_v5():
    """
    PME
    """
    logger.info("Excute function move_pme_v5")
    # 设置源目录，目标目录与特定版本文件名
    # PME
    platform_v5_dir = f"{g_common.bin_cmc_path}/application/platforms/platform_v5"
    platform_v5_source = f"{g_common.temp_path}/platforms/platform_v5/Platform_V5"
    check_dir(platform_v5_dir)
    subprocess.run(f"cp {platform_v5_source}/hi1710sdk.tar.gz {platform_v5_dir}", shell=True)
    subprocess.run(f"cp {platform_v5_source}/Module.symvers {platform_v5_dir}", shell=True)
    subprocess.run(f"cp {platform_v5_source}/pme.tar.gz {platform_v5_dir}", shell=True)
    subprocess.run(f"cp {platform_v5_source}/PME_SDK.tar.gz {platform_v5_dir}", shell=True)
    subprocess.run(f"cp {platform_v5_source}/pme_strip.tar.gz {platform_v5_dir}", shell=True)
    subprocess.run(f"cp {platform_v5_source}/rtos.tar.gz {platform_v5_dir}", shell=True)
    subprocess.run(f"cp {platform_v5_source}/u-boot.bin {platform_v5_dir}", shell=True)
    subprocess.run(f"cp {platform_v5_source}/u-boot_debug.bin {platform_v5_dir}", shell=True)
    return

def move_pme_v6():
    """
    PME_V6
    """
    global g_local
    logger.info("Excute function move_pme_v6")
    platform_v6_dir = f"{g_common.bin_cmc_path}/application/platforms/platform_v6"
    if g_local == "":
        platform_v6_source = f"{g_common.temp_path}/platforms/platform_v6"
    elif g_local == "-l":
        platform_v6_source = f"{g_common.pme_full_src_path}/platforms/platform_v6"
    logger.info(f"platform_v6_source = {platform_v6_source}")
    check_dir(platform_v6_dir)
    subprocess.run(f"cp {platform_v6_source}/PME_SDK.tar.gz {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/hi1711sdk.tar.gz {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/Hi1711_boot_4096.bin {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/Hi1711_boot_4096_debug.bin {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/Hi1711_boot_4096_pmode.bin {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/Hi1711_boot_4096_pmode_debug.bin {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"dd if={platform_v6_source}/Hi1711_boot_4096_pmode.bin of={platform_v6_dir}/Hi1711_boot_pmode.bin bs=1k count=1024 skip=768", shell=True, check=True)
    subprocess.run(f"dd if={platform_v6_source}/Hi1711_boot_4096_pmode_debug.bin of={platform_v6_dir}/Hi1711_boot_pmode_debug.bin bs=1k count=1024 skip=768", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/Hi1711_boot_4096_factory.bin {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/Hi1711_boot_4096_pmode_factory.bin {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"dd if={platform_v6_source}/Hi1711_boot_4096_pmode_factory.bin of={platform_v6_dir}/Hi1711_boot_pmode_factory.bin bs=1k count=1024 skip=768", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/pme.tar.gz {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/pme_strip.tar.gz {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"cp {platform_v6_source}/rtos.tar.gz {platform_v6_dir}", shell=True, check=True)

    check_dir(f"{platform_v6_source}/hi1711sdk")
    g_common.untar_to_dir(f"{platform_v6_source}/hi1711sdk.tar.gz", f"{platform_v6_source}/hi1711sdk")
    subprocess.run(f"cp {platform_v6_source}/hi1711sdk/Module.symvers {platform_v6_dir}", shell=True, check=True)
    subprocess.run(f"sudo rm -rf {platform_v6_source}/hi1711sdk/*", shell=True)
    logger.info("Excute function move_pme_v6 finish")
    return

def move_adaptivelm():
    """
    AdaptiveLM
    """
    logger.info("Excute function move_adaptivelm")
    adaptivelv_dir = g_common.adaptivelv_path
    adaptivelv_source = f"{g_common.temp_path}/platforms/AdaptiveLM"
    check_dir(adaptivelv_dir)
    shutil.copyfile(f"{adaptivelv_source}/lic_iBMC_suse12sp5_arm32A9le_hi1710_le_release.so",
                    f"{adaptivelv_dir}/liblic.so")

def move_adaptivelm_arm64():
    """
    AdaptiveLM arm64
    """
    logger.info("Excute function move_adaptivelm_arm64")
    adaptivelv_dir_arm64 = g_common.adaptivelv_path_arm64
    if g_local == "":
        adaptivelv_source = f"{g_common.temp_path}/platforms/AdaptiveLM"
        adaptivelv_source_arm64 = f"{g_common.temp_path}/platforms/AdaptiveLM/arm64"
    elif g_local == "-l":
        adaptivelv_source = f"{g_common.pme_full_src_path}/platforms/AdaptiveLM"
        adaptivelv_source_arm64 = f"{g_common.pme_full_src_path}/platforms/AdaptiveLM/arm64"
    logger.info(f"adaptivelv_source = {adaptivelv_source}")
    logger.info(f"adaptivelv_source_arm64 = {adaptivelv_source_arm64}")
    check_dir(adaptivelv_dir_arm64)
    shutil.copyfile(f"{adaptivelv_source_arm64}/lic_iBMC_suse12sp5_arm64le_hi1711_le_release.so",
                    f"{adaptivelv_dir_arm64}/liblic.so")
    subprocess.run(f"yes y | cp -ai {adaptivelv_source}/include/* {g_workspace}/../src/include/pme_app/license_mgnt/", shell=True)
    logger.info("Excute function move_adaptivelm_arm64 finish")
    return

def move_fusionirm():
    """
    FusioniRM
    """
    logger.info("Excute function move_fusionirm")
    asset_locator_dir = g_common.asset_locator_path
    asset_locator_source = f"{g_common.temp_path}/platforms/asset_locator"
    check_dir(asset_locator_dir)
    shutil.copyfile(f"{asset_locator_source}/asset_locator_2.8.0.bin", f"{asset_locator_dir}/asset_locator.bin")
    return

def move_FusionPoD_RM210_hinic():
    """
    FusionPoD RM210板载1822网卡相关
    """
    logger.info("Excute function move_FusionPoD_RM210_hinic")
    fpod_hinic_dir = g_common.fpod_hinic_dir
    g_common.check_path(fpod_hinic_dir)
    fpod_hinic_source = f"{g_common.temp_path}/platforms/1822"
    subprocess.run(f"yes y | cp -ai {fpod_hinic_source}/firmware/IN500_solution_5.1.0.SPC690.B090.zip {fpod_hinic_dir}/firmware.zip", shell=True)
    return

def move_site_ai():
    """
    SiteAi
    """
    logger.info("Excute function move_site_ai")
    site_ai_lib_dir = f"{g_common.bin_cmc_path}/application/src_bin/platforms/site_ai"
    site_ai_lib_dir_v6 = f"{g_common.bin_cmc_path}/application/src_bin/platforms/site_ai_v6"
    site_ai_lib_dir_ut = f"{g_common.bin_cmc_path}/application/src_bin/platforms/site_ai_ut"
    site_ai_include_dir = f"{g_common.code_root}/platforms/site_ai/include"
    site_ai_source = f"{g_common.temp_path}/platforms/site_ai"

    """
    文件夹不存在应该直接返回
    """
    if not os.path.isdir(site_ai_source):
        return
    check_dir(site_ai_lib_dir)
    check_dir(site_ai_lib_dir_v6)

    check_dir(f"{g_common.bin_cmc_path}/tools/others/dev_tools/llt/pme/lib/")

    check_dir(f"{site_ai_include_dir}/cml/")
    site_ai_file_name="SiteAI V100R023C00B500.tar.gz"
    site_ai_dir_name="V100R023C00B500"

    check_dir(f"{site_ai_source}/{site_ai_dir_name}")
    g_common.untar_to_dir(f"{site_ai_source}/{site_ai_file_name}", f"{site_ai_source}/{site_ai_dir_name}")
    # v5 lib
    g_common.copy(f"{site_ai_source}/{site_ai_dir_name}/src/cml/rtos_arm32a55le/lib/libsiteai_cml.so", f"{site_ai_lib_dir}/libsiteai_cml.so")
    g_common.copy(f"{site_ai_source}/{site_ai_dir_name}/src/cml/rtos_arm32a55le/lib/libsiteai_util.so", f"{site_ai_lib_dir}/libsiteai_util.so")
    g_common.copy_all(f"{site_ai_source}/{site_ai_dir_name}/src/cml/rtos_arm32a55le/include/C/", f"{site_ai_include_dir}/cml/")
    # v6 lib
    g_common.copy(f"{site_ai_source}/{site_ai_dir_name}/src/cml/rtos_arm64a55le/lib/libsiteai_cml.so", f"{site_ai_lib_dir_v6}/libsiteai_cml.so")
    g_common.copy(f"{site_ai_source}/{site_ai_dir_name}/src/cml/rtos_arm64a55le/lib/libsiteai_util.so", f"{site_ai_lib_dir_v6}/libsiteai_util.so")
    g_common.copy(f"{site_ai_source}/{site_ai_dir_name}/tools/cml_converter/fbs/CML.fbs", f"{site_ai_lib_dir}/CML.fbs")
    g_common.copy_all(f"{site_ai_source}/{site_ai_dir_name}/src/cml/rtos_arm64a55le/include/C/", f"{site_ai_include_dir}/cml/")
    #ut lib
    g_common.copy(f"{site_ai_lib_dir_ut}/libsiteai_cml.so", f"{g_common.bin_cmc_path}/tools/others/dev_tools/llt/pme/lib/libsiteai_cml.so")
    g_common.copy(f"{site_ai_lib_dir_ut}/libsiteai_util.so", f"{g_common.bin_cmc_path}/tools/others/dev_tools/llt/pme/lib/libsiteai_util.so")
    logger.info("Finish function move_site_ai")
    return

def move_site_data():
    """
    SiteData
    """
    logger.info("Excute function move_site_data")
    site_data_lib_dir = f"{g_common.bin_cmc_path}/application/src_bin/platforms/site_data"
    site_data_lib_dir_v6 = f"{g_common.bin_cmc_path}/application/src_bin/platforms/site_data_v6"
    site_ai_lib_dir_ut = f"{g_common.bin_cmc_path}/application/src_bin/platforms/site_ai_ut"
    site_data_include_dir = f"{g_common.code_root}/platforms/site_data/include"
    site_data_source = f"{g_common.temp_path}/platforms/site_data"

    """
    文件夹不存在应该直接返回
    """
    if not os.path.isdir(site_data_source):
        return
    check_dir(site_data_lib_dir)
    check_dir(site_data_lib_dir_v6)
    check_dir(f"{g_common.bin_cmc_path}/tools/others/dev_tools/llt/pme/lib/")
    check_dir(f"{site_data_include_dir}/cdf/")
    site_data_file_name="SiteData V100R023C00B500.tar.gz"
    site_data_dir_name="V100R023C00B500"

    check_dir(f"{site_data_source}/{site_data_dir_name}")
    g_common.untar_to_dir(f"{site_data_source}/{site_data_file_name}", f"{site_data_source}/{site_data_dir_name}")
    # v5 lib
    g_common.copy(f"{site_data_source}/{site_data_dir_name}/src/cdf/rtos_arm32a55le/lib/libsitedata_cdf.so", f"{site_data_lib_dir}/libsitedata_cdf.so")
    g_common.copy_all(f"{site_data_source}/{site_data_dir_name}/src/cdf/rtos_arm32a55le/include/C/", f"{site_data_include_dir}/cdf/")
    # v6 lib
    g_common.copy(f"{site_data_source}/{site_data_dir_name}/src/cdf/rtos_arm64a55le/lib/libsitedata_cdf.so", f"{site_data_lib_dir_v6}/libsitedata_cdf.so")
    g_common.copy_all(f"{site_data_source}/{site_data_dir_name}/src/cdf/rtos_arm64a55le/include/C/", f"{site_data_include_dir}/cdf/")
    #ut lib
    g_common.copy(f"{site_ai_lib_dir_ut}/libsitedata_cdf.so", f"{g_common.bin_cmc_path}/tools/others/dev_tools/llt/pme/lib/libsitedata_cdf.so")
    logger.info("Finish function move_site_data")
    return

if __name__ == "__main__":
    set_global()
    move_pme_v5()
    move_pme_v6()
    try:
        move_adaptivelm()
    except Exception as e:
        logger.error(f"move_adaptivelm failed: {e}")

    try:
        move_adaptivelm_arm64()
    except Exception as e:
        logger.error(f"move_adaptivelm_arm64 failed: {e}")

    try:
        move_FusionPoD_RM210_hinic()
    except Exception as e:
        logger.error(f"move_FusionPoD_RM210_hinic failed: {e}")

    try:
        move_site_ai()
    except Exception as e:
        logger.error(f"move_site_ai failed: {e}")

    try:
        move_site_data()
    except Exception as e:
        logger.error(f"move_site_data failed: {e}")

