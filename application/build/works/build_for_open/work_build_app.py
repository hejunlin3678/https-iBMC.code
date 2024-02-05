#!/usr/bin/env python
# coding=utf-8
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
文件名：work_build_app.py
功能：编译app，依赖work_build_app_prepare.py准备环境
版权信息：华为技术有限公司，版本所有(C) 2023-2023
"""

import os
import time
import shutil
import sys
import argparse
import subprocess

cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.build.work_build_app import WorkBuildApp
from utils.config import Config
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE


class WorkBuildApp(WorkBuildApp):
    def __init__(self, config, work_name=""):
        super(WorkBuildApp, self).__init__(config, work_name)
        self.manifest_file_path = f"{self.common.code_root}/application/build/works/build_for_open/manifest.yml"


    def run(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        
        self.set_global()

        # 编译极限模式 ( 定义宏 ULTIMATE_ENABLED )
        if self.config.step == "ultimate":
            self.config.step = ""
            self.logger.info("now build for server with ultimate")
            self.make_package()
        else:
            err_msg = f"{EXCEPTION_CODE[701]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[4]}"
            self.logger.error(f"{err_msg}main: arch is wrong")
            sys.exit(-1)

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", 
        help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    parser.add_argument("-t", "--build_type", 
        help="build type, default value is debug, can be: debug, release, dft_separation", default="debug")
    parser.add_argument("-p", "--app_name", help="app_name", default="")
    parser.add_argument("-v", "--verbose", help="make VERBOSE=1", action="store_true")
    parser.add_argument("-c", "--fuzz_cov", help="Fuzz test coverage", action="store_true")
    parser.add_argument("-a", "--asan_test", help="Asan test for fuzz", action="store_true")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    cfg.set_debug(True)
    cfg.set_build_type(args.build_type)
    cfg.set_app_name(args.app_name)
    cfg.fuzz_cov = args.fuzz_cov
    cfg.asan_test = args.asan_test
    wk = WorkBuildApp(cfg)
    wk.set_verbose(args.verbose)
    wk.run()
