#!/usr/bin/env python
# coding: utf-8
"""
功 能：buildimg_ext4脚本，该脚本 make ext4 image
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import shutil
import subprocess
import sys
import time
import json
import argparse
from pathlib import Path

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.config import Config
from datetime import datetime
from works.packet.check_file_list import CheckFileList
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildRootfsImg(Work):
    def run(self):
        # self.set_evn()
        # 设置sudo来执行代码，放到default当中
        self.super_user_run = True
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        if not os.path.isdir(f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}"):
            err_msg = f"{EXCEPTION_CODE[630]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[1]}"
            self.logger.error(f"{err_msg}{self.common.code_root}/application/src/resource/board/{self.config.board_name} is not exit!")
            os._exit(1)

        self.customization.rootfs_base_deploy(self.config.buildimg_dir)
        self.customization.rootfs_base_conf(self.config.buildimg_dir)
        self.customization.datafs_deploy(self.config.buildimg_dir)
        if self.config.build_type == "release":
            self.customization.rootfs_ro_deploy(self.config.buildimg_dir)
        if self.config.build_type == "debug":
            self.customization.rootfs_rw_deploy(self.config.buildimg_dir)
        if self.config.build_type == "dft_separation":
            self.customization.rootfs_dft_deploy(self.config.buildimg_dir)

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    cfg.set_debug(True)
    wk = WorkBuildRootfsImg(cfg)
    wk.run()
    print("end here")
