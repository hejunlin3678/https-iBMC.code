#!/usr/bin/env python
# coding:utf-8

"""
文件名：set_env_for_compile.py
功能：编译自研工具
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""


import os
import sys
import shutil
import tarfile
import argparse
import subprocess
import time

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

logger = Logger().logger


class WorkSetEnvForCompileArm(Work):
    '''
    '''

    def set_global(self):

        self.rtos_version = "208.5.0"
        if self.config.board_name == "":
            self.board_version = "V5"
        else:
            self.board_version = self.common.get_board_version(self.config.board_name)
        self.open_sdk_dir = f"{self.common.pme_dir}/sdk"
        self.js_clean_tool_dir = f"{self.common.code_root}/tools/toolchain/js_clean"
        self.open_pm_dir = ""
        self.libstdcpp = ""
        self.libstdcpp_install_path = ""

        if self.board_version == "V6":
            self.open_pm_dir = f"{self.common.pme_dir}/platform_v6"
            self.libstdcpp = f"{self.config.sysroot_base}/sdk/usr/lib64/libstdc++.so"
            self.libstdcpp_install_path = f"{self.config.sysroot_base}/sdk/usr/lib64"
        elif self.board_version == "V5":
            self.open_pm_dir = f"{self.common.pme_dir}/platform_v5"
            self.libstdcpp = f"{self.config.sysroot_base}/sdk/usr/lib/libstdc++.so"
            self.libstdcpp_install_path = f"{self.config.sysroot_base}/sdk/usr/lib"
        else:
            err_msg = f"{EXCEPTION_CODE[602]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
            logger.error(f"{err_msg}parameter error!")
            os._exit(1)
        return


    def get_shell_cmd_result(self, cmd):
        ret = subprocess.getstatusoutput(cmd)
        return ret[1]


    def move_file_or_dir(self, src_dir, dst_dir):
        subprocess.run(f"mv {src_dir} {dst_dir}", shell=True)
        return

    def set_data_dir_sync(self, dst_dir):
        """
        设置数据文件夹同步
        """
        if not os.path.isdir(f"{self.common.code_root}/application/rootfs/opt/pme/conf/data_dir_sync2/3/{dst_dir}"):
            self.move_file_or_dir(f"{self.common.code_root}/bin_cmc/application/rootfs/opt/pme/conf/data_dir_sync2/3/{dst_dir}",
                            f"{self.common.code_root}/application/rootfs/opt/pme/conf/data_dir_sync2/3")
        else:
            return

        os.chdir(f"{self.common.code_root}/application/rootfs/opt/pme/conf/data_dir_sync2/3/{dst_dir}")
        self.common.tar_to_dir("./", f"../{dst_dir}")
        if not os.path.isdir(f"{self.common.code_root}/bin_cmc/application/rootfs/opt/pme/conf/data_dir_sync2/3"):
            os.makedirs(f"{self.common.code_root}/bin_cmc/application/rootfs/opt/pme/conf/data_dir_sync2/3")
        self.move_file_or_dir(f"{self.common.code_root}/application/rootfs/opt/pme/conf/data_dir_sync2/3/{dst_dir}",
                        f"{self.common.code_root}/bin_cmc/application/rootfs/opt/pme/conf/data_dir_sync2/3")
        return


    def check_rootfs(self):
        """
        同步 rootfs 文件
        """
        if os.path.isdir(f"{self.common.code_root}/application/rootfs"):
            self.set_data_dir_sync(".factory_recover_point")
            self.set_data_dir_sync(".factory_recover_point_v5")
            subprocess.run(f"ls -al {self.common.code_root}/application/rootfs/opt/pme/conf/data_dir_sync2/3", shell=True)
        return

    def check_libstdcpp(self):
        if not os.path.exists(self.libstdcpp):
            if self.board_version == "V6":
                subprocess.run(f"cp -a {self.config.cross_compile_install_path}/aarch64-target-linux-gnu/lib64/libstdc++.* {self.libstdcpp_install_path}",
                            shell=True)
            elif self.board_version == "V5":
                subprocess.run(f"cp -a {self.config.cross_compile_install_path}/arm-linux-gnueabi/lib/libstdc++* {self.libstdcpp_install_path}",
                            shell=True)
        return

    def check_debug(self):
        if self.config.debug:
            logger.info(f"self.common.pme_dir {self.common.pme_dir}")
            logger.info(f"self.common.lib_open_source {self.common.lib_open_source}")
        return

    def run(self):
        '''
        '''
        # build_target中，在执行set_env_for_compile_arm32_64.py之前执行了一下两个步骤，暂时先补充到这里
        os.chdir(self.common.code_path)

        self.set_global()
        if self.board_version == "V5":
            self.check_rootfs()

        self.check_libstdcpp()
        self.check_debug()

        subprocess.run(f"find \"{self.common.pme_dir}\" -name \'*.sh\' -type f | xargs chmod u+x > /dev/null 2>&1", shell=True)
        subprocess.run(f"find \"{self.common.pme_dir}\" -name \'*.py\' -type f | xargs chmod u+x > /dev/null 2>&1", shell=True)

        os.chdir(self.open_pm_dir)
        self.common.remove_path(self.open_sdk_dir)
        os.makedirs(self.open_sdk_dir)
        self.common.untar_to_dir("PME_SDK.tar.gz", self.open_sdk_dir)

        # 清理 js 脚本
        os.chdir(self.js_clean_tool_dir)
        logger.info(os.getcwd())
        ret = subprocess.run("chmod +x set_env.sh", shell=True)
        if ret.returncode:
            err_msg = f"{EXCEPTION_CODE[631]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}"
            logger.error(f"{err_msg}make js tool faild")
            os._exit(1)
        else:
            logger.info("make js tool success")
        os.chdir(self.common.code_path)


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    wk = WorkSetEnvForCompileArm(cfg)
    wk.run()