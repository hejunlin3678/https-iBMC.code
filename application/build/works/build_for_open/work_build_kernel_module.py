#!/usr/bin/env python
# coding=utf-8
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
文件名：work_build_app.py
功能：编译app，依赖work_build_app_prepare.py准备环境
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import stat
import time
import shutil
import sys
import argparse
import subprocess

cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.build.work_build_kernel_module import WorkBuildKernelModule
from utils.config import Config
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

global delete_target
delete_target = "-d"
global g_rm_board_list
g_rm_board_list = ("RM210", "RM210_SECURITY_3V0")


class WorkBuildKernelModule(WorkBuildKernelModule):

    def install(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return        
        ko_target_dir = f"{self.config.rootfs_path}/opt/pme/lib/modules/ko"
        os.makedirs(ko_target_dir, exist_ok=True)

        # 拷贝 1822 的 hinic 驱动到 opt/pme/lib/modules/ko 目录下
        if self.config.board_name in g_rm_board_list:
            all_ko_list = self.common.py_find_dir(".*\.ko", f"{self.target_path}/src/drivers/1822")
            for ko_file in all_ko_list:
                shutil.copy(ko_file, ko_target_dir)
            try:
                shutil.copy(f"{self.target_path}/src/drivers/1822/hinicadmdfx",
                            f"{self.common.code_root}/tools/dft_tools/config/RM210")
            except Exception as e:
                self.logger.error("copy src/drivers/1822/hinicadmdfx error")
                raise e

        # 拷贝linux kernel的ko到opt/pme/lib/modules/ko目录下
        if self.config.board_name in self.linux_kernel_compile_board_lists:
            self.run_command(f"cp -af {self.target_path}/src/drivers/Linux_Kernel/output/*.ko {ko_target_dir}", 
                super_user=True)

        # 创建编译时间文件
        now = time.strftime("%Y-%m-%d-%H_%M_%S", time.localtime())
        date_file_name = f"{self.config.rootfs_path}/opt/pme/build_date.txt"
        if not os.path.exists(date_file_name):
            flags = os.O_WRONLY | os.O_CREAT | os.O_EXCL
            modes = stat.S_IWUSR | stat.S_IRUSR
            with os.fdopen(os.open(date_file_name, flags, modes), 'w') as fout:
                fout.write(now)
                fout.close()
        shutil.copytree(self.target_path, self.common.work_out, dirs_exist_ok=True)
        return

    def make_package(self, build_mod=""):
        if self.config.board_version == "V5":
            self.run_command(f"sh build.sh {self.common.code_path}/.. {build_mod}")
            # hi51 驱动编译
            self.common.copy_file_or_dir(f"{self.common.code_path}/../src/hi51",
                            f"{self.target_path}/src")
            self.common.copy_file_or_dir(f"{self.common.code_path}/../infected/hi51/linux_drv",
                            f"{self.target_path}/src/hi51")
            os.chdir(f"{self.target_path}/src/hi51/linux_drv")
            self.run_command(f"chmod u+x {self.target_path}/src/hi51/linux_drv/build.sh")
            self.run_command(f"sh build.sh {build_mod} {self.common.code_path}/..")

            # rs485 BMC 侧驱动编译
            self.common.copy_file_or_dir(f"{self.common.code_path}/../src/drivers",
                            f"{self.target_path}/src")
            os.chdir(f"{self.target_path}/src/drivers/rs485")
            self.run_command(f"sh build.sh {self.common.code_path}/.. {build_mod}")
        elif self.config.board_version == "V6":
            is_pangea_board = 0
            if self.common.is_pangea_storage_board(self.config.board_name):
                is_pangea_board = 1
            
            # 1822 hinic BMC侧驱动编译
            if self.config.board_name in g_rm_board_list:
                os.chdir(f"{self.target_path}/src/drivers/1822")
                zip_file_list = self.common.py_find_dir("IN500_solution.*\.zip", 
                    f"{self.common.temp_path}/platforms/1822/driver")
                for zip_file in zip_file_list:
                    self.run_command(f"unzip {zip_file} >/dev/null")

                os.makedirs("hinic", exist_ok=True)
                self.common.copy_all("hinic-1.8.3.3/hinic", "hinic")
                targz_file_list = self.common.py_find_dir("hinicadm.*\.tar\.gz", 
                    f"{self.common.temp_path}/platforms/1822/tools")
                for targz_file in targz_file_list:
                    self.run_command(f"tar -zxf {targz_file}")
                self.run_command(f"sh build.sh {self.common.code_path}/.. ultimate arm64")

    def run(self):
        self.make_package()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", 
        help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    parser.add_argument("-t", "--build_type", 
        help="build type, default value is debug, can be: debug, release, dft_separation", default="debug")
    parser.add_argument("-d", "--delete_target", help="delete temporary files after build", 
        const="-d", action="store_const")
    args = parser.parse_args()
    delete_target = args.delete_target
    cfg = Config(args.board_name)
    cfg.set_build_type(args.build_type)
    wk = WorkBuildKernelModule(cfg)
    wk.run()
    wk.install()
