#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：下载并安装rtos/hcc工具
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import time
import subprocess
import sys
import os
import shutil
import argparse
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkDownloadBuildTools(Work):
    def set_global(self):
        shutil.rmtree(f"{self.common.temp_path}/build_tools", ignore_errors=True)

    def __init__(self, config, work_name = ""):
        super(WorkDownloadBuildTools, self).__init__(config, work_name)
        self.board_version = self.config.board_version

    def get_oem_buildtools(self, dependency_file):
        self.logger.info('get OEM buildtools begin...')
        # 支持OEM 定制化修改, 使用build_tree里面的dependency
        oem_dependency_file = os.path.join(self.common.oem_config_path, "dependency_buildtools.xml")
        if os.path.exists(oem_dependency_file):
            dependency_file = os.path.realpath(oem_dependency_file)
            self.logger.info('use OEM buildtools config...')

        subprocess.run(f"cat {dependency_file} | grep -E 'componentName|componentVersion'", shell=True)
        return dependency_file

    def download_tools(self):
        self.logger.info('download dependency tools begin...')
        agentpath = os.path.join(self.common.temp_path, "build_tools")
        self.common.check_path(agentpath)
        dependency_file = self.get_path("dependency_for_hpm/downxml/dependency_buildtools.xml")
        dependency_file = self.get_oem_buildtools(dependency_file)
        cmd = f"artget pull -d {dependency_file} -ap {agentpath}"
        self.run_command(cmd)
        self.logger.info('download dependency tools end!')
        # 移动到tools/build_tools目录中
        self.common.copy_all(f"{self.common.code_root}/tools/build_tools", f"{self.common.temp_path}/build_tools")
        self.logger.info("move dependency tools done")



    def check_rtos_sdk(self, board_version):
        """
        验证 rtos sdk 版本
        """
        if board_version == "V6":
            rtos_sdk_dir = f"{self.common.temp_path}/build_tools/rtos-sdk-arm64"
        elif board_version == "V5":
            rtos_sdk_dir = f"{self.common.temp_path}/build_tools/rtos-sdk"
        self.logger.info(f"build {board_version} toolchain...")

        os.chdir(self.common.code_path)
        i = 0
        while i < 3:
            while 1:
                flag = self.common.re_match_list(f"rpm_rtos.flag_.*", os.listdir("/opt"))
                if not len(flag):
                    self.logger.info("/opt/rpm_rtos.flag 文件不存在，继续执行...")
                    break
                else:
                    time.sleep(0.1)
            if board_version == "V6":
                exitcode, sdk_reversion = subprocess.getstatusoutput(f"sed -n 1p {self.config.sysroot_base}/RTOS-Revision")
                ret = self.common.get_shell_command_result(f"sed -n 1p {self.config.sysroot_base}/RTOS-Revision")
                self.logger.info(f"{ret}")
            elif board_version == "V5":
                exitcode, sdk_reversion = subprocess.getstatusoutput(f"sed -n 1p {self.config.sysroot_base}/RTOS-Revision")
                ret = self.common.get_shell_command_result(f"sed -n 1p {self.config.sysroot_base}/RTOS-Revision")
                self.logger.info(f"{ret}")
            # 打印找到的版本号，列出当前目录文件，获取匹配列表
            if not exitcode and os.path.exists(self.config.cross_compile_install_path):
            # if not exitcode and os.path.exists(self.config.cross_compile_install_path):
                self.logger.info(sdk_reversion)
                self.logger.info(os.listdir(rtos_sdk_dir))
                file_list = self.common.re_match_list(f".*{sdk_reversion}.*", os.listdir(rtos_sdk_dir))
                if not len(file_list):
                    self.logger.info("版本时间戳不匹配，rtos进行重新安装")
                else:
                    self.logger.info("版本时间戳匹配，跳过安装，继续执行")
                    break
            else:
                self.logger.info("Read RTOS-Revsion failed OR opt/hcc目录不存在")
                file_list = ""
            # 读RTOS-Revision失败或者RTOS-Revision内时间戳与当前rpm包时间戳不匹配均要重新安装
            if not len(file_list):
                self.logger.info("Check RTOS SDK version failed!")
                os.chdir(rtos_sdk_dir)
                #并发时需要创建锁
                self.logger.info("rpm_rtos.flag锁定")
                os.mknod(f"/opt/rpm_rtos.flag_{self.common.jenkins_executors}")

                self.logger.info(f"board_version:{board_version}")
                if board_version == "V6":
                    self.logger.info(f"run install.sh")
                    subprocess.run("sh install.sh", shell=True)
                elif board_version == "V5":
                    self.logger.info(f"run install_rtos_sdk.sh")
                    subprocess.run("sh install_rtos_sdk.sh", shell=True)
                #删除锁
                os.remove(f"/opt/rpm_rtos.flag_{self.common.jenkins_executors}")
                self.logger.info("rpm_rtos.flag解除")
                self.logger.info(f"{i+1}th installation attempt")
                os.chdir(self.common.code_path)
            else:
                self.logger.info("Check RTOS SDK version successfully!")
                break
            i = i + 1
        if i == 3:
            err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}Update RTOS SDK failed. Please make sure that you have the permission and the SDK packages are exist")
            os._exit(1)
        return


    def run(self):
        if not self.config.from_source:
            self.logger.info("from source build, judging board version =======================>")
            self.set_global()
            self.download_tools()
        else:
            self.common.copy_all(f"{self.common.code_root}/tools/build_tools", f"{self.common.temp_path}/build_tools")
            self.logger.info("move dependency tools done")

    def install(self):
        # 检查rtos是否安装，未安装或版本不匹配时安装
        if self.board_version == "V6":
            self.check_rtos_sdk("V6")
        elif self.board_version == "V5":
            self.check_rtos_sdk("V5")


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkDownloadBuildTools(cfg)
    wk.run()
    wk.install()
