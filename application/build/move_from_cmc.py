#!/usr/bin/env python
# coding:utf-8

"""
文件名：move_from_cmc.py
功能：将download_from_cmc.py脚本中下载的文件，位于V2R2_trunk同级目录下的temp，移动到iBMC目录下的不同目
    录，配合其他脚本完成构建
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import sys
import argparse
import shutil
import tarfile
import zipfile
import re
import subprocess

from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

logger = Logger().logger

# 获取命令行参数(调用脚本时的位置参数)参数
parser = argparse.ArgumentParser(description = "Excute move_from_cmc.py")
parser.add_argument("-b", "--board_name", help = "Fill in board_name", default = "")
parser.add_argument("-f", "--from_source", const="from_source",
    action="store_const", help = "Fill in from_source", default = "")
parser.add_argument("-l", "--local", const="-l", action="store_const", help="File in from source", default="")
args = parser.parse_args()

def set_global():
    """
    配置全局路径
    """
    global g_common
    global g_compatibility_enabled
    global g_board_name
    global g_from_source
    global g_local
    global g_work_dir
    global g_source_dir
    global g_bin_cmc_dir
    global g_internal_release_dir
    global g_temp_dir
    g_common = Common()
    g_compatibility_enabled = 0
    g_board_name = args.board_name
    g_from_source = args.from_source
    g_local = args.local
    g_work_dir = g_common.code_path
    g_source_dir = g_common.code_root
    g_bin_cmc_dir = g_common.bin_cmc_path
    g_internal_release_dir = g_common.internal_release_path
    g_temp_dir = g_common.temp_path
    return

def remove_and_copy_dir(src_dir, dst_dir):
    if os.path.isdir(dst_dir):
        shutil.rmtree(dst_dir)
    shutil.copytree(src_dir, dst_dir)
    return

def move_middle_binary():
    """
    拷贝二进制文件
    """
    if os.path.isdir(g_temp_dir):
        logger.info(f"move binary files from {g_temp_dir}")
        g_common.copy_all(f"{g_temp_dir}/bin_cmc/internal_release", f"{g_source_dir}/internal_release")
        g_common.copy_all(f"{g_temp_dir}/bin_cmc", f"{g_source_dir}/bin_cmc")
    else:
        logger.info("no binary file folders")
    return

def move_src_bin():

    # lsw
    os.chdir(f"{g_source_dir}/application/src_bin")

    # user_interface
    remove_and_copy_dir(f"{g_bin_cmc_dir}/application/src_bin/user_interface", "./user_interface")

    logger.info("move src_bin from cmc done")
    return

def move_tools():
    logger.info("###############move tools from cmc begin###############")
    g_common.copy_all(f"{g_internal_release_dir}/release", f"{g_source_dir}/tools/release")
    g_common.copy_all(f"{g_bin_cmc_dir}/tools/release", f"{g_source_dir}/tools/release")
    logger.info("move tools/release from cmc done")
    logger.info("##############move tools from cmc done##################")
    return

if __name__ == "__main__":
    set_global()
    move_middle_binary()
    move_src_bin()
    move_tools()
    os.chdir(g_work_dir)
    logger.info("Move from cmc finish")
