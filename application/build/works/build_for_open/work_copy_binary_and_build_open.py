#!/usr/bin/env python
# coding=utf-8

"""
文件名：work_copy_binary_and_build_open.py
功能：该脚本用于执行二进制移动
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

class WorkCopyBinaryAndBuildOpen(Work):
    def __init__(self, config, work_name=""):
        super(WorkCopyBinaryAndBuildOpen, self).__init__(config, work_name)
        self.manifest_file_path = f"{self.common.code_root}/prepare_for_open/manifest.yml"

    def package_for_open(self):
        # 切到代码仓目录
        os.chdir(f"{self.common.temp_path}/build_open")
        self.common.remove_file(f"{self.common.work_out}/V2R2_trunk.tar.gz")
        self.common.remove_file(f"{self.common.open_code_root}/application/src_bin/rootfs/opt/pme/web/htdocs/bmc/pages/jar/vconsole.jar.sig")
        # 打包到output
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E tar --format=gnu -czvf {self.common.work_out}/V2R2_trunk.tar.gz ./V2R2_trunk", shell=True)
        self.logger.info(f"----package_for_open end----")
        return
    
    def clean_file(self, re_exp, dir_path):
        res_file_list = self.common.py_find_dir(re_exp, dir_path, True)
        for res_file in res_file_list:
            self.logger.info("----clean_file---- %s", res_file)
            os.remove(res_file)

    def run(self):
        self.logger.info(f"copy binary start")

        # 拷贝裁剪清单里 binary 的文件
        build_result_files = self.get_manufacture_config(self.manifest_file_path, f"build_result/files")
        # self.logger.info(f"build_result_files \n {build_result_files}")
        self.copy_manifest_files(build_result_files)
        
        build_type_turple = ["debug", "release", "dft_separation"]
        for arg_build_type in build_type_turple:
            self.config.set_build_type(arg_build_type)
            self.manifest_obj = None
            self.logger.info(f"set_build_type {arg_build_type}")
            build_result_temp_files = self.get_manufacture_config(self.manifest_file_path, f"build_result_temp/files")
            self.copy_manifest_files(build_result_temp_files)
            # 拷贝 app 相关的文件
            build_result_app_files = self.get_manufacture_config(self.manifest_file_path, f"build_result_app/files")
            self.copy_manifest_files(build_result_app_files)

        # 清理 git 文件
        os.chdir(f"{self.common.temp_path}/build_open")
        self.clean_file(r"\.gitkeep", f"{self.common.open_code_root}")
        # 清理 __pycache__ 文件夹
        pycache_list = self.common.py_find_directory("__pycache__", f"{self.common.open_code_root}", True)
        for pycache_dir in pycache_list:
            shutil.rmtree(pycache_dir)
        # 清理不在开放范围的C文件
        self.clean_file(r"\S*\.c", f"{self.common.open_code_root}/tools/toolchain")
        self.clean_file(r"\S*\.c", f"{self.common.open_code_root}/temp/tiny_temp/webapp/node_modules")
        self.clean_file(r"\S*\.c", f"{self.common.open_code_root}/temp/debug/target_app")
        self.clean_file(r"\S*\.c", f"{self.common.open_code_root}/temp/release/target_app")
        # 打包
        self.package_for_open()
        
# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="copy for open")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkCopyBinaryAndBuildOpen(cfg)
    wk.run()
