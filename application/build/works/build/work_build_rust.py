#!/usr/bin/env python
# coding: utf-8

"""
功 能：build_java脚本，该脚本构建kvm_client
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import shutil
import subprocess
import sys
import os
import re

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.common import Common
from utils.config import Config

from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildRust(Work):
    timeout = 600
    def move_rust_dependency_inc(self):
        # 复制rust编译所需的头文件
        if not os.path.isdir(f"{self.common.code_root}/application/src/include/open_source"):
            subprocess.run(f"cp -af {self.common.code_root}/tools/others/other_tools/open_source \
                {self.common.code_root}/application/src/include/", shell=True)

    def run(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        self.move_rust_dependency_inc()
        cmake_env_dir = f"{self.common.temp_path}/cmake_env_dir"
        if not os.path.exists(cmake_env_dir):
            os.makedirs(cmake_env_dir)

        # 设置和打印编译器相关环境变量
        self.config.set_env_global()
        self.common.path_env_revise()
        self.logger.info("----->>>>rust build start<<<<-----")
        work_dir = self.common.code_path
        os.environ["LIBRARY_PATH"] = f"{work_dir}/../src/lib_open_source"
        luajit_inc_dir = f"{self.common.pme_dir}/src/include/open_source/luajit2"
        jsonc_inc_dir = f"{self.common.pme_dir}/src/include/open_source/json"
        glib_inc_dir1 = f"{self.config.sysroot_base}/sdk/usr/include/glib-2.0"
        glib_inc_dir2_v5 = f"{self.config.sysroot_base}/sdk/usr/lib/glib-2.0/include"
        glib_inc_dir2_v6 = f"{self.config.sysroot_base}/sdk/usr/lib64/glib-2.0/include"
        glib_inc_dir3 = f"{self.config.sysroot_base}/sdk/usr/include"
        dflib_path = f"{self.common.pme_dir}/sdk/PME/V100R001C10/lib"
        self.common.copy_all(f"{work_dir}/../src/apps/web_host", f"{self.common.temp_path}/web_host")
        os.chdir(f"{self.common.temp_path}/web_host")
        self.common.check_path("out")
        if self.config.board_version == "V5":
            subprocess.run(f"make arm-linux \
                LUA_INCLUDE={luajit_inc_dir} LUA_JSONC_INCLUDE={jsonc_inc_dir} \
                GLIB_INCLUDE_PATH={glib_inc_dir1} GLIB_INCLUDE_PATH2={glib_inc_dir2_v5} \
                GLIB_INCLUDE_PATH3={glib_inc_dir3} DFLIB_PATH={dflib_path} COMPATIBILITY_ENABLED={self.common.compability_enable} \
                    SECURITY_ENHANCED_COMPATIBLE_BOARD_V1={self.common.security_enhanced_compatible_board_v1} \
                    SECURITY_ENHANCED_COMPATIBLE_BOARD_V5={self.common.security_enhanced_compatible_board_v5} \
                    SECURITY_ENHANCED_COMPATIBLE_BOARD_V3={self.common.security_enhanced_compatible_board_v3}", shell=True)
        if self.config.board_version == "V6":
            subprocess.run(f"make aarch64-linux \
                LUA_INCLUDE={luajit_inc_dir}  LUA_JSONC_INCLUDE={jsonc_inc_dir} \
                GLIB_INCLUDE_PATH={glib_inc_dir1} GLIB_INCLUDE_PATH2={glib_inc_dir2_v6} \
                GLIB_INCLUDE_PATH3={glib_inc_dir3} DFLIB_PATH={dflib_path} COMPATIBILITY_ENABLED={self.common.compability_enable} \
                    SECURITY_ENHANCED_COMPATIBLE_BOARD_V1={self.common.security_enhanced_compatible_board_v1} \
                    SECURITY_ENHANCED_COMPATIBLE_BOARD_V5={self.common.security_enhanced_compatible_board_v5} \
                    SECURITY_ENHANCED_COMPATIBLE_BOARD_V3={self.common.security_enhanced_compatible_board_v3}", shell=True)

        self.logger.info("----->>>>rust build finish<<<<-----")
        return

    def install(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return        
        dst_path = f"{self.config.rootfs_path}/opt/pme/bin/portal"
        src_path = f"{self.common.temp_path}/web_host/release/opt/pme/bin/portal"
        if os.path.isdir(dst_path):
            shutil.rmtree(dst_path, ignore_errors=True)
        os.makedirs(dst_path, 0o755, exist_ok=True)
        self.run_command(f"cp -af {src_path}/* {dst_path}", super_user=True)

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    # pack_java.py未移植完成，以下代码是为保持shell调用方式功能正确
    board = "2488hv6"
    if len(sys.argv) >= 2:
        board = sys.argv[1]
    cfg = Config(board, "target_personal")
    wk = WorkBuildRust(cfg)
    wk.run()
