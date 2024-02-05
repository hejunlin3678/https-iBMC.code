#!/usr/bin/env python
# coding: utf-8

"""
功 能：一键式迭代提交CMC二进制库
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import shutil

from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE
import subprocess
import re

logger = Logger().logger
def get_path(path):
    return os.path.realpath(os.path.join(current_script_path, path))


def set_path():
    global g_common
    g_common = Common()
    global current_script_path
    current_script_path = g_common.code_path

def upload_bin():
    logger.info('update middle binary files begin...')

    agentpath = os.path.join(g_common.temp_path, "bin_cmc")
    dependency_file = get_path("dependency_for_hpm/downxml/dependency_bin.xml")

    logname = os.path.join(g_common.temp_path, "upload.log")
    shcmd = f"artget push -d {dependency_file} -ap {agentpath} >{logname} 2>&1 "
    subprocess.run(shcmd, shell=True)

    list = g_common.file_find_text("version=", logname)
    for line in list:
        if re.search("3.2.0.x", line) != None:
            continue
        else:
            logger.info(line.split("version=")[1].split(",")[0])
    os.chdir(current_script_path)
    logger.info('update middle binary files end!')


if __name__ == "__main__":
    set_path()
    upload_bin()
