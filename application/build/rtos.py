#!/usr/bin/env python
# coding: utf-8
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

import os
import stat
import subprocess

from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

logger = Logger().logger

class Rtos:
    """
    功能描述:设置RTOS相关配置及方法
    接口：无
    """
    global g_open_source_bin_dir
    global g_rtos_skd_dir
    global g_cross_compile_install_path
    global g_cpu
    global g_kernel
    global g_arg_flag_arm64
    global g_libstdcpp_install_path
    global g_vision_file
    global g_lib_path
    global g_libstdcpp
    global g_rtos_common
    global g_rtos_install_sh
    global g_arm_name
    global g_http_patch_file
    global g_rtos_path
    g_rtos_common = Common()
    g_rtos_set_sh = g_rtos_common.rtos_dlsetenv

    def __init__(self, arm64_flag = False):
        self.set_rtos_evn()
        board_version = "V6" if arm64_flag else "V5"
        self.g_cross_compile_install_path = g_rtos_common.get_hcc_path(version=board_version)
        self.g_rtos_path = g_rtos_common.get_rtos_path(version=board_version)
        self.g_vision_file = f"{self.g_rtos_path}/RTOS-Revision"
        if arm64_flag:
            self.g_open_source_bin_dir = os.path.join(g_rtos_common.open_source_path, "arm64")
            self.g_rtos_skd_dir = g_rtos_common.rtos_arm64_path
            self.g_cpu = "arm64le"
            self.g_kernel = "5.10_ek_preempt_pro"
            self.g_arg_flag_arm64 = "1"
            self.g_libstdcpp_install_path = os.path.join(os.environ.get("RTOS_ARM64_PATH"), "sdk/usr/lib64")
            self.g_lib_path = os.path.join(self.g_cross_compile_install_path, "aarch64-target-linux-gnu/lib64")
            self.g_libstdcpp = os.path.join(self.g_libstdcpp_install_path, "libstdc++.so")
            self.g_rtos_install_sh = "install.sh"
            self.g_arm_name = "ARM64"
            self.g_http_patch_file = "huawei_pcre-config-64.patch"
        else:
            self.g_open_source_bin_dir = g_rtos_common.open_source_path
            self.g_rtos_skd_dir = g_rtos_common.rtos_path
            self.g_cpu = "arm32A9le"
            self.g_kernel = "5.10_ek_preempt_pro"
            self.g_arg_flag_arm64 = "0"
            self.g_libstdcpp_install_path = os.path.join(os.environ.get("RTOS_ARM32_PATH"), "sdk/usr/lib")
            self.g_lib_path = os.path.join(self.g_cross_compile_install_path, "arm-linux-gnueabi/lib")
            self.g_libstdcpp = os.path.join(self.g_libstdcpp_install_path, "libstdc++.so")
            self.g_rtos_install_sh = "install_rtos_sdk.sh"
            self.g_arm_name = "ARM32"
            self.g_http_patch_file = "huawei_pcre-config.patch"

    def set_rtos_evn(self):
        """
        功能描述：设置 RTOS_PATH 环境变量
        参数：无
        返回值：无
        """
        os.environ["RTOS_ARM32_PATH"] = g_rtos_common.get_rtos_path(version="V5")
        os.environ["RTOS_ARM64_PATH"] = g_rtos_common.get_rtos_path(version="V6")

    def install_rtos(self):
        """
        功能描述：安装rtos
        参数：无
        返回值：无
        """
        i = 0
        while i < 3:
            sdk_revision = self.check_version()
            if not self.check_match_file(sdk_revision) or not os.path.exists(self.g_rtos_set_sh):
                logger.info('Check RTOS SDK version failed!')

                os.chdir(self.g_rtos_skd_dir)
                logger.info(f"install {self.g_arm_name} rtos begin...")

                g_rtos_common.run_command(f"sh {self.g_rtos_install_sh}", f"install {self.g_arm_name} rtos sdk ERR!")
                i = i + 1
                logger.info(f"install {self.g_arm_name} rtos end!")
            else:
                logger.info(f'Check {self.g_arm_name} RTOS SDK version successfully!')
                open_source_env_dir = f"{g_rtos_common.temp_path}/open_source_env_dir"
                if not os.path.exists(open_source_env_dir):
                    os.makedirs(open_source_env_dir)

                subprocess.run(f"source {self.g_rtos_set_sh} -p {self.g_cpu}_{self.g_kernel} \
                    --sdk-path={self.g_cross_compile_install_path} && \
                    env > {open_source_env_dir}/open_source_env.txt",
                    shell=True)
                break

        if i == 3:
            err_msg = f"{EXCEPTION_CODE[403]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[3]}"
            logger.error(f"{err_msg}Update RTOS SDK failed. Please make sure that you have the permission and the SDK packages are exist")
            os._exit(1)

    def check_version(self):
        """
        功能描述：检查rtos版本号
        参数：无
        返回值：版本号
        """
        sdk_revision = ""
        if os.path.exists(self.g_vision_file):
            with open(rf'{self.g_vision_file}') as somefile:
                # 取第一行，去换行符 获取版本号
                sdk_revision = somefile.readline().strip('\n')
        logger.info(f'sdk_revision = {sdk_revision}')
        return sdk_revision

    def check_match_file(self, sdk_revision):
        """
        功能描述：检查是否有匹配版本号的文件
        参数：sdk_revision 版本号
        返回值：True,False
        """
        files = os.listdir(self.g_rtos_skd_dir)
        version_match_flag = False
        if sdk_revision != "":
            for f in files:
                if sdk_revision in f:
                    version_match_flag = True

        return version_match_flag