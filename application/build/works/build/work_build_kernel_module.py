#!/usr/bin/env python
# coding=utf-8

"""
文件名：work_build_app.py
功能：编译app，依赖work_build_app_prepare.py准备环境
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import time
import shutil
import sys

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

global delete_target
delete_target = "-d"
global g_rm_board_list
g_rm_board_list = ("RM210", "RM210_SECURITY_3V0")
class WorkBuildKernelModule(Work):
    '''
    '''
    timeout = 600

    def __init__(self, config, work_name = ""):
        super(WorkBuildKernelModule, self).__init__(config, work_name)
        self.linux_kernel_compile_board_lists = ("2488hv6", "1288hv6_2288hv6_5288v6", "TaiShan2280v2_1711", "xh321v6", "DP1210_DP2210_DP4210", "BM320", "BM320_V3", "CST1020V6")
        self.target_path = f"{self.common.temp_path}/target_kernel"
        self.common.check_path(self.target_path)
        os.chdir(self.target_path)

        # 确保 cmake 库路径正确
        os.environ["CMAKE_LIBRARY_PATH"] = f"{self.common.code_path}/../open_source/lib"

        # 设置和打印编译器相关环境变量
        self.config.set_env_global()
        self.common.path_env_revise()
        return

    def install(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return        
        ko_target_dir = f"{self.config.rootfs_path}/opt/pme/lib/modules/ko"
        os.makedirs(ko_target_dir, exist_ok = True)

        # 拷贝 agentless 驱动到 opt/pme/lib/modules/ko 目录下
        all_ko_list = self.common.py_find_dir(".*\.ko", f"{self.target_path}/src/drivers/agentless")
        for ko_file in all_ko_list:
            shutil.copy(ko_file, ko_target_dir)

        try:
            # 拷贝 virtual_cdev 驱动到 opt/pme/lib/modules/ko 目录下
            all_ko_list = self.common.py_find_dir(".*\.ko", f"{self.target_path}/src/drivers/virtual_drv")
            for ko_file in all_ko_list:
                shutil.copy(ko_file, ko_target_dir)
        except:
            self.logger.info(f"{self.config.board_name} not support virtual_drv")

        try:
            # 拷贝 hisport 驱动到 opt/pme/lib/modules/ko 目录下
            all_ko_list = self.common.py_find_dir(".*\.ko", f"{self.target_path}/src/drivers/hisport")
            for ko_file in all_ko_list:
                shutil.copy(ko_file, ko_target_dir)
        except:
            self.logger.info(f"{self.config.board_name} not support hisport")

        # 拷贝 rs485 驱动到 opt/pme/lib/modules/ko 目录下
        all_ko_list = self.common.py_find_dir(".*\.ko", f"{self.target_path}/src/drivers/rs485")
        for ko_file in all_ko_list:
            shutil.copy(ko_file, ko_target_dir)

        try:
            # 拷贝 hi51 协处理器驱动到 opt/pme/lib/modules/ko 目录下
            all_ko_list = self.common.py_find_dir(".*\.ko", f"{self.target_path}/src/hi51/linux_drv")
            for ko_file in all_ko_list:
                shutil.copy(ko_file, ko_target_dir)
        except:
            self.logger.info(f"{self.config.board_name} not support hi51")

        # 拷贝 1822 的 hinic 驱动到 opt/pme/lib/modules/ko 目录下
        if self.config.board_name in g_rm_board_list:
            all_ko_list = self.common.py_find_dir(".*\.ko", f"{self.target_path}/src/drivers/1822")
            for ko_file in all_ko_list:
                shutil.copy(ko_file, ko_target_dir)
            # shutil.copy(f"{self.target_path}/src/drivers/1822/hinicadm", f"{self.config.rootfs_path}/sbin")
            try:
                shutil.copy(f"{self.target_path}/src/drivers/1822/hinicadmdfx",
                            f"{self.common.code_root}/tools/dft_tools/config/RM210")
            except:
                self.logger.info(f"Does not has this file")

        # 拷贝linux kernel的ko到opt/pme/lib/modules/ko目录下
        if self.config.board_name in self.linux_kernel_compile_board_lists:
            self.run_command(f"cp -af {self.target_path}/src/drivers/Linux_Kernel/output/*.ko {ko_target_dir}", super_user=True)

        # 创建编译时间文件
        now = time.strftime("%Y-%m-%d-%H_%M_%S", time.localtime())
        date_file_name = f"{self.config.rootfs_path}/opt/pme/build_date.txt"
        f = open(date_file_name, "w")
        f.write(now)
        f.close()
        self.common.copy_all(self.target_path, self.common.work_out)
        return

    def make_package(self, build_mod=""):
        # agentless BMC 侧驱动编译
        self.common.copy_file_or_dir(f"{self.common.code_path}/../src/drivers",
                        f"{self.target_path}/src")
        self.common.copy_file_or_dir(f"{self.common.code_path}/../infected/drivers/agentless",
                        f"{self.target_path}/src/drivers/")
        os.chdir(f"{self.target_path}/src/drivers/agentless")
        if self.config.board_version == "V5":
            subprocess.run(f"sh build.sh {self.common.code_path}/.. {build_mod}", shell=True)
            # hi51 驱动编译
            self.common.copy_file_or_dir(f"{self.common.code_path}/../src/hi51",
                            f"{self.target_path}/src")
            self.common.copy_file_or_dir(f"{self.common.code_path}/../infected/hi51/linux_drv",
                            f"{self.target_path}/src/hi51")
            os.chdir(f"{self.target_path}/src/hi51/linux_drv")
            subprocess.run("chmod u+x build.sh", shell=True)
            subprocess.run(f"sh build.sh {build_mod} {self.common.code_path}/..", shell=True)

            # rs485 BMC 侧驱动编译
            self.common.copy_file_or_dir(f"{self.common.code_path}/../src/drivers",
                            f"{self.target_path}/src")
            os.chdir(f"{self.target_path}/src/drivers/rs485")
            subprocess.run(f"sh build.sh {self.common.code_path}/.. {build_mod}", shell=True)
        elif self.config.board_version == "V6":
            is_pangea_board = 0
            if self.common.is_pangea_storage_board(self.config.board_name):
                is_pangea_board = 1
            subprocess.run(f"sh build.sh {self.common.code_path}/.. ultimate arm64 {is_pangea_board}", shell=True)
            # rs485 BMC 侧驱动编译
            os.chdir(f"{self.target_path}/src/drivers/rs485")
            subprocess.run(f"sh build.sh {self.common.code_path}/.. ultimate arm64", shell=True)

            # hisport 驱动编译
            self.common.copy_file_or_dir(f"{self.common.code_path}/../src/drivers", f"{self.target_path}/src")
            self.common.copy_file_or_dir(f"{self.common.code_path}/../infected/drivers/hisport", f"{self.target_path}/src/drivers")
            os.chdir(f"{self.target_path}/src/drivers/hisport")
            subprocess.run(f"sh build.sh {self.common.code_path}/.. ultimate arm64", shell=True)

            # 1822 hinic BMC侧驱动编译
            if self.config.board_name in g_rm_board_list:
                os.chdir(f"{self.target_path}/src/drivers/1822")
                zip_file_list = self.common.py_find_dir("IN500_solution.*\.zip", f"{self.common.temp_path}/platforms/1822/driver")
                for zip_file in zip_file_list:
                    subprocess.run(f"unzip {zip_file} >/dev/null", shell=True)

                os.makedirs("hinic", exist_ok=True)
                self.common.copy_all("hinic-1.8.3.3/hinic", "hinic")
                targz_file_list = self.common.py_find_dir("hinicadm.*\.tar\.gz", f"{self.common.temp_path}/platforms/1822/tools")
                for targz_file in targz_file_list:
                    subprocess.run(f"tar -zxf {targz_file}", shell=True)
                subprocess.run(f"sh build.sh {self.common.code_path}/.. ultimate arm64", shell=True)

        # linux kernel wifi和usb rndis相关ko编译
        if self.config.board_name in self.linux_kernel_compile_board_lists:
            self.common.copy_file_or_dir(f"{self.common.code_path}/../../open_source/Linux_Kernel", f"{self.target_path}/src/drivers")
            self.common.copy_file_or_dir(f"{self.common.code_path}/../platform_v6/Module.symvers", f"{self.target_path}/src/drivers/Linux_Kernel")
            os.chdir(f"{self.target_path}/src/drivers/Linux_Kernel")
            subprocess.run("chmod a+x build.sh", shell=True)
            subprocess.run("sh build.sh", shell=True)

    def run(self):
        self.make_package()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    parser.add_argument("-t", "--build_type", help="build type, default value is debug, can be: debug, release, dft_separation", default="debug")
    parser.add_argument("-d", "--delete_target", help="delete temporary files after build", const="-d", action="store_const")
    args = parser.parse_args()
    delete_target = args.delete_target
    cfg = Config(args.board_name)
    cfg.set_build_type(args.build_type)
    wk = WorkBuildKernelModule(cfg)
    wk.run()
    wk.install()
