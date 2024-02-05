#!/usr/bin/env python
# coding=utf-8
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
文件名：work_build_app_prepare.py
功能：从work_build_app.py中拆出，用于完成公共环境准备，以实现多个work_build_app.py并发运行
版权信息：华为技术有限公司，版本所有(C) 2019-2020
可配参数：only_app: 为true时仅编译app不打包，可以不用下载开源源码
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
from works.build.work_build_app_prepare import WorkBuildAppPrepare
from utils.config import Config
from utils.common import Common


class WorkBuildAppPrepare(WorkBuildAppPrepare):
    def __init__(self, config, work_name=""):
        super(WorkBuildAppPrepare, self).__init__(config, work_name)
        self.manifest_file_path = f"{self.common.code_root}/application/build/works/build_for_open/manifest.yml"
        self.manifest_obj = None
    
    # 拷贝需要的文件
    def copy_build_type_related(self):
        self.manifest_obj = None
        self.logger.info("self.config.build_type %s", self.config.build_type)
        build_type_related_files = self.get_manufacture_config(self.manifest_file_path, f"build_type_related/files")
        self.logger.info("copy build_type_related_files, %s", build_type_related_files)
        self.copy_manifest_files(build_type_related_files)

    def move_vpp(self):
        # vpp组件
        vpp_dir = "./platforms/vpp"
        os.chdir(f"{self.common.code_root}/application/src_bin")
        if os.path.isdir(vpp_dir):
            shutil.rmtree(vpp_dir)
        shutil.copytree(f"{self.common.code_root}/temp/vpp", vpp_dir)
        return

    def run(self):
        self.set_global()
        self.copy_build_type_related()
        # 自动更新 event_lang_template_v2.xml 版本
        self.update_event_lang_template_version()

        self.run_command(f"sh common.sh {self.common.code_path} {self.config.board_name}")

        # 这是主要操作
        if not self.only_app:
            self.xml_formula_check()

        self.common.remove_path(self.config.work_out) 
        os.makedirs(self.config.work_out)
        self.common.check_path(f"{self.common.code_root}/output")
        self.copy_rootfs()

        if self.common.security_enhanced_compatible_board_v2 == 0:
            self.add_weakdictionary()

        self.get_sdk()

        self.move_vpp()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", 
        help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkBuildAppPrepare(cfg)
    wk.run()