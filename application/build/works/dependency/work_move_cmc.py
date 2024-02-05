#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import time
import subprocess
import sys
import os
import argparse
import shutil
import tarfile
import zipfile
import re

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkMoveCmc(Work):
    def set_global(self):
        """
        配置全局路径
        """
        self.bin_cmc_path = self.common.bin_cmc_path
        self.internal_release_dir = self.common.internal_release_path
        self.temp_path = self.common.temp_path
        return

    def get_env_param(self):
        """
        未指定参数，属于其他编译路径（非1710,1710的编译路径已指定）遗漏的场景，应当默认功能关
        """
        if self.config.board_name == "":
            self.logger.info("no board input")
        elif self.config.board_name == "allboard":
            self.logger.info("enable by default")
        else:
            ret = subprocess.run(f"grep {self.config.board_name} compatible_board.list", shell=True)
            if not ret.returncode:
                self.logger.info("in board list")
            else:
                self.logger.info("not in board list")
        return

    def run(self):
        self.set_global()
        os.chdir(self.common.code_path)

        self.get_env_param()

        os.chdir(self.common.code_root)


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    wk = WorkMoveCmc(cfg)
    wk.run()
