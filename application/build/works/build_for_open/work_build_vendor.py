#!/usr/bin/python
# -*- coding: UTF-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-14 创建
自定义参数：package_name 可选all或opensource_build.json中定义的软件包名
'''
import sys
import os
import json
import subprocess
import argparse
import traceback
import shutil
cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.opensource.work_build_vendor import WorkBuildVendor
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.log import Logger


class WorkBuildVendor(WorkBuildVendor):
    def __init__(self, config, work_name=""):
        super(WorkBuildVendor, self).__init__(config, work_name)
        vendor_json_file = f"{self.common.code_root}/application/build/works/build_for_open/work_build_vendor.json"
        with open(vendor_json_file, "r") as json_file:
            self.choices = json.load(json_file)
        self.vendor_dir = f"{self.common.code_root}/vendor"
        self.package_name = "all"
        self.manifest_file_path = f"{self.common.code_root}/application/build/works/build_for_open/manifest.yml"

    def patch_src(self):
        patch_src_files = self.get_manufacture_config(self.manifest_file_path, f"patch_src/storelib/files")
        for patch in patch_src_files:
            src_directory = patch.get("directory")
            patch_file_path = patch.get("patch_file")
            os.chdir(src_directory)
            self.run_command(f"patch -p0 <{patch_file_path}")

    def run(self):
        self.config.set_env_global()
        os.chdir(self.vendor_dir)
        # 如果没有源码就退出。
        if not os.path.isdir("./StoreLib/versionChangeSet") or \
        not os.path.isdir("./StoreLib3108/versionChangeSet") or \
        not os.path.isdir("./StoreLibIR3/versionChangeSet") or \
        not os.path.isdir("./StoreLibIT/versionChangeSet"):
            self.logger.info(f">>>>>>>> no src dir, quit WorkBuildVendor")
            return
        
        # storelib源码由伙伴从厂商获取，编译前先打patch适配以iBMC
        self.patch_src()

        for package in self.choices:
            if package == self.package_name or self.package_name == "all":
                self.logger.info(">>>>>>>>>>>>>>>>>>>>>>>>>>>> build package: %s, self:%s", package, self.package_name)
                self.vender_build(package, self.choices[package], self.config.board_version)
        
        output_path = f"{self.common.code_root}/output/vendor/arm64"
        # StoreLib
        self.common.copy_file_or_dir(f"{output_path}/StoreLib/libstorelib.so.07.1602.0100.0000", 
            self.common.lib_open_source)
        # StoreLib3108
        self.common.copy_file_or_dir(f"{output_path}/StoreLib3108/libstorelib.so.07.0604.0100.1000", 
            self.common.lib_open_source)
        # StoreLibIR3
        self.common.copy_file_or_dir(f"{output_path}/StoreLibIR3/libstorelibir3.so.16.02-0", 
            self.common.lib_open_source)
        # StoreLibIT
        self.common.copy_file_or_dir(f"{output_path}/StoreLibIT/libstorelibit.so.07.0700.0200.0600", 
            self.common.lib_open_source)

        os.chdir(self.common.lib_open_source)
        # StoreLib
        self.common.rm_symbol_link("libstorelib.so")
        os.symlink("libstorelib.so.07.1602.0100.0000", "libstorelib.so")
        # StoreLib3108
        self.common.rm_symbol_link("libstorelib.so.07.06")
        os.symlink("libstorelib.so.07.0604.0100.1000", "libstorelib.so.0706")
        # StoreLibIR3
        self.common.rm_symbol_link("libstorelibir3.so")
        os.symlink("libstorelibir3.so.16.02-0", "libstorelibir3.so")
        # StoreLibIT
        self.common.rm_symbol_link("libstorelibit.so")
        os.symlink("libstorelibit.so.07.0700.0200.0600", "libstorelibit.so")

# 以下为提供脚本单独调试功能示例代码，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-p", "--package_name", help="open source package name", default="all")
    parser.add_argument("-b", "--board_name", 
        help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkBuildVendor(cfg)
    wk.set_package_name(args.package_name)
    wk.run()