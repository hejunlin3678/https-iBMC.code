#!/usr/bin/env python
# coding:utf-8

"""
文件名：build.py
功能：编译自研工具
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""


import os
import shutil
from application.build.utils.log import Logger
from application.build.utils.error_info import EXCEPTION_STAGE
from application.build.utils.error_info import EXCEPTION_RESPONSE
from application.build.utils.error_info import EXCEPTION_CODE
import subprocess
from application.build.utils.common import Common
logger = Logger().logger


def set_global():
    global g_common
    global g_huawei_developdtools_dir
    global g_bin_dst_dir

    g_common = Common()
    g_huawei_developdtools_dir = f"{g_common.temp_path}/huawei_developedtools"
    g_bin_dst_dir = f"{g_common.code_root}/tools"
    os.chdir(f"{g_common.code_root}/tools/toolchain")
    return


def check_dir(dir_name):
    """
    确认目录是否存在，存在则删除重建，不存在则创建
    """
    if os.path.isdir(dir_name):
        shutil.rmtree(dir_name)
        os.makedirs(dir_name)
    else:
        os.makedirs(dir_name)
    return


def move_to_temp_build():
    g_common.copy_all(f"{g_bin_dst_dir}/toolchain", f"{g_huawei_developdtools_dir}/toolchain")
    g_common.copy_all(f"{g_bin_dst_dir}/dev_tools", f"{g_huawei_developdtools_dir}/dev_tools")
    return


def compile_cryto_tool_ext4():
    """
    编译 crypto_tool_ext4 并移动到目标位置
    """
    os.chdir(f"{g_huawei_developdtools_dir}/toolchain/crypto_tool")
    check_dir("build")
    os.chdir("build")
    subprocess.run("cmake ../", shell=True)
    ret = g_common.run_command("make")
    if ret.returncode:
        err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}"
        logger.error(f"{err_msg} make crypto_tool_ext4 failed")
        os._exit(1)
    logger.info("got crypto tool:")
    subprocess.run("ls -a | grep crypto_tool_ext4", shell=True)

    shutil.copy("crypto_tool_ext4", f"{g_bin_dst_dir}/toolchain/hpmpack/ipmcimage/")
    shutil.copy("crypto_tool_ext4", f"{g_bin_dst_dir}/toolchain/hpmpack/ipmcimage/x86")
    shutil.copy("crypto_tool_ext4", f"{g_bin_dst_dir}/dev_tools/hpmpack_release/common")
    shutil.copy("crypto_tool_ext4", f"{g_bin_dst_dir}/dev_tools/hpmpack_release/common/x86")
    return


def compile_hpmimage():
    """
    编译 hpmimage 并移动到目标位置
    """
    os.chdir(f"{g_huawei_developdtools_dir}/toolchain/hpmimage")
    ret = g_common.run_command("make")
    if ret.returncode:
        err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}"
        logger.error(f"{err_msg} make hpmimage failed")
        os._exit(1)
    shutil.copy("hpmimage", f"{g_bin_dst_dir}/toolchain/hpmpack/ipmcimage/hpmimage")
    shutil.copy("hpmimage", f"{g_bin_dst_dir}/toolchain/hpmpack/ipmcimage/hpmimage_ext4")
    shutil.copy("hpmimage", f"{g_bin_dst_dir}/dev_tools/hpmpack_release/common/hpmimage")
    shutil.copy("hpmimage", f"{g_bin_dst_dir}/dev_tools/hpmpack_release/common/hpmimage_ext4")
    return


def compile_filesizecheck():
    """
    编译 filesizecheck 并移动到目标位置
    """
    os.chdir(f"{g_huawei_developdtools_dir}/toolchain/filesizecheck")
    ret = g_common.run_command("make")
    if ret.returncode:
        err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}"
        logger.error(f"{err_msg} make filesizecheck failed")
        os._exit(1)
    shutil.copy("filesizecheck", f"{g_bin_dst_dir}/toolchain/hpmpack/ipmcimage/")
    shutil.copy("filesizecheck", f"{g_bin_dst_dir}/dev_tools/hpmpack_release/common/")
    return


def compile_header():
    """
    编译 gpp_header 并移动到目标位置
    """
    os.chdir(f"{g_huawei_developdtools_dir}/toolchain/emmc_hpm_header")
    ret = g_common.run_command("make")
    if ret.returncode:
        err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}"
        logger.error(f"{err_msg} make emmc_hpm_header failed")
        os._exit(1)
    shutil.copy("gpp_header", f"{g_bin_dst_dir}/toolchain/emmc_hpm_header/")
    return


if __name__ == "__main__":
    set_global()
    move_to_temp_build()
    compile_cryto_tool_ext4()
    compile_hpmimage()
    compile_filesizecheck()
    compile_header()
