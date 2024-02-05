#!/usr/bin/env python
# coding=utf-8

"""
文件名：work_cut_common_dependency_for_open.py
功能：该脚本用于执行开放代码裁剪以及依赖开源软件，第三方软件下载打包
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import time
import shutil
import sys
import re
import argparse
import subprocess

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkCutCommonDependencyForOpen(Work):
    def __init__(self, config, work_name=""):
        super(WorkCutCommonDependencyForOpen, self).__init__(config, work_name)
        self.manifest_file_path = f"{self.common.code_root}/prepare_for_open/manifest.yml"

    def run(self):
        self.logger.info(f"copy common dependency start")
        os.chdir(f"{self.common.open_code_root}")
        self.common.check_path(f"{self.common.open_code_root}/temp")
        
        # 拷贝裁剪清单里 common_dependency 的文件
        common_dependency_files = self.get_manufacture_config(self.manifest_file_path, f"common_dependency/files")
        self.logger.info(f"common_dependency_files \n {common_dependency_files}")
        self.copy_manifest_files(common_dependency_files)

        # 清理 git 文件
        gitkeep_file_list = self.common.py_find_dir("\.gitkeep", self.common.open_code_root, True)
        for gitkeep_file in gitkeep_file_list:
            os.remove(gitkeep_file)
        self.logger.info(f"copy common dependency end")

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="copy for open")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkCutCommonDependencyForOpen(cfg)
    wk.run()
